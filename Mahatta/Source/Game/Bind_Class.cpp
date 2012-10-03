#include "game_lua_i.h"

/// @brief Calls a method if it exists
/// @note I: Instance handle
/// @note method: Method name
/// @note ...: Arguments to routine
/// @note Return: Results of call
static int Invoke_ (lua_State * L)
{
	if (lua_isnoneornil(L, 1)) luaL_error(L, "I:Invoke_: nil instance");
	if (lua_isnoneornil(L, 2)) luaL_error(L, "I:Invoke_: nil method");

	int count = lua_gettop(L) - 2;

	lua_pushvalue(L, 1);// I, method, ..., I
	lua_pushvalue(L, 2);// I, method, ..., I, method
	lua_gettable(L, -2);// I, method, ..., I, func

	if (lua_isnil(L, -1)) return 0;

	lua_replace(L, -4 - count);	// func, method, ..., I
	lua_replace(L, -2 - count);	// func, I, ...;

	if (lua_pcall(L, count + 1, LUA_MULTRET, 0) != 0) luaL_error(L, "I:Invoke_: member not callable");	// results

	return lua_gettop(L);
}

/// @brief Calls a method if a condition is satisfied
/// @note I: Instance handle
/// @note condition: Condition to test
/// @note method: Method name
/// @note ...: Arguments to routine
/// @note Return: Results of call
static int InvokeIf (lua_State * L)
{
	if (lua_isnoneornil(L, 1)) luaL_error(L, "I:InvokeIf: nil instance");
	if (lua_isnoneornil(L, 3)) luaL_error(L, "I:InvokeIf: nil method");

	if (!lua_isnoneornil(L, 2) && (!lua_isboolean(L, 2) || lua_toboolean(L, 2)))
	{
		int count = lua_gettop(L) - 3;

		lua_remove(L, 2);	// I, method, ...
		lua_pushvalue(L, 1);// I, method, ..., I
		lua_pushvalue(L, 2);// I, method, ..., I, method
		lua_gettable(L, -2);// I, method, ..., I, func

		if (lua_isnil(L, -1)) luaL_error(L, "I:InvokeIf: nil member");

		lua_replace(L, -4 - count);	// func, method, ..., I
		lua_replace(L, -2 - count);	// func, I, ...

		if (lua_pcall(L, count + 1, LUA_MULTRET, 0) != 0) luaL_error(L, "I:InvokeIf: member not callable");	// results

		return lua_gettop(L);
	}

	return 0;
}

/// @brief Data indices
enum {
		eDI_S,	///< Start entry, unused
	eDefs,	///< Index of class definition table
	eMM,///< Index of metamethod list
	eNewI,	///< Index of new instance stack
	eUDTables,	///< Userdata tables
	eUsage,	///< Index of usage table
		eDI_E,	///< End entry, unused
	eDI_N = eDI_E - 1	///< Number of class data indices
};

/// @brief Class definition indices
enum {
		eCDI_S,	///< Start entry, unused
	eBase,	///< Index of base type
	eMeta,	///< Index of metamethods table
	eMethods,	///< Index of methods table
	eNew,	///< Index of new function
	eSize,	///< Index of class size
		eCDI_E,	///< End entry, unused
	eCDI_N = eCDI_E - 1	///< Number of class data table indices
};

/// @brief Codes used in metatable setup
enum {
	eInvalid,	///< No metamethod
	eNormal,///< Typical metamethod
	eIndex,	///< __index metamethod
	eNewIndex	///< __newindex metamethod
};

/// @brief Common __index body
/// @param iM Upvalue index 1 (methods table)
/// @param iD Upvalue index 2 (data table)
/// @note object: Object being accessed
/// @note key: Lookup key
static void IndexBody (lua_State * L, int iM, int iD)
{
	// If the object is a userdatum, index its table.
	if (lua_isuserdata(L, 1))
	{
		lua_rawgeti(L, lua_upvalueindex(iD), eUDTables);// object, key, _UDT
		lua_pushvalue(L, 1);// object, key, _UDT, object
		lua_gettable(L, 3);	// object, key, _UDT, table
		lua_pushvalue(L, 2);// object, key, _UDT, table, key
		lua_gettable(L, 4);	// object, key, _UDT, table, result

		if (!lua_isnil(L, 5)) return;
	}

	// Finally, index the methods table.
	lua_settop(L, 2);	// object, key
	lua_gettable(L, lua_upvalueindex(iM));	// object, result
}

