#include "game_lua.h"
#include "game_lua_i.h"

// Bindings
#include "game_ball.h"
#include "game_match.h"
#include "game_steering.h"
#include "game_team.h"
#include "game_teamplayer.h"
#include "trajectory.h"

using namespace Lua;

/*+%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%% ROUTINES %%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%-*/

static int CmdPostStr (lua_State * L)
{
	g_gi.CmdPostStr(WS_(L, 1)->c_str());

	return 0;
}

static int GetTick (lua_State * L)
{
	lua_pushnumber(L, g_gi.EngineGetTick());

	return 1;
}

static int HUDRender (lua_State * L)
{
	::HUDRender();

	return 0;
}

static int LoadMap (lua_State * L)
{
	tCHAR buff[MAXCHARBUFF]={0}, path[MAXCHARBUFF]={0};

	///////////////////////////////////////
	//load map
	if(g_gi.ValueGetStr(L"mapstart", buff, MAXCHARBUFF)
		&& g_gi.WorldGetPostMapPath()[0] == 0)
	{
		g_gi.EngineLoadDisplayText(g_game.dbgFnt, L"Loading map...");
		g_gi.WorldLoad(buff);
	}

	return 0;
}

static int UpdateCamera (lua_State * L)
{
	//update camera to look-at player
	if(g_game.view)
	{
		//g_gi.ViewSetListener(g_game.view, Vec3D(0,0,0));
		//SoundSetListener
		Vec3D pt(g_game.view->GetPt()), tgt(g_game.view->GetTgt()), up(0,1,0), fwd;
		
		fwd = tgt-pt; fwd.y = 0;

		g_gi.SoundSetListener(tgt, Vec3D(0,0,0), fwd, up);

		g_game.view->CollideMap(g_gi.WorldGetMap());

		if(g_game.view->m_bStereo)
		{
			Vec3D vView(g_game.view->GetTgt() - g_game.view->GetPt());
			g_game.view->m_dv = Vec3DLen(&vView);//*2;
			//convert to inches...
		}

		g_game.view->Update();
	}

	return 0;
}

static int WorldLoadPostMap (lua_State * L)
{
	g_gi.WorldLoadPostMap();

	return 0;
}

/*+%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%% hCFG %%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%-*/

static GetItemStr (lua_State * L)
{
	tCHAR str[MAXCHARBUFF];

	if (_hT<hCFG>(L, 1)->GetItemStr(WS_(L, 2)->c_str(), WS_(L, 3)->c_str(), str, MAXCHARBUFF))
	{
		return _copyT(L, std::wstring(str));
	}

	return 0;
}

//
// hCFG metamethods
//
static int hCFG_gc (lua_State * L)
{
	return _gcT<hCFG>(L);
}

//
// hCFG new
//
static int hCFG_New_WS (lua_State * L)
{
	*(hCFG*)UD(L, 1) = g_gi.CfgFileLoad(WS_(L, 2)->c_str());

	return 0;
}

static int hCFG_New_C (lua_State * L)
{
	return _refT<hCFG>(L);
}

///
/// hCFG utilities
///
static int _hCFG (lua_State * L)
{
	return GenericNew(L, "hCFG", 1);
}

/// @brief Defines the hCFG class
static void define_hCFG (lua_State * L)
{
	// Supply an overloaded new.
	Overload hCFG_Cons(L, 2);	// G
		hCFG_Cons.AddDef(hCFG_New_WS, "hCFG", "WS");
		hCFG_Cons.AddDef(hCFG_New_C, "hCFG", "userdata");
		hCFG_Cons.AddDef(hCFG_New_C, "hCFG", "hCFG");

	// Define the config file.
	luaL_reg funcs[] = {
		{ "__gc", hCFG_gc },
		{ 0, 0 }
	};

	class_Define(L, "Cfg", funcs, 0, sizeof(hCFG));	// stack clear

	// Install a helper function.
	lua_register(L, "hCFG", _hCFG);
}

/*+%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%% hENTITY %%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%-*/

//
// hENTITY metamethods
//
static int hENTITY_gc (lua_State * L)
{
	return _gcT<hENTITY>(L);
}

