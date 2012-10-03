#include "game_steering.h"
#include "game_lua.h"

/// @brief Dummy random number generator
/// @return Random float in [0,1]
static float DummyRandom (void)
{
	return float(rand()) / RAND_MAX;
}

/// @brief Accumulates valid forces and manages result
/// @return Resultant force
Vec3D Steering::GetFinalForce (void)
{
	Vec3D force(0.0f, 0.0f, 0.0f);

	for (std::list<Force>::iterator oi = mOrder.begin(); oi != mOrder.end(); ++oi)
	{
		if (!mStatus.test(*oi)) continue;

		force += mForces[*oi];
	}

	return force;
}

/// @brief Adds a force to be applied
/// @param which Force to assign
/// @param force Force vector
void Steering::AddForce (Force which, Vec3D const & force)
{
	Property props[] = {
		eAlignmentScale, eCohesionScale, eSeparationScale,
		eTargetScale,
		eWallScale,
		eWanderScale
	};

	mForces[which] = force * GetProperty(props[which]);
}

/// @brief Applies group force
/// @param ent Entity to steer
/// @param entities Interactive objects
void Steering::GetGroupForces (hENTITY ent, std::list<hENTITY> & entities)
{
	Vec3D heading(0.0f, 0.0f, 0.0f), center(0.0f, 0.0f, 0.0f), separation(0.0f, 0.0f, 0.0f);

	for (std::list<hENTITY>::iterator ei = entities.begin(); ei != entities.end(); ++ei)
	{
		if (mStatus.test(eAlignment)) heading += (*ei)->m_dir;
		if (mStatus.test(eCohesion)) center += (*ei)->m_loc;
		if (mStatus.test(eSeparation))
		{
			Vec3D diff = ent->m_loc - (*ei)->m_loc;

			separation += diff / Vec3DDot(&diff, &diff);
		}
	}

	if (mStatus.test(eAlignment)) AddForce(eAlignment, heading / float(entities.size()) - ent->m_dir);
	if (mStatus.test(eCohesion)) AddForce(eCohesion, center / float(entities.size()) - ent->m_loc);
	if (mStatus.test(eSeparation)) AddForce(eSeparation, separation);
}

/// @brief Applies a target force
/// @param ent Entity to steer
void Steering::GetTargetForce (hENTITY ent)
{
	// Direct the player toward its target at top speed.
	Vec3D vXZ = mTarget - ent->m_loc, seek(-ent->m_vel);

	vXZ.y = 0.0f;

	if (Vec3DLenSQ(&vXZ) > 0.0f)
	{
		float cofactor = Vec3DLen(&vXZ) / GetProperty(eTargetApproach);

		Vec3D vXZs;	Vec3DScaleTo(&vXZ, GetProperty(eTargetSpeed), &vXZs);
		
		seek += vXZs * (cofactor < 1.0f ? cofactor : 1.0f);
	}

	AddForce(eTarget, seek);
}

/// @brief Applies a wander force
/// @param ent Entity to steer
void Steering::GetWanderForce (hENTITY ent)
{
	float angle = float(PI / 2) + GetProperty(eWanderAngle) * (mRandomFunc() - 0.5f);

	float cos = cosf(angle), sin = sinf(angle);

	Vec3D side(-ent->m_dir.z, ent->m_dir.y, +ent->m_dir.x);

	Vec3D veer = side * (cos >= 0.0f ? +sin : -sin) + ent->m_dir * fabsf(cos);

	AddForce(eWander, ent->m_dir * GetProperty(eWanderDistance) + veer * GetProperty(eWanderRadius));
}

/// @brief Constructs a Steering object
Steering::Steering (void) : mForces(eNForces), mProperties(eNProperties, 1.0f), mRandomFunc(DummyRandom), mTarget(0.0f, 0.0f, 0.0f)
{
	Force order[] = {
		eTarget,
		eAlignment, eCohesion, eSeparation,
		eWall,
		eWander
	};
	
	for (int index = 0; index < sizeof(order) / sizeof(Force); ++index) mOrder.push_back(order[index]);
}

/// @brief Destructs a Steering object
Steering::~Steering (void)
{
}

/// @brief Gets the steering's force priority order
/// @return Ordered force type list
std::list<Steering::Force> Steering::GetOrder (void)
{
	return mOrder;
}

/// @brief Gets a steering force
/// @param ent Entity to steer
/// @param entities Interactive objects
/// @param qbsp BSP collision tree
/// @return Force vector
Vec3D Steering::GetForce (hENTITY ent, std::list<hENTITY> & entities, hQBSP qbsp)
{
	mForces.assign(eNForces, Vec3D(0.0f, 0.0f, 0.0f));

	if (entities.size() != 0) GetGroupForces(ent, entities);
	if (mStatus.test(eTarget)) GetTargetForce(ent);
	if (mStatus.test(eWander)) GetWanderForce(ent);

	return GetFinalForce();
}

/// @brief Gets a property value
/// @param property Property type
/// @return Property value; 1.0f if absent
float Steering::GetProperty (Property property)
{
	return mProperties[property];
}

/// @brief Enables or disables a steering force
/// @param force Force to manage
/// @param bEnable If true, enable force
void Steering::EnableForce (Force force, bool bEnable)
{
	mStatus[force] = bEnable;
}

/// @brief Assigns the steering's force priority order
/// @param order Ordered force type list
void Steering::SetOrder (std::list<Force> const & order)
{
	mOrder = order;
}

/// @brief Assigns a property value
/// @param property Property type
/// @param value Value to assign
void Steering::SetProperty (Property property, float value)
{
	mProperties[property] = value;
}