/// @brief Encapsulating __index closure
/// @note object: Object being accessed
/// @note key: Lookup key
/// @note upvalue 1: Table or function to try first
/// @note upvalue 2: Methods table
/// @note upvalue 3: Data table
static int Index (lua_State * L)
{
	// Do a call/index of the supplied function/table.
	if (lua_isfunction(L, lua_upvalueindex(1)))
	{
		lua_pushvalue(L, lua_upvalueindex(1));	// object, key, func
		lua_pushvalue(L, 1);// object, key, func, object
		lua_pushvalue(L, 2);// object, key, func, object, key
		lua_call(L, 2, 1);	// object, key, result
	}

	else
	{
		lua_pushvalue(L, 2);// object, key, key
		lua_gettable(L, lua_upvalueindex(1));	// object, key, result
	}

	// If the result of the previous step was non-nil, index on the object.
	if (lua_isnil(L, 3))
	{
		lua_pop(L, 1);	// object, key

		IndexBody(L, 2, 3);	// object, result
	}

	return 1;
}

/// @brief Default __index closure
/// @note object: Object being accessed
/// @note key: Lookup key
/// @note upvalue 1: Methods table
/// @note upvalue 2: Data table
static int IndexDefault (lua_State * L)
{
	IndexBody(L, 1, 2);	// object, result

	return 1;
}

/// @brief Common __newindex body
/// @param iD Upvalue index 1 (data table)
/// @note object: Object being accessed
/// @note key: Lookup key
/// @note value: Value to assign
static void NewIndexBody (lua_State * L, int iD)
{
	// If the object is a userdatum, replace it with its table.
	if (lua_isuserdata(L, 1))
	{
		lua_rawgeti(L, lua_upvalueindex(iD), eUDTables);// object, key, value, _UDT
		lua_pushvalue(L, 1);// object, key, value, _UDT, object
		lua_gettable(L, 4);	// object, key, value, _UDT, table
		lua_replace(L, 1);	// table, key, value, _UDT
		lua_pop(L, 1);	// table, key, value
	}

	// Assign to the object table.
	lua_rawset(L, 1);	// table
}

/// @brief Encapsulating __newindex closure
/// @note object: Object being accessed
/// @note key: Assign key
/// @note value: Value to assign
/// @note upvalue 1: Table or function to try first
/// @note upvalue 2: Methods table
/// @note upvalue 3: Data table
static int NewIndex (lua_State * L)
{
	lua_pushvalue(L, lua_upvalueindex(1));	// object, key, value, func/table

	// If a function is supplied, call it.
	if (lua_isfunction(L, 4))
	{
		lua_pushvalue(L, 1);// object, key, value, func, object
		lua_pushvalue(L, 2);// object, key, value, func, object, key
		lua_pushvalue(L, 3);// object, key, value, func, object, key, value
		lua_call(L, 3, 1);	// object, key, value, result

		// A non-nil result means no official assignment was made. Assign to the object.
		if (lua_isnil(L, 4))
		{
			lua_pop(L, 1);	// object, key, value

			NewIndexBody(L, 3);	// table
		}
	}

	// Otherwise, assign to the supplied table. 
	else
	{
		lua_replace(L, 1);	// table, key, value
		lua_settable(L, 1);	// table
	}

	return 0;
}

/// @brief Default __newindex closure
/// @note object: Object being accessed
/// @note key: Assign key
/// @note value: Value to assign
/// @note upvalue 1: Methods table
/// @note upvalue 2: Data table
static int NewIndexDefault (lua_State * L)
{
	NewIndexBody(L, 2);	// table

	return 0;
}

