#include "game_lua.h"
#include "game_lua_i.h"

using namespace Lua;

static int AND (lua_State * L)
{
	lua_pushinteger(L, uI(L, 1) & uI(L, 2));

	return 1;
}

static int ANDS (lua_State * L)
{
	Uint test = uI(L, 2);

	lua_pushboolean(L, (uI(L, 1) & test) == test);

	return 1;
}

static int MOD (lua_State * L)
{
	lua_pushinteger(L, uI(L, 1) % uI(L, 2));
	
	return 1;
}

static int NOT (lua_State * L)
{
	lua_pushinteger(L, ~uI(L, 1));

	return 1;
}

static int OR (lua_State * L)
{
	lua_pushinteger(L, uI(L, 1) | uI(L, 2));

	return 1;
}

static int SHL (lua_State * L)
{
	lua_pushinteger(L, uI(L, 1) << uI(L, 2));

	return 1;
}

static int SHR (lua_State * L)
{
	lua_pushinteger(L, uI(L, 1) >> uI(L, 2));

	return 1;
}

static int XOR (lua_State * L)
{
	lua_pushinteger(L, uI(L, 1) ^ uI(L, 2));

	return 1;
}

/// @brief Binds bit utilities to the Lua scripting system
void LuaI::open_bit (lua_State * L)
{
	static const luaL_reg BitFuncs[] = {
		{ "And", AND },
		{ "Ands", ANDS },
		{ "Mod", MOD },
		{ "Not", NOT },
		{ "Or", OR },
		{ "Shl", SHL },
		{ "Shr", SHR },
		{ "Xor", XOR },
		{ 0, 0 }
	};

	luaL_register(L, "bit", BitFuncs);
	lua_pop(L, 1);
}