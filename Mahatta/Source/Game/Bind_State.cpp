/// TEMP
#include "game_match.h"
/// TEMP

#include "game_lua.h"
#include "game_lua_i.h"
#include "game_local.h"

using namespace Lua;

/// @brief Binds routines to an environment global
/// @note variable: Variable to bind
/// @note get: Get routine
/// @note set: Set routine
/// @note upvalue 1: get table
/// @note upvalue 2: set table
static int BindGlobal (lua_State * L)
{
	lua_pushvalue(L, 1);// variable, get, set, variable
	lua_insert(L, 3);	// variable, get, variable, set
	lua_settable(L, lua_upvalueindex(2));	// variable, get
	lua_settable(L, lua_upvalueindex(1));

	return 0;
}

/// @brief Environment __index closure
/// @note env: Environment being accessed
/// @note key: Lookup key
/// @note upvalue 1: get table
static int Index (lua_State * L)
{
	lua_gettable(L, lua_upvalueindex(1));	// env, get[key]

	if (!lua_isnil(L, 2)) lua_call(L, 0, 1);// env, result

	return 1;
}

/// @brief Environment __newindex closure
/// @note env: Environment being accessed
/// @note key: Lookup key
/// @note value: Value to assign
/// @note upvalue 1: set table
static int NewIndex (lua_State * L)
{
	lua_pushvalue(L, 2);// env, key, value, key
	lua_gettable(L, lua_upvalueindex(1));	// env, key, value, set[key]

	if (!lua_isnil(L, 4))
	{
		lua_insert(L, 3);	// env, key, set[key], value
		lua_call(L, 1, 0);	// env, key
	}

	else
	{
		lua_pop(L, 1);	// env, key, value
		lua_rawset(L, 1);	// env
	}

	return 0;
}
			
/// @brief Configures an environment	
/// @note env: Environment to configure
static int EnvConfigure (lua_State * L)
{
	lua_createtable(L, 0, 2);	// env, meta
	lua_newtable(L);// env, meta, get
	lua_newtable(L);// env, meta, get, set
	lua_pushvalue(L, 3);// env, meta, get, set, get
	lua_pushvalue(L, 4);// env, meta, get, set, get, set
	lua_pushcclosure(L, BindGlobal, 2);	// env, meta, get, set, BindGlobal
	lua_setfield(L, 1, "BindGlobal");	// env = { ..., BindGlobal }, meta, get, set
	lua_pushcclosure(L, NewIndex, 1);	// env, meta, get, NewIndex
	lua_setfield(L, 2, "__newindex");	// env, meta = { __newindex = NewIndex }, get
	lua_pushcclosure(L, Index, 1);	// env, meta, Index
	lua_setfield(L, 2, "__index");	// env, meta = { __newindex, __index = Index }
	lua_setmetatable(L, 1);	// env

	return 0;
}

/// @brief Prints a formatted message to a file
/// @note file: File handle or name of file to set
/// @note text: Format string
/// @note ...: Arguments to string
static int fprintf (lua_State * L)
{
/*
	"local stream = type(file) ~= 'string' and file or io.open(file, 'a');"
	"stream:write(text:format(...), '\n');"
	"if file ~= stream then"
		"stream:close();"
	"end"
*/
	return 0;
}

/// @brief Installs a set of routines
/// @note routines: Routine set
/// @note Returns success boolean
static int LoadRoutines (lua_State * L)
{
	size_t index, count = lua_objlen(L, 1);

	std::string prefix("Main/Scripts/");

	for (index = 0; index < count; ++index)
	{
		lua_rawgeti(L, 1, int(index) + 1);	// routines, ..., routines[index]

		// If an entry is a string, load the script it references.
		if (lua_type(L, -1) == LUA_TSTRING)
		{
			// Remove any old results.
			lua_insert(L, 2);	// routines, routines[index], ...
			lua_settop(L, 2);	// routines, routines[index]

			luaL_loadfile(L, (prefix + S(L, 2)).c_str());	// routines, routines[index], file[, error]

			if (lua_isnil(L, 3) || lua_pcall(L, 0, LUA_MULTRET, 0) != 0)
			{
				Call(L, "printf", 0, "s a", "Loading error: %s", -1);

				break;
			}

			// Leave any results on the stack to be passed to functions.
			lua_remove(L, 2);	// routines, ...
		}

		// If an entry is a function, evaluate it with any arguments left on the stack.
		// Non-nil results indicate failure.
		else if (lua_type(L, -1) == LUA_TFUNCTION)
		{
			lua_insert(L, 2);	// routines, func, ...
			lua_call(L, lua_gettop(L) - 2, 1);	// routines, result

			if (!lua_isnil(L, 2)) break;
		}

		else luaL_error(L, "LoadRoutines: bad type");
	}

	lua_pushboolean(L, index == count);	// routines, ..., bSuccess

	return 1;
}

/// @brief Binds state utilities to the Lua scripting system
void LuaI::open_state (lua_State * L)
{
	#define R_(n) lua_register(L, #n, n)

	R_(EnvConfigure);
	R_(fprintf);
	R_(LoadRoutines);

	#undef R_
}