/// @brief Adds functionality to an existing class type
/// @note type: Type name
/// @note methods: Table of methods to add
/// @note upvalue 1: Data table
static int Extend (lua_State * L)
{
	if (lua_isnoneornil(L, 1)) luaL_error(L, "class.extend: nil type");
	if (!lua_istable(L, 2)) luaL_error(L, "class.extend: non-table methods");

	lua_settop(L, 2);	// type, methods

	// Get the type's metamethod and method tables ready.
	lua_rawgeti(L, lua_upvalueindex(1), eDefs);	// type, methods, _D
	lua_pushvalue(L, 1);// type, methods, _D, type
	lua_gettable(L, 3);	// type, methods, _D, _D[type]
	lua_rawgeti(L, 4, eMethods);// type, methods, _D, _D[type], cmethods
	lua_rawgeti(L, 4, eMeta);	// type, methods, _D, _D[type], cmethods, cmeta
	lua_replace(L, 1);	// cmeta, methods, _D, _D[type], cmethods
	lua_insert(L, 2);	// cmeta, cmethods, methods, _D, _D[type]
	lua_pop(L, 2);	// cmeta, cmethods, methods
	lua_rawgeti(L, lua_upvalueindex(1), eMM);	// cmeta, cmethods, methods, MM
	lua_pushnil(L);	// cmeta, cmethods, methods, MM, nil

	// Iterate through the methods provided, choosing the table in which to place the method.
	// If a key yields a valid code, pick metamethods; otherwise choose methods.
	while (lua_next(L, 3) != 0)	// cmeta, cmethods, methods, MM[, name, method]
	{
		lua_pushvalue(L, 5);// cmeta, cmethods, methods, MM, name, method, name
		lua_gettable(L, 4);	// cmeta, cmethods, methods, MM, name, method, code

		lua_Integer code = lua_tointeger(L, 7);

		lua_pop(L, 1);	// cmeta, cmethods, methods, MM, name, method

		// Encapsulate __index or __newindex methods.
		if (eIndex == code || eNewIndex == code)
		{
			if (!lua_isfunction(L, 6) && !lua_istable(L, 6)) luaL_error(L, "class.extend: Invalid __%sindex method", eIndex == code ? "" : "new");

			lua_pushvalue(L, 2);// cmeta, cmethods, methods, MM, name, method, cmethods
			lua_pushvalue(L, lua_upvalueindex(1));	// cmeta, cmethods, methods, data
			lua_pushcclosure(L, eIndex == code ? Index : NewIndex, 3);	// cmeta, cmethods, methods, MM, name, closure
		}

		// Copy the method into the chosen table.
		lua_pushvalue(L, 5);// cmeta, cmethods, methods, MM, name, method, name
		lua_insert(L, 6);	// cmeta, cmethods, methods, MM, name, name, method
		lua_rawset(L, code != eInvalid ? 1 : 2);// table = { ..., name = method } -> cmeta, cmethods, methods, MM, name
	}

	return 0;
}

