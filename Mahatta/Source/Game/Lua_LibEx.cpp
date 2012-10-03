#include "game_lua.h"
#include "game_lua_i.h"

namespace Lua
{
	/// @brief Builds a Lua state and configures its library
	/// @return Lua state
	lua_State * NewState (void)
	{
		lua_State * state = luaL_newstate();

		// Supply standard libraries.
		luaL_openlibs(state);

		// Provide OO, environment control, and standard game features.
		LuaI::open_class(state);
		LuaI::open_state(state);
		LuaI::open_std(state);

		// Provide a global environment binding.
		Call(state, "EnvConfigure", 0, "a", LUA_GLOBALSINDEX);

		// Add game libraries/tools.
		LuaI::open_bit(state);
		LuaI::open_graphics(state);
		LuaI::open_input(state);
		LuaI::open_mathops(state);
		LuaI::open_sequence(state);
		LuaI::open_voxel(state);

		return state;
	}

	/// @brief Defines a class
	/// @param L Lua state
	/// @param name Type name
	/// @param methods Methods to associate with class
	/// @param newf Function used to instantiate class
	/// @param base [optional] Base type name
	/// @param size [optional] Userdatum size
	void class_Define (lua_State * L, char const * name, luaL_Reg const * methods, lua_CFunction newf, char const * base, Uint size)
	{
		class_Define(L, name, methods, 0, newf, base, size);
	}

	/// @brief Defines a class, with new function on the stack top
	/// @param L Lua state
	/// @param name Type name
	/// @param methods Methods to associate with class
	/// @param base [optional] Base type name
	/// @param size [optional] Userdatum size
	void class_Define (lua_State * L, char const * name, luaL_Reg const * methods, char const * base, Uint size)
	{
		char const * dummy[] = { 0 };

		class_Define(L, name, methods, dummy, base, size);
	}

	/// @brief Defines a class, with closures on the stack
	/// @param L Lua state
	/// @param name Type name
	/// @param methods Methods to associate with class
	/// @param closures Closure names
	/// @param newf Function used to instantiate class
	/// @param base [optional] Base type name
	/// @param size [optional] Userdatum size
	void class_Define (lua_State * L, char const * name, luaL_Reg const * methods, char const * closures[], lua_CFunction newf, char const * base, Uint size)
	{
		lua_pushcfunction(L, newf);	// ..., newf

		class_Define(L, name, methods, closures, base, size);
	}

	/// @brief Defines a class, with closures on the stack and new function at the top
	/// @param L Lua state
	/// @param name Type name
	/// @param methods Methods to associate with class
	/// @param closures Closure names
	/// @param base [optional] Base type name
	/// @param size [optional] Userdatum size
	void class_Define (lua_State * L, char const * name, luaL_Reg const * methods, char const * closures[], char const * base, Uint size)
	{
		assert(methods != 0 || closures != 0);

		// Count the closures.
		int count = 0;

		while (closures != 0 && closures[count] != 0) ++count;

		// Install the new function.
		lua_insert(L, -count - 1);	// new, ...

		// Load methods.
		lua_newtable(L);// new, ..., M
		
		if (methods != 0) luaL_register(L, 0, methods);

		// Load closures.
		for (int index = 0; index < count; ++index)
		{
			lua_pushstring(L, closures[index]);	// new, ..., M, name
			lua_pushvalue(L, -count - 2 + index);	// new, ..., M, name, closure
			lua_settable(L, -3);// new, ..., M = { ..., name = closure }
		}

		lua_insert(L, -count - 2);	// M, new, ...
		lua_pop(L, count);	// M, new

		// Assign any parameters.
		Call(L, "class.define", 0, "s a a C{ CKss CKsi }", name, -2, -1, base != 0 || size != 0, base != 0, "base", base, size != 0, "size", size);

		lua_pop(L, 2);
	}

	/// @brief Instantiates a class
	/// @param L Lua state
	/// @param name Type name
	/// @param count Count of parameters on stack
	void class_New (lua_State * L, char const * name, int count)
	{
		GetGlobal(L, "class.new");	// ..., class.new

		lua_pushstring(L, name);// ..., class.new, name
		lua_insert(L, -2 - count);	// name, ..., class.new 
		lua_insert(L, -2 - count);	// class.new, name, ...
		lua_call(L, count + 1, 1);	// I
	}

	/// @brief Instantiates a class
	/// @param L Lua state
	/// @param name Type name
	/// @param params Parameter descriptors (q.v. CallCore)
	/// @note Vararg parameters are the arguments
	void class_New (lua_State * L, char const * name, char const * params, ...)
	{
		GetGlobal(L, "class.new");	// class.new

		lua_pushstring(L, name);// class.new, name

		va_list args;	va_start(args, params);

		try {
			CallCore(L, 1, 1, params, args);
		}
		
		catch (char const * error)
		{
			lua_getglobal(L, "printf");	// printf
			lua_pushliteral(L, "Lua::class_New(%s): %s");	// printf, format
			lua_pushstring(L, name);// printf, format, name
			lua_pushstring(L, error);	// printf, format, name, error
			lua_call(L, 3, 0);
		}
	}

	/// @brief Invokes a base type constructor
	/// @param L Lua state
	/// @param base Base type name
	/// @param count Count of parameters on stack
	void class_SCons (lua_State * L, char const * base, int count)
	{
		GetGlobal(L, "class.scons");// ..., class.scons

		lua_pushstring(L, base);// ..., class.scons, base
		lua_insert(L, -2 - count);	// class.scons, ..., base
		lua_insert(L, -2 - count);	// class.scons, base, ...
		lua_call(L, count + 1, 0);
	}

	/// @brief Invokes a base type constructor
	/// @param L Lua state
	/// @param base Base type name
	/// @param params Parameter descriptors (q.v. CallCore)
	/// @note Vararg parameters are the arguments
	void class_SCons (lua_State * L, char const * base, char const * params, ...)
	{
		GetGlobal(L, "class.scons");// class.scons

		lua_pushstring(L, base);// class.scons

		va_list args;	va_start(args, params);

		try {
			CallCore(L, 1, 0, params, args);
		}
		
		catch (char const * error)
		{
			lua_getglobal(L, "printf");	// printf
			lua_pushliteral(L, "Lua::class_SCons(%s): %s");	// printf, format
			lua_pushstring(L, base);// printf, format, base
			lua_pushstring(L, error);	// printf, format, base, error
			lua_call(L, 3, 0);
		}
	}

	/// @brief Indicates whether an item is an instance
	/// @param L Lua state
	/// @param index Index of argument
	/// @return If true, item is an instance
	bool class_IsInstance (lua_State * L, int index)
	{
		Call(L, "class.isinstance", 1, "a", index);
		
		bool bInstance = lua_toboolean(L, -1) != 0;

		lua_pop(L, 1);

		return bInstance;
	}

	/// @brief Indicates whether an item is of the given type
	/// @param L Lua state
	/// @param index Index of item
	/// @param type Type name
	/// @param return If true, item is of the type
	bool class_IsType (lua_State * L, int index, char const * type)
	{
		Call(L, "class.istype", 1, "a s", index, type);
		
		bool bType = lua_toboolean(L, -1) != 0;

		lua_pop(L, 1);

		return bType;
	}
}