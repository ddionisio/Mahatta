#include "trajectory.h"
#include "game_lua.h"
#include <algorithm>
#include <cmath>

///
/// Trajectory
///
static const float _Pi = float(PI);
static const float _HalfPi = _Pi / 2;

/// @brief Gets the angles matching the parameters
/// @param x Target x, relative to launch x
/// @param y Target y, relative to launch y
/// @param speed Launch speed
/// @param gravity Gravity constant
/// @param angleOut [out] Numerically ordered angles
/// @return Count of angles
u8 TrajectGetAngles (f32 x, f32 y, f32 speed, f32 gravity, f32 angleOut[2])
{
	if (x < 0.0f) return 0;
	if (speed <= 0.0f) return 0;
	if (gravity <= 0.0f) return 0;

	// Ensure that angles will lie in the bounding parabola.
	// radius: Distance to target
	// factor: [y + g·(x/v)²]/radius
	// Within parabola: factor <= 1
	float fX2 = x * x;
	float fNumerator = y + gravity * fX2 / (speed * speed), fRadius = sqrtf(fX2 + y * y);

	u8 count = 0;

	if (fNumerator <= fRadius)
	{
		// Angle to (x, y): theta = arcsin(y/radius)
		// Solution A: sin(2·A - theta) = factor
		// A = (arcsin(factor) + theta)/2
		// Solution B: sin(a) = sin(pi - a) -> sin(theta + pi - 2·B) = factor
		// B = (theta + pi - arcsin(factor))/2
		float fIncline = asinf(y / fRadius), fTerm = asinf(fNumerator / fRadius);
		float fValues[2] = { +0.5f * (fTerm + fIncline), -0.5f * (fTerm - fIncline - _Pi) };

		// Keep angles found in the right semicircle. If this includes both, sort them by
		// increasing radian. Return these angles.
		for (int index = 0; index < 2; ++index)
		{
			if (fValues[index] <= -_HalfPi) continue;
			if (fValues[index] >= +_HalfPi) continue;
			
			angleOut[count++] = fValues[index];
		}

		if (count == 2 && angleOut[1] < angleOut[0]) std::swap(angleOut[0], angleOut[1]);
	}

	return count;
}

/// @brief Gets the speed matching the parameters
/// @param x Target x, relative to launch x
/// @param y Target y, relative to launch y
/// @param angle Launch angle
/// @param gravity Gravity constant
/// @param speedOut [out] Speed
/// @return 0 on failure; 1 otherwise
u8 TrajectGetSpeed (f32 x, f32 y, f32 angle, f32 gravity, f32 & speedOut)
{
	if (x < 0.0f) return 0;
	if (angle <= -_HalfPi || angle >= +_HalfPi) return 0;
	if (gravity <= 0.0f) return 0;

	// x(t) = v·cos·t
	// y(x/[v·cos]) = x·tan - g·[x/(v·cos)]²/2
	// 2·y·cos² = 2·x·cos·sin - g·(x/v)²
	// v² = g·x²/[2·cos·(x·sin - y·cos)]
	// v = x·(g·sec/[2·(x·sin - y·cos)])½
	float fSin = sinf(angle);
	float fCos = sqrtf(1.0f - fSin * fSin);
	float fDisc = x * fSin - y * fCos;

	if (fDisc < 0.0f) return 0;

	speedOut = x * sqrtf(0.5f * gravity / (fDisc * fCos));

	return 1;
}

/// @brief Constructs a Trajectory
/// @param start Start position of curve in space
/// @param launch Launch velocity
Trajectory::Trajectory (Vec3D const & start, Vec3D const & launch)
{
	Build(start, launch);
}

/// @brief Builds a trajectory curve
/// @param start Start position of curve in space
/// @param launch Launch velocity
void Trajectory::Build (Vec3D const & start, Vec3D const & launch)
{
	mStart = start;
	mLaunch = launch;
	mGravity = g_gi.WorldGetGravity();	///< Worth generalizing?
}