/// @brief Defines a new type of class
/// @brief type: Type name
/// @brief methods: Method table
/// @brief new: New function
/// @brief params: Extra parameters
/// @note upvalue 1: Data table
static int Define (lua_State * L)
{
	if (lua_isnoneornil(L, 1)) luaL_error(L, "class.define: nil type");
	if (!lua_istable(L, 2)) luaL_error(L, "class.define: non-table methods");
	if (lua_isnoneornil(L, 3)) luaL_error(L, "class.define: nil new");

	// Allocate an entry for the class type and its methods. Check for extra parameters.
	bool bParams = lua_gettop(L) > 3;

	lua_settop(L, bParams ? 4 : 3);	// type, methods, new[, params]

	lua_createtable(L, eCDI_N, 0);	// type, methods, new[, params], C = {}
	lua_newtable(L);// type, methods, new[, params], C, cmethods = {}
	lua_rawgeti(L, lua_upvalueindex(1), eDefs);	// type, methods, new[, params], C, cmethods, _D
	lua_pushvalue(L, 1);// type, methods, new[, params], C, cmethods, _D, type
	lua_gettable(L, -2);// type, methods, new[, params], C, cmethods, _D, _D[type]

	if (!lua_isnil(L, -1)) luaL_error(L, "class.define: class already defined");

	lua_pop(L, 1);	// type, methods, new[, params], C, cmethods, _D

	if (bParams)
	{
		if (!lua_istable(L, 4)) luaL_error(L, "class.define: non-table parameters");

		// Inherit from base class if provided.
		lua_getfield(L, 4, "base");	// type, methods, new, params, C, cmethods, _D, base

		if (!lua_isnil(L, 8))
		{
			// Verify that the base class exists.
			lua_gettable(L, 7);	// type, methods, new, params, C, cmethods, _D, _D[base]

			if (lua_isnil(L, 8)) luaL_error(L, "class.define: base class does not exist");

			// By default, the new class will inherit the base class's size.
			lua_rawgeti(L, 8, eSize);	// type, methods, new, params, C, cmethods, _D, _D[base], size
			lua_rawseti(L, 5, eSize);	// type, methods, new, params, C, cmethods, _D, _D[base]

			// Bind the method table's "__index" metamethod to the base class's method table to
			// inherit its methods.
			lua_newtable(L);// type, methods, new, params, C, cmethods, _D, _D[base], {}
			lua_rawgeti(L, 8, eMethods);// type, methods, new, params, C, cmethods, _D, _D[base], {}, bmethods

			lua_setfield(L, 9, "__index");	// type, methods, new, params, C, cmethods, _D, _D[base], { __index = bmethods }
			lua_replace(L, 8);	// type, methods, new, params, C, cmethods, _D, { __index }
			lua_setmetatable(L, 6);	// type, methods, new, params, C, cmethods, _D

			// Store the base class name.
			lua_getfield(L, 4, "base");	// type, methods, new, params, C, cmethods, _D, base
			lua_rawseti(L, 5, eBase);	// type, methods, new, params, C, cmethods, _D
		}

		else lua_pop(L, 1);	// type, methods, new, params, C, cmethods, _D

		// Assign userdatum size if provided.
		lua_getfield(L, 4, "size");	// type, methods, new, params, C, cmethods, _D, size

		if (!lua_isnil(L, 8))
		{
			if (lua_tointeger(L, 8) <= 0) luaL_error(L, "class.define: non-positive integer size");

			lua_rawseti(L, 5, eSize);	// type, methods, new, params, C, cmethods, _D
		}

		else lua_pop(L, 1);	// type, methods, new, params, C, cmethods, _D

		// Treat remainder of function as though no parameters were provided.
		lua_remove(L, 4);	// type, methods, new, C, cmethods, _D
	}

	// Register the class.
	lua_pushvalue(L, 1);// type, methods, new, C, cmethods, _D, type
	lua_pushvalue(L, 4);// type, methods, new, C, cmethods, _D, type, C
	lua_settable(L, 6);	// _D[type] = C -> type, methods, new, C, cmethods, _D
	lua_pop(L, 1);	// type, methods, new, C, cmethods

	// Bind the metamethods table, installing stock methods.
	lua_newtable(L);// type, methods, new, C, cmethods, {}

	luaL_reg meta[] = {
		{ "__index", IndexDefault },
		{ "__newindex", NewIndexDefault }
	};

	for (int index = 0; index < sizeof(meta) / sizeof(luaL_reg); ++index)
	{
		lua_pushvalue(L, 5);// type, methods, new, C, cmethods, { ... }, cmethods
		lua_pushvalue(L, lua_upvalueindex(1));	// type, methods, new, C, cmethods, { ... }, cmethods, data
		lua_pushcclosure(L, meta[index].func, 2);	// type, methods, new, C, cmethods, { ... }, func
		lua_setfield(L, 6, meta[index].name);	// type, methods, new, C, cmethods, { ..., name = func }
	}

	lua_pushboolean(L, true);	// type, methods, new, C, cmethods, { ... }, true
	lua_setfield(L, 6, "__metatable");	// type, methods, new, C, cmethods, { ..., __metatable = true }
	lua_rawseti(L, 4, eMeta);	// type, methods, new, { meta = { ..., __metatable } }, cmethods

	// Bind the methods table, installing stock methods.
	luaL_reg Methods[] = {
		{ "Invoke_", Invoke_ },
		{ "InvokeIf", InvokeIf },
		{ 0, 0 }
	};

	luaL_register(L, 0, Methods);	// type, methods, new, { meta }, cmethods
	lua_rawseti(L, 4, eMethods);// type, methods, new, { meta, methods = cmethods }

	// Bind the new function.
	lua_pushvalue(L, 3);// type, methods, new, { meta, methods }, new
	lua_rawseti(L, 4, eNew);// type, methods, new, { meta, methods, new = new }

	// Install user-supplied methods and metamethods. Ensure that some metamethods are set.
	return Extend(L);	// cmeta, cmethods, methods, MM, n
}

