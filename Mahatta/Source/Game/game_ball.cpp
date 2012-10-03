#include "game_ball.h"
#include "game_teamplayer.h"
#include "game_lua.h"
#include "game_lua_i.h"
#include "game_local.h"
#include "trajectory.h"

Ball::Ball() : ball(0) {

	ballInit ballI;

	// read ini file for ball
	tCHAR buff[MAXCHARBUFF];
	hCFG cfg = g_gi.CfgFileLoad(L"Data\\Entity\\ball.ini");

	// read sounds and fill ballInit structure
	cfg->GetItemStr(L"sound", L"sndBounce", buff, MAXCHARBUFF);
	g_gi.SoundSysGetID(buff, &ballI.mSndBounce);
	cfg->GetItemStr(L"sound", L"sndExplode", buff, MAXCHARBUFF);
	g_gi.SoundSysGetID(buff, &ballI.mSndExplode);

	// read model file and create ball
	cfg->GetItemStr(L"main", L"mdl", buff, MAXCHARBUFF);
	ball = BallCreate(buff, ballI);

	// check if ball entity is already on world, if not, attach it
	if(ball && !g_gi.WorldEntityExist(ball))
		g_gi.WorldAddEntity(ball);

	// set ball's initial stuff

	cfg->Release();
}

Ball::~Ball(){
	// let game_match take care of the instance
}

Ball* Ball::getMe(){
	return Lua::_toclassT<Ball>(g_game.mLua, "ball");
}

void Ball::reset(){
	
	//* in front of currently controlled player
	Vec3D pos = g_game.player->GetEnt()->m_loc + g_game.player->GetEnt()->m_dir * 40;
	pos.y += Ball::getMe()->getEnt()->m_boundMax.y;

	setPos(pos);
}

///
/// LUA
///
using namespace Lua;

template<> char const * Lua::_typeT<Ball> (void)
{
	return "Ball";
}

static int getEnt (lua_State * L)
{
	class_New(L, "hENTITY", "u", _pT<Ball>(L, 1)->getEnt());

	return 1;
}

static int GetPath (lua_State * L)
{
	std::list<Ball::PathItem> const & path = _pT<Ball>(L, 1)->GetPath();

	lua_createtable(L, int(path.size()), 0);// ball, {}

	f32 lapse = 0.0f, times[2];

	if (!path.empty())
	{
		Vec3D moved = *_pT<Ball>(L, 1)->getPos() - path.front().mPosition;

		Trajectory bt(path.front().mPosition, path.front().mVelocity);

		if (!bt.GetXTime(Vec3DxzL(&moved), lapse))
		{
			u8 count = bt.GetYTimes(moved.y, times);

		//	lapse = times[y < 0 ? count - 1 : 0];
		}
	}

	for (std::list<Ball::PathItem>::const_iterator pi = path.begin(); pi != path.end(); ++pi)
	{
		lua_createtable(L, 0, 5);	// ball, { ... }, {}

		class_New(L, "Vec3D", "u", &pi->mPosition);	// ball, { ... }, {}, pos
		class_New(L, "Vec3D", "u", &pi->mVelocity);	// ball, { ... }, {}, pos, vel

		lua_setfield(L, 3, "vel");	// ball, { ... }, { vel }, pos
		lua_setfield(L, 3, "pos");	// ball, { ... }, { pos, vel }
		lua_pushnumber(L, pi->mTime - lapse);	// ball, { ... }, { pos, vel }, time
		lua_setfield(L, 3, "time");	// ball, { ... }, { pos, vel, time }

		if (pi->mTeam1)
		{
			lua_pushboolean(L, true);	// ball, { ... }, { pos, vel, time }, true
			lua_setfield(L, 3, "bTeam1");	// ball, { ... }, { pos, vel, time, bTeam1 }
		}

		if (pi->mTeam2)
		{
			lua_pushboolean(L, true);	// ball, { ... }, { pos, vel, time[, bTeam1] }, true
			lua_setfield(L, 3, "bTeam2");	// ball, { ... }, { pos, vel, time[, bTeam1], bTeam2 }
		}

		lua_rawseti(L, 2, luaL_getn(L, 2) + 1);	// ball, { ..., new top = { pos, vel, time[, bTeam1, bTeam2] }
	}

	return 1;
}

static int GetVel (lua_State * L)
{
	class_New(L, "Vec3D", "u", &_pT<Ball>(L, 1)->GetVel());

	return 1;
}

static int hit (lua_State * L)
{
	_pT<Ball>(L, 1)->hit(*Vec3D_(L, 2));

	return 0;
}

static int setPos (lua_State * L)
{
	_pT<Ball>(L, 1)->setPos(*Vec3D_(L, 2));

	return 0;
}

static int Ball_gc (lua_State * L)
{
	_pT<Ball>(L, 1)->~Ball();

	return 0;
}

void LuaBall_A (void * pData)
{
	new (pData) Ball;
}

static int Ball_New (lua_State * L)
{
	LuaBall_A(UD(L, 1));

	return 0;
}

void luabind_Ball (void)
{
	// Define the ball.
	luaL_reg funcs[] = {
		{ "__gc", Ball_gc },
		{ "getEnt", getEnt },
		{ "GetPath", GetPath },
		{ "GetVel", GetVel },
		{ "hit", hit },
		{ "setPos", setPos },
		{ 0, 0 }
	};

	Lua::class_Define(g_game.mLua, "Ball", funcs, Ball_New, 0, sizeof(Ball));
}