//
// hENTITY getters
//
static int EGet_ (lua_State * L, Vec3D Entity::* member)
{
	class_New(L, "RVec3D", "u", &(_hT<hENTITY>(L, 1)->*member));

	return 1;
}

static int EGet_boundMax (lua_State * L)
{
	return EGet_(L, &Entity::m_boundMax);
}

static int EGet_boundMin (lua_State * L)
{
	return EGet_(L, &Entity::m_boundMin);
}

static int EGet_dir (lua_State * L)
{
	return EGet_(L, &Entity::m_dir);
}

static int EGet_loc (lua_State * L)
{
	return EGet_(L, &Entity::m_loc);
}

static int EGet_vel (lua_State * L)
{
	return EGet_(L, &Entity::m_vel);
}

//
// hENTITY setters
//
static int ESet_ (lua_State * L, Vec3D Entity::* member)
{
	_hT<hENTITY>(L, 1)->*member = *Vec3D_(L, 3, "assign");

	return 0;
}

static int ESet_boundMax (lua_State * L)
{
	return ESet_(L, &Entity::m_boundMax);
}

static int ESet_boundMin (lua_State * L)
{
	return ESet_(L, &Entity::m_boundMin);
}

static int ESet_dir (lua_State * L)
{
	return ESet_(L, &Entity::m_dir);
}

static int ESet_loc (lua_State * L)
{
	return ESet_(L, &Entity::m_loc);
}

static int ESet_vel (lua_State * L)
{
	return ESet_(L, &Entity::m_vel);
}

//
// hENTITY new
//
static int hENTITY_New_E (lua_State * L)
{
	return _refT<hENTITY>(L);
}

/// @brief Defines the hENTITY class
static void define_hENTITY (lua_State * L)
{
	Member_Reg members[3];

	members[0].Set(offsetof(Entity, m_boundRadius), "boundRadius", Member_Reg::eFloat);
	members[1].Set(offsetof(Entity, m_spd), "spd", Member_Reg::eFloat);
	members[2].Set(offsetof(Entity, m_usrDat), "usrDat", Member_Reg::ePointer);

	luaL_Reg getters[] = {
		{ "boundMax", EGet_boundMax },
		{ "boundMin", EGet_boundMin },
		{ "dir", EGet_dir },
		{ "loc", EGet_loc },
		{ "vel", EGet_vel },
		{ 0, 0 }
	}, setters[] = {
		{ "boundMax", ESet_boundMax },
		{ "boundMin", ESet_boundMin },
		{ "dir", ESet_dir },
		{ "loc", ESet_loc },
		{ "vel", ESet_vel },
		{ 0, 0 }
	}, funcs[] = {
		{ "__gc", hENTITY_gc },
		{ 0, 0 }
	};
	
	MembersBind(L, getters, setters, members, 3, ePointerTo, 0, false);	// __index, __newindex

	// Supply an overloaded new.
	Overload hENTITY_Cons(L, 2);// __index, __newindex, G
		hENTITY_Cons.AddDef(hENTITY_New_E, "hENTITY", "userdata");
		hENTITY_Cons.AddDef(hENTITY_New_E, "hENTITY", "hENTITY");

	char const * EClosures[] = { "__index", "__newindex", 0 };

	class_Define(L, "hENTITY", funcs, EClosures, 0, sizeof(hENTITY));
}

/*+%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%% WIDE STRINGS %%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%-*/

//
// Wide string utilities
//
template<> char const * Lua::_typeT<std::wstring> (void)
{
	return "WS";
}

std::wstring * Lua::WS_ (lua_State * L, int index)
{
	return _pT<std::wstring>(L, index);
}

static int WS_new (lua_State * L, std::wstring & ws)
{
	return _copyT(L, ws);
}

//
// Wide string metamethods
//
static int WS_eq (lua_State * L)
{
	lua_pushboolean(L, *WS_(L, 1) == *WS_(L, 2));

	return 1;
}

static int WS_concat (lua_State * L)
{
	return WS_new(L, *WS_(L, 1) + *WS_(L, 2));
}

static int WS_gc (lua_State * L)
{
	WS_(L, 1)->~wstring();

	return 0;
}

static int WS_le (lua_State * L)
{
	lua_pushboolean(L, *WS_(L, 1) <= *WS_(L, 2));

	return 1;
}