/// @brief Indicates whether an item is a class instance
/// @note I: Item
/// @note upvalue 1: Data table
/// @note Return: If true, item is an instance
static int IsInstance (lua_State * L)
{
	lua_settop(L, 1);	// Item

	// If an item was supplied, look it up.
	if (!lua_isnoneornil(L, 1))
	{
		lua_rawgeti(L, lua_upvalueindex(1), eUsage);// I, _U
		lua_insert(L, 1);	// _U, I
		lua_gettable(L, 1);	// _U, type
	}

	// Indicate whether the item exists in the instance usage data.
	lua_pushboolean(L, !lua_isnoneornil(L, 2));	// U, type, bExists

	return 1;
}

/// @brief Indicates whether an instance is of a given type
/// @note I: Instance handle
/// @note what: Type to test
/// @note upvalue 1: Data table
/// @note Return: If true, instance matches type
static int IsType (lua_State * L)
{
	if (lua_isnoneornil(L, 1)) luaL_error(L, "class.istype: nil instance");
	if (lua_isnoneornil(L, 2)) luaL_error(L, "class.istype: nil type");

	lua_settop(L, 2);	// I, what

	// Start with the instance's own type and progress up its base types. If any match the
	// given type, report success.
	lua_rawgeti(L, lua_upvalueindex(1), eDefs);	// I, what, _D
	lua_rawgeti(L, lua_upvalueindex(1), eUsage);// I, what, _D, _U
	lua_pushvalue(L, 1);// I, what, _D, _U, I
	lua_gettable(L, 4);	// I, what, _D, _U, type
	lua_replace(L, 4);	// I, what, _D, type

	while (!lua_isnil(L, 4) && !lua_equal(L, 2, 4))
	{
		lua_gettable(L, 3);	// I, what, _D, _D[type]
		lua_rawgeti(L, 4, eBase);	// I, what, _D, _D[type], base
		lua_replace(L, 4);	// I, what, _D, base
	}

	lua_pushboolean(L, lua_equal(L, 2, 4));	// I, what, _D, type, bIsType

	return 1;
}

/// @brief Instantiates a class
/// @note type: Type name
/// @note ...: Constructor arguments
/// @note upvalue 1: Data table
/// @note Return: Instance handle
static int New (lua_State * L)
{
	if (lua_isnoneornil(L, 1)) luaL_error(L, "class.new: nil type");

	// Instantiate the class, initially as a table.
	lua_newtable(L);// type, ..., I = {}

	// Get size information. If non-zero, allocate a userdatum and replace the instance; the
	// instance table will then be bound to the userdatum.
	lua_rawgeti(L, lua_upvalueindex(1), eDefs);	// type, ..., I, _D
	lua_pushvalue(L, 1);// type, ..., I, _D, type
	lua_gettable(L, -2);// type, ..., I, _D, _D[type]

	if (lua_isnil(L, -1)) luaL_error(L, "class.new: type does not exist");

	lua_rawgeti(L, -1, eSize);	// type, ..., I, _D, _D[type], size

	lua_Integer size = lua_tointeger(L, -1);

	lua_pop(L, 1);	// type, ..., I, _D, _D[type]

	if (size > 0)
	{
		lua_newuserdata(L, size);	// type, ..., I, _D, _D[type], ud
		lua_rawgeti(L, lua_upvalueindex(1), eUDTables);	// type, ..., I, _D, _D[type], ud, _UD
		lua_pushvalue(L, -2);	// type, ..., I, _D, _D[type], ud, _UD, ud
		lua_pushvalue(L, -6);	// type, ..., I, _D, _D[type], ud, _UD, ud, I
		lua_settable(L, -3);// _UD[ud] = I -> type, ..., I, _D, _D[type], ud, _UD
		lua_pop(L, 1);	// type, ..., I, _D, _D[type], ud
		lua_replace(L, -4);	// type, ..., I = ud, _D, _D[type]
	}

	// Bind the metatable to supply metamethods and methods.
	lua_rawgeti(L, -1, eMeta);	// type, ..., I, _D, _D[type], meta
	lua_setmetatable(L, -4);// type, ..., I, _D, _D[type]

	// Indicate that the instance is in use.
	lua_rawgeti(L, lua_upvalueindex(1), eUsage);// type, ..., I, _D, _D[type], _U
	lua_pushvalue(L, -4);	// type, ..., I, _D, _D[type], _U, I
	lua_pushvalue(L, 1);// type, ..., I, _D, _D[type], _U, I, type
	lua_settable(L, -3);// _U[I] = type -> type, ..., I, _D, _D[type], _U
	lua_pop(L, 1);	// type, ..., I, _D, _D[type]

	// Prepare the new function.
	lua_rawgeti(L, -1, eNew);	// type, ..., I, _D, _D[type], new
	lua_replace(L, 1);	// new, ..., I, _D, _D[type]

	// Allow base constructor invocations.
	lua_rawgeti(L, lua_upvalueindex(1), eNewI);	// new, ..., I, _D, _D[type], _N
	lua_pushvalue(L, -4);	// new, ..., I, _D, _D[type], _N, I
	lua_rawseti(L, -2, luaL_getn(L, -2) + 1);	// _N[new top] = I -> new, ..., I, _D, _D[type], _N

	// Invoke the new function.
	lua_pop(L, 3);	// new, ..., I
	lua_pushvalue(L, -1);	// new, ..., I, I
	lua_insert(L, 1);	// I, new, ..., I
	lua_insert(L, 3);	// I, new, I, ...
	
	if (lua_pcall(L, lua_gettop(L) - 2, 0, 0) != 0) luaL_error(L, "class.new: new function not callable");	// I

	// Disallow base constructor invocations.
	lua_rawgeti(L, lua_upvalueindex(1), eNewI);	// I, _N
	lua_pushnil(L);	// I, _N, nil
	lua_rawseti(L, 2, luaL_getn(L, 2));	// _N[old top] = nil -> I, _N
	lua_pop(L, 1);	// I

	return 1;
}