/// @brief Gets the maximum height on the curve
/// @param maxHeight [out] Maximum height
void Trajectory::GetMaxHeight (f32 & maxH)
{
	// If the angle is non-positive, the trajectory will never be upward, and thus peaks at
	// the start of the curve. Otherwise, compute the height when gravity matches ascent.
	if (Vec3DDot(&mLaunch, &mGravity) >= 0.0f) maxH = 0.0f;

	// dy/dt = 0 -> v·sin = g·t
	// y(v·sin/g) = (v·sin)²/g - (v·sin)²/(2·g)
	// y = (v·sin)²/(2·g)
	else maxH = -0.5f * mLaunch.y * mLaunch.y / mGravity.y;
}

/// @brief Gets a position and velocity on the curve
/// @param t Time on curve
/// @param pOut [out] Position at curve(t)
/// @param vOut [out] Velocity at curve(t)
void Trajectory::GetPosVel (f32 t, Vec3D & pOut, Vec3D & vOut)
{
	pOut = GetPosition(t);
	vOut = GetVelocity(t);
}

/// @brief Gets a position on the curve
/// @param t Time on curve
/// @return Position at curve(t)
Vec3D Trajectory::GetPosition (f32 t)
{
	// x(t) = x0 + v·cos·t
	// y(t) = y0 + v·sin·t - g·t²/2
	return mStart + t * (mLaunch + 0.5f * mGravity * t);
}

/// @brief Gets a velocity on the curve
/// @param t Time on curve
/// @return Velocity at curve(t)
Vec3D Trajectory::GetVelocity (f32 t)
{
	// x'(t) = v·cos
	// y'(t) = v·sin - g·t
	return mLaunch + mGravity * t;
}

/// @brief Gets the average curve height
/// @param dx Tangential distance along curve
/// @param avgH [out] Maximum height
/// @return If true, height was found
bool Trajectory::GetAverageHeight (f32 dx, f32 & avgH)
{
	f32 xzL = Vec3DxzL(&mLaunch);

	if (xzL < 1e-3f) return false;
	if (dx < 0.0f) return false;

	// avg y(t) = integral(y(t), dt, 0, t)/(T - 0)
	// avg y(t) = [v·sin·t²/2 - g·t³/6]/T, 0, T
	// avg y(t) = v·sin·T/2 - g·T²/6
	// x(T) = v·cos·T
	// T = x/(v·cos)
	float fT = dx / xzL;

	avgH = fT * (0.5f * mLaunch.y + mGravity.y * fT / 6.0f);

	return true;
}

/// @brief Gets the time along the curve
/// @param dx Tangential distance along curve
/// @param tOut [out] Time
/// @return If true, time was found
bool Trajectory::GetXTime (f32 dx, f32 & tOut)
{
	f32 xzL = Vec3DxzL(&mLaunch);

	if (xzL < 1e-3f) return false;
	if (dx < 0.0f) return false;

	// x(t) = v·cos·t
	// t = x/(v·cos)
	tOut = dx / xzL;

	return true;
}

/// @brief Gets the times when the curve reaches the height
/// @param dy Normal distance along curve
/// @param tOut [out] Times
/// @return Count of times
u8 Trajectory::GetYTimes (f32 dy, f32 tOut[2])
{
	// y(t) = v·sin·t - g·t²/2
	// a: -g/2
	// b: v·sin
	// c: -y
	// t = (v·sin ± [(v·sin)² - 2·g·y]½)/g
	f32 fDisc = mLaunch.y * mLaunch.y + 2.0f * mGravity.y * dy;

	u8 count = 0;

	if (fDisc >= 0.0f)
	{
		f32 fRoot = sqrtf(fDisc);

		// Find the lesser and greater times that satisfy the height parabola. Choose the
		// non-negative solutions.
		if (-mLaunch.y >= +fRoot) tOut[count++] = (-mLaunch.y - fRoot) / mGravity.y;
		if (-mLaunch.y >= -fRoot) tOut[count++] = (-mLaunch.y + fRoot) / mGravity.y;

		if (count == 2 && tOut[1] < tOut[0]) std::swap(tOut[0], tOut[1]);
	}

	return count;
}

