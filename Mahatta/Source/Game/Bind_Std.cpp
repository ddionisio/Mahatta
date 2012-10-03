#include "game_lua.h"
#include "game_lua_i.h"

/// @brief Binds utilities to the Lua scripting system
void LuaI::open_std (lua_State * L)
{
	char const * files[] = { "Call.txt", "Generic.txt", "Method.txt" };

	lua_createtable(L, sizeof(files) / sizeof(char const *), 0);// {}

	for (int index = 0; index < sizeof(files) / sizeof(char const *); ++index)
	{
		lua_pushstring(L, files[index]);// { ... }, file
		lua_rawseti(L, -2, index + 1);	// { ..., file }
	}

	Lua::Call(L, "LoadRoutines", 0, "a", -1);

	lua_pop(L, 1);
}