/// @brief Invokes a base type constructor
/// @note base: Base type name
/// @note ...: Constructor arguments
/// @note upvalue 1: Data table
static int SCons (lua_State * L)
{
	if (lua_isnoneornil(L, 1)) luaL_error(L, "class.scons: nil base type");

	// Get the new instance. If nil, this is invoked outside of a new function.
	lua_rawgeti(L, lua_upvalueindex(1), eNewI);	// base, ..., _N

	if (lua_objlen(L, -1) == 0) luaL_error(L, "class.scons: invoked outside of new function");

	lua_rawgeti(L, -1, luaL_getn(L, -1));	// base, ..., _N, I
	lua_replace(L, -2);	// base, ..., I

	// Ensure that the base type is not the same as the class type.
	if (lua_equal(L, 1, -1)) luaL_error(L, "class.scons: base type equal to class type");

	// Ensure that the base type exists.
	lua_rawgeti(L, lua_upvalueindex(1), eDefs);	// base, ..., I, _D
	lua_pushvalue(L, 1);// base, ..., I, _D, base
	lua_gettable(L, -2);// base, ..., I, _D, _D[base]
	lua_replace(L, 1);	// _D[base], ..., I, _D

	if (lua_isnil(L, 1)) luaL_error(L, "class.scons: base type does not exist");

	// Ensure that the instance is derived from the base type.
	lua_rawgeti(L, lua_upvalueindex(1), eUsage);// _D[base], ..., I, _D, _U
	lua_pushvalue(L, -3);	// _D[base], ..., I, _D, _U, I
	lua_gettable(L, -2);// _D[base], ..., I, _D, _U, type
	lua_gettable(L, -3);// _D[base], ..., I, _D, _U, _D[type]
	lua_replace(L, -2);	// _D[base], ..., I, _D, _D[type]

	while (!lua_rawequal(L, 1, -1))
	{
		lua_rawgeti(L, -1, eBase);	// _D[base], ..., I, _D, _D[type], abase

		if (lua_isnil(L, -1)) luaL_error(L, "class.scons: not a base type of new instance");

		lua_gettable(L, -3);// _D[base], ..., I, _D, _D[type], _D[abase]
		lua_replace(L, -2);	// _D[base], ..., I, _D, _D[abase]
	}

	lua_pop(L, 2);	// _D[base], ..., I

	// Invoke the constructor.
	lua_pushvalue(L, 1);// _D[base], ..., I, _D[base]
	lua_rawgeti(L, -1, eNew);	// _D[base], ..., I, _D[base], new
	lua_replace(L, 1);	// new, ..., I, _D[base]
	lua_pop(L, 1);	// new, ..., I
	lua_insert(L, 2);	/// new, I, ...
	lua_call(L, lua_gettop(L) - 1, 0);

	return 0;
}

