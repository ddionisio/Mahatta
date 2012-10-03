#include "game_lua.h"
#include "game_lua_i.h"
#include "game_local.h"

using namespace Lua;

/*+%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%% GENERAL %%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%-*/

static int GetKeyBuffer (lua_State * L)
{
	inpState buffer[64];

	u8 count = g_gi.INPKBGetBufferStates(buffer);

	lua_createtable(L, count, 0);	// {}

	for (u8 index = 0; index < count; ++index)
	{
		lua_createtable(L, 0, 2);	// { ... }, {}
		lua_pushinteger(L, buffer[index].ofs);	// { ... }, {}, key
		lua_setfield(L, 2, "ofs");	// { ... }, { ofs = key }
		lua_pushboolean(L, !(buffer[index].state & INPUT_RELEASED));// { ... }, { ofs }, bPushed
		lua_setfield(L, 2, "bDown");// { ... }, { ofs, down = bPushed }
		lua_rawseti(L, 1, luaL_getn(L, 1) + 1);	// { ..., new top = { ofs, down } }
	}

	return 1;
}

static int IsPressed (lua_State * L)
{
	lua_pushboolean(L, g_gi.INPIsPressed(eInputType(sI(L, 1)), uI(L, 2)));

	return 1;
}

static int MouseGetMove (lua_State * L)
{
	s32 dx, dy, dz;		g_gi.INPMouseGetMove(&dx, &dy, &dz);

	lua_pushinteger(L, dx);
	lua_pushinteger(L, dy);
	lua_pushinteger(L, dz);

	return 3;
}

/*+%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%% GLOBALS %%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%-*/

static int _cx (lua_State * L)
{
	iPoint point;	g_gi.INPMouseGetLoc(&point);

	lua_pushinteger(L, point.x);

	return 1;
}

static int _cy (lua_State * L)
{
	iPoint point;	g_gi.INPMouseGetLoc(&point);

	lua_pushinteger(L, point.y);

	return 1;
}

/// @brief Binds input utilities to the Lua scripting system
void LuaI::open_input (lua_State * L)
{
	// Bind globals.
	Call(L, "BindGlobal", 0, "s f f", "cx", _cx, NOP);
	Call(L, "BindGlobal", 0, "s f f", "cy", _cy, NOP);

	// Install input routines.
	luaL_reg funcs[] = {
		{ "GetKeyBuffer", GetKeyBuffer },
		{ "IsPressed", IsPressed },
		{ "MouseGetMove", MouseGetMove },
		{ 0, 0 }
	};

	luaL_register(L, "inp", funcs);	// { ... }

	// Load constants.
	struct {
		char const * mName;	///< Constant name
		int mValue;	///< Constant value
	} consts[] = {
		{ "Keyboard", INP_TYPE_KEYBOARD },
		{ "Mouse", INP_TYPE_MOUSE },
		{ "Joystick", INP_TYPE_JOYSTICK },
		{ "_escape", INP_ESCAPE },
		{ "_tab", INP_TAB },
		{ "_left", INP_LEFT },
		{ "_right", INP_RIGHT },
		{ "_down", INP_DOWN },
		{ "_up", INP_UP }
	};

	for (int index = 0; index < sizeof(consts) / sizeof(*consts); ++index)
	{
		lua_pushinteger(L, consts[index].mValue);	// { ... }, value
		lua_setfield(L, -2, consts[index].mName);	// { ..., name = value }
	}

	lua_pop(L, 1);
}