/// @brief Assign the random function
/// @param randomFunc Random function; if 0, restore the default
void Steering::SetRandomFunc (RandomFunc randomFunc)
{
	mRandomFunc = randomFunc != 0 ? randomFunc : DummyRandom;
}

/// @brief Sets targeting properties
/// @param target Target point
void Steering::SetTarget (Vec3D const & target)
{
	mTarget = target;
}

///
/// LUA
///
using namespace Lua;

template<> char const * Lua::_typeT<Steering> (void)
{
	return "Steering";
}

static int EnableForces (lua_State * L)
{
	Steering * pSteering = _pT<Steering>(L, 1);

	for (int index = 0; index < Steering::eNForces; ++index) pSteering->EnableForce(Steering::Force(index), false);

	for (int index = luaL_getn(L, 2); index > 0; --index)
	{
		lua_rawgeti(L, 2, index);	// S, forces, name
		lua_gettable(L, lua_upvalueindex(1));	// S, forces, force

		if (!lua_isnil(L, 3)) pSteering->EnableForce(Steering::Force(uI(L, 3)), true);

		lua_pop(L, 1);	// S, forces
	}

	return 0;
}

static int GetProperty (lua_State * L)
{
	lua_gettable(L, lua_upvalueindex(1));	// S, property

	if (!lua_isnil(L, 2)) lua_pushnumber(L, _pT<Steering>(L, 1)->GetProperty(Steering::Property(uI(L, 2))));	// S, property, value

	return 1;
}

static int SetProperties (lua_State * L)
{
	Steering * pSteering = _pT<Steering>(L, 1);

	for (int index = 0; index < Steering::eNProperties; ++index) pSteering->SetProperty(Steering::Property(index), 1.0f);

	lua_pushnil(L);	// S, properties, nil

	while (lua_next(L, 2) != 0)	// S, properties, name, value
	{
		lua_pushvalue(L, 3);// S, properties, name, value, name
		lua_gettable(L, lua_upvalueindex(1));	// S, properties, name, value, property

		if (!lua_isnil(L, 5)) pSteering->SetProperty(Steering::Property(uI(L, 5)), F(L, 4));

		lua_pop(L, 2);	// S, properties, name
	}

	return 0;
}

static int SetTarget (lua_State * L)
{
	_pT<Steering>(L, 1)->SetTarget(*Vec3D_(L, 2));

	return 0;
}

static int Steering_gc (lua_State * L)
{
	_pT<Steering>(L, 1)->~Steering();

	return 0;
}

static int Steering_New (lua_State * L)
{
	new (UD(L, 1)) Steering;

	return 0;
}

void luabind_Steering (void)
{
	// Define a force closure.
	struct {
		Steering::Force mForce;	///< Force value
		char const * mName;	///< Force name
	} Forces[] = {
		Steering::eAlignment, "Alignment",
		Steering::eCohesion, "Cohesion",
		Steering::eSeparation, "Separation",
		Steering::eTarget, "Target",
		Steering::eWall, "Wall",
		Steering::eWander, "Wander"
	};

	lua_createtable(g_game.mLua, Steering::eNForces, 0);// {}

	for (int index = 0; index < sizeof(Forces) / sizeof(*Forces); ++index)
	{
		lua_pushinteger(g_game.mLua, Forces[index].mForce);	// { ... }, force
		lua_setfield(g_game.mLua, -2, Forces[index].mName);	// { ..., name = force }
	}

	lua_pushcclosure(g_game.mLua, EnableForces, 1);	// EnableForces

	// Define property closures.
	struct {
		Steering::Property mProperty;	///< Property value
		char const * mName;	///< Property name
	} Properties[] = {
		Steering::eAlignmentScale, "AlignmentScale",
		Steering::eCohesionScale, "CohesionScale",
		Steering::eSeparationScale, "SeparationScale",
		Steering::eTargetApproach, "TargetApproach",
		Steering::eTargetScale, "TargetScale",
		Steering::eTargetSpeed, "TargetSpeed",
		Steering::eWallScale, "WallScale",
		Steering::eWanderAngle, "WanderAngle",
		Steering::eWanderDistance, "WanderDistance",
		Steering::eWanderRadius, "WanderRadius",
		Steering::eWanderScale, "WanderScale"
	};

	lua_createtable(g_game.mLua, Steering::eNProperties, 0);// EnableForces, {}

	for (int index = 0; index < sizeof(Properties) / sizeof(*Properties); ++index)
	{
		lua_pushinteger(g_game.mLua, Properties[index].mProperty);	// EnableForces, { ... }, property
		lua_setfield(g_game.mLua, -2, Properties[index].mName);	// EnableForces, { ..., name = property }
	}

	lua_pushvalue(g_game.mLua, -1);	// EnableForces, properties, properties
	lua_pushcclosure(g_game.mLua, GetProperty, 1);	// EnableForces, properties, GetProperty
	lua_insert(g_game.mLua, -2);// EnableForces, GetProperty, properties
	lua_pushcclosure(g_game.mLua, SetProperties, 1);// EnableForces, GetProperty, SetProperties

	// Define the steering.
	luaL_reg const funcs[] = {
		{ "__gc", Steering_gc },
		{ "SetTarget", SetTarget },
		{ 0, 0 }
	};

	char const * closures[] = { "EnableForces", "GetProperty", "SetProperties", 0 };

	class_Define(g_game.mLua, "Steering", funcs, closures, Steering_New, 0, sizeof(Steering));
}