/// @brief Gets the base type of a given type
/// @note type: Type name
/// @note upvalue 1: Data table
/// @note Return: Base type name
static int Super (lua_State * L)
{
	if (lua_isnoneornil(L, 1)) luaL_error(L, "class.super: nil type");

	lua_settop(L, 1);	// type

	// Fail if no class is associated with the type.
	lua_rawgeti(L, lua_upvalueindex(1), eDefs);	// type, _D
	lua_insert(L, 1);	// _D, type
	lua_gettable(L, 1);	// _D, _D[type]

	if (lua_isnil(L, 2)) return 0;

	// Return the base type name.
	lua_rawgeti(L, 2, eBase);	// _D, _D[type], base

	return 1;
}

/// @brief Gets the instance type
/// @note I: Instance handle
/// @note upvalue 1: Data table
/// @note Return: Type name
static int Type (lua_State * L)
{
	if (lua_isnoneornil(L, 1)) luaL_error(L, "class.type: nil instance");

	// If the instance exists, return its type.
	lua_rawgeti(L, lua_upvalueindex(1), eUsage);// I, _U
	lua_insert(L, 1);	// _U, I
	lua_gettable(L, 1);	// _U, type

	return 1;
}

/// @brief Binds the class library to the Lua scripting system
void LuaI::open_class (lua_State * L)
{
	// Class data table.
	lua_createtable(L, eDI_N, 0);	// {}

	// Create userdata table storage and the new instance stack.
	int indices[] = { eNewI, eUDTables };

	for (int index = 0; index < sizeof(indices) / sizeof(int); ++index)
	{
		lua_newtable(L);// { ... }, {}
		lua_rawseti(L, -2, indices[index]);	// { ..., [index] = {} }
	}

	// Bind codes to each valid metamethod key.
	char const * metamethods[] = {
		"__index", "__newindex",
		"__eq", "__le", "__lt",
		"__add", "__div", "__mul", "__sub",
		"__mod", "__pow", "__unm",
		"__call", "__concat", "__gc", "__len"
	};

	lua_createtable(L, 0, sizeof(metamethods) / sizeof(char const *));	// { newI, udTables }, {}

	for (int index = 0; index < sizeof(metamethods) / sizeof(char const *); ++index)
	{
		lua_pushinteger(L, index > 1 ? eNormal : eIndex + index);	// { newI, udTables }, { ... }, code
		lua_setfield(L, -2, metamethods[index]);// { newI, udTables }, { ..., metamethod = code }
	}

	lua_rawseti(L, -2, eMM);// { defs, MM, newI, udTables }

	// Create weak tables for class definition and instance usage storage, where the latter
	// doubles as the metatable since its instance keys will never be strings.
	lua_newtable(L);// { MM, newI, userdata }, usage
	lua_pushliteral(L, "k");// { MM, newI, udTables }, usage, "k"
	lua_setfield(L, -2, "__mode");	// { MM, newI, udTables }, usage = { __mode = "k" }
	lua_newtable(L);// { MM, newI, udTables }, usage, defs

	int windices[] = { eDefs, eUsage };

	for (int index = 0; index < sizeof(windices) / sizeof(int); ++index)
	{
		lua_pushvalue(L, -2 + index);	// { MM, newI, udTables[, defs] }, usage[, defs], usage
		lua_setmetatable(L, -2);// { MM, newI, udTables[, defs] }, usage[, defs]
		lua_rawseti(L, -3 + index, windices[index]);// { ..., [index] = {} }[, usage]
	}

	// Install the class function table.
	const luaL_reg Funcs[] = {
		{ "define", Define },
		{ "extend", Extend },
		{ "isinstance", IsInstance },
		{ "istype", IsType },
		{ "new", New },
		{ "scons", SCons },
		{ "super", Super },
		{ "type", Type },
		{ 0, 0 }
	};

	luaI_openlib(L, "class", Funcs, 1);	// _G.class
	lua_pop(L, 1);	// stack clear
}