///
/// LUA
///
using namespace Lua;

template<> char const * Lua::_typeT<Trajectory> (void)
{
	return "Trajectory";
}

static int GetAngles (lua_State * L)
{
	f32 angles[2];

	u8 count = TrajectGetAngles(F(L, 1), F(L, 2), F(L, 3), F(L, 4), angles);

	for (u8 index = 0; index < count; ++index) lua_pushnumber(L, angles[index]);

	return count;
}

static int GetSpeed (lua_State * L)
{
	float speed;

	if (TrajectGetSpeed(F(L, 1), F(L, 2), F(L, 3), F(L, 4), speed) != 0)
	{
		lua_pushnumber(L, speed);

		return 1;
	}

	return 0;
}

static int Build (lua_State * L)
{
	_pT<Trajectory>(L, 1)->Build(*Vec3D_(L, 2), *Vec3D_(L, 3));

	return 0;
}

static int GetAverageHeight (lua_State * L)
{
	float avgh;

	if (_pT<Trajectory>(L, 1)->GetAverageHeight(F(L, 2), avgh))
	{
		lua_pushnumber(L, avgh);

		return 1;
	}

	return 0;
}

static int GetMaxHeight (lua_State * L)
{
	float maxh;

	_pT<Trajectory>(L, 1)->GetMaxHeight(maxh);

	lua_pushnumber(L, maxh);

	return 1;
}

static int GetPosition (lua_State * L)
{
	Vec3D pos = _pT<Trajectory>(L, 1)->GetPosition(F(L, 2));

	class_New(L, "Vec3D", "u", &pos);

	return 1;
}

static int GetPosVel (lua_State * L)
{
	Vec3D pos, vel;

	_pT<Trajectory>(L, 1)->GetPosVel(F(L, 2), pos, vel);

	class_New(L, "Vec3D", "u", &pos);
	class_New(L, "Vec3D", "u", &vel);

	return 2;
}

static int GetVelocity (lua_State * L)
{
	Vec3D vel = _pT<Trajectory>(L, 1)->GetVelocity(F(L, 2));

	class_New(L, "Vec3D", "u", &vel);

	return 1;
}

static int GetXTime (lua_State * L)
{
	float time;

	if (_pT<Trajectory>(L, 1)->GetXTime(F(L, 2), time))
	{
		lua_pushnumber(L, time);

		return 1;
	}

	return 0;
}

static int GetYTimes (lua_State * L)
{
	f32 times[2];

	u8 count = _pT<Trajectory>(L, 1)->GetYTimes(F(L, 2), times);

	for (u8 index = 0; index < count; ++index) lua_pushnumber(L, times[index]);

	return count;
}

static int Trajectory_gc (lua_State * L)
{
	_pT<Trajectory>(L, 1)->~Trajectory();

	return 0;
}

static int Trajectory_New (lua_State * L)
{
	new (UD(L, 1)) Trajectory(*Vec3D_(L, 2), *Vec3D_(L, 3));

	return 0;
}

void luabind_Trajectory (void)
{
	luaL_reg funcs[] = {
		{ "GetAngles", GetAngles },
		{ "GetSpeed", GetSpeed },
		{ 0, 0 }
	};

	luaL_register(g_game.mLua, "traject", funcs);

	// Define the trajectory.
	luaL_reg methods[] = {
		{ "Build", Build },
		{ "__gc", Trajectory_gc },
		{ "GetAverageHeight", GetAverageHeight },
		{ "GetMaxHeight", GetMaxHeight },
		{ "GetPosition", GetPosition },
		{ "GetPosVel", GetPosVel },
		{ "GetVelocity", GetVelocity },
		{ "GetXTime", GetXTime },
		{ "GetYTimes", GetYTimes },
		{ 0, 0 }
	};

	class_Define(g_game.mLua, "Trajectory", methods, Trajectory_New, 0, sizeof(Trajectory));
}