static int WS_len (lua_State * L)
{
	lua_pushinteger(L, WS_(L, 1)->length());

	return 1;
}

static int WS_lt (lua_State * L)
{
	lua_pushboolean(L, *WS_(L, 1) < *WS_(L, 2));

	return 1;
}

//
// Wide string methods
//
static int WS_Dup (lua_State * L)
{
	return WS_new(L, *WS_(L, 1));
}

//
// Wide string new
//
static int WSNew_String (lua_State * L)
{
	size_t count = lua_objlen(L, 2);

	try {
		wchar_t * wc = new wchar_t[count];

		mbstowcs(wc, lua_tostring(L, 2), count);

		new (UD(L, 1)) std::wstring(wc, count);

		delete [] wc;
	} catch (std::bad_alloc &) {}

	return 0;
}

static int WSNew_WS (lua_State * L)
{
	new (UD(L, 1)) std::wstring(*WS_(L, 2));

	return 0;
}

///
/// Wide string utilities
///
static int _WS (lua_State * L)
{
	return GenericNew(L, "WS", 1);
}

/// @brief Defines the wide string class
static void define_WS (lua_State * L)
{
	// Supply an overloaded new.
	Overload WS_Cons(L, 2);	// G
		WS_Cons.AddDef(WSNew_String, "WS", "string");
		WS_Cons.AddDef(WSNew_WS, "WS", "userdata");
		WS_Cons.AddDef(WSNew_WS, "WS", "WS");

	// Define the wide string.
	luaL_reg funcs[] = {
		{ "__eq", WS_eq },
		{ "__concat", WS_concat },
		{ "__gc", WS_gc },
		{ "__le", WS_le },
		{ "__len", WS_len },
		{ "__lt", WS_lt },
		{ "Dup", WS_Dup },
		{ 0, 0 }
	};

	class_Define(L, "WS", funcs, 0, sizeof(std::wstring));// stack clear

	// Install helper functions.
	lua_register(L, "WS", _WS);
	lua_register(L, "L", _WS);
}

/*+%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%% GLOBALS %%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%-*/

static int _gravity (lua_State * L)
{
	class_New(L, "Vec3D", "u", &g_gi.WorldGetGravity());

	return 1;
}

static int _player (lua_State * L)
{
	if (0 == g_game.player) return 0;

	Team * pTeam = g_game.player->GetTeam();

	u8 player = 0;

	while (g_game.player != pTeam->getPlayer(player)) ++player;

	Invoke(L, pTeam == Match::getMe()->getTeam(1) ? "team1" : "team2", "getPlayer", 1, "i", player);// player

	return 1;
}

static int _viewbLook (lua_State * L)
{
	lua_pushboolean(L, g_game.viewbLook != 0);

	return 1;
}

/// @brief Binds game logic and classes to Lua
void Lua::BindGameLogic (void)
{
	// Bind globals.
	Call(g_game.mLua, "BindGlobal", 0, "s f f", "gravity", _gravity, NOP);
	Call(g_game.mLua, "BindGlobal", 0, "s f f", "player", _player, NOP);
	Call(g_game.mLua, "BindGlobal", 0, "s f f", "viewbLook", _viewbLook, NOP);

	// Install functions.
	luaL_reg funcs[] = {
		{ "CmdPostStr", CmdPostStr },
		{ "GetTick", GetTick },
		{ "HUDRender", HUDRender },
		{ "LoadMap", LoadMap },
		{ "UpdateCamera", UpdateCamera },
		{ "WorldLoadPostMap", WorldLoadPostMap },
		{ 0, 0 }
	};

	luaL_register(g_game.mLua, "game", funcs);
	lua_pop(g_game.mLua, 1);

	// Bind game routines.
	luabind_Trajectory();

	// Bind game objects.
	luabind_Ball();
	luabind_Match();
	luabind_Steering();
	luabind_Team();
	luabind_TeamPlayer();

	// Define game classes.
	define_hCFG(g_game.mLua);
	define_hENTITY(g_game.mLua);
	define_WS(g_game.mLua);

	// Load game scripts.
	if (luaL_dofile(g_game.mLua, "Main/Scripts/Boot.txt") != 0)
	{
		Call(g_game.mLua, "printf", 0, "a", -1);

		lua_pop(g_game.mLua, 1);
	}
}