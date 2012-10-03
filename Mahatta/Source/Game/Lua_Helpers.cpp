#include "game_lua.h"
#include "game_lua_i.h"

using namespace Lua;

/// @brief Used to read arguments
struct Reader {
	// Enumerations
	enum {
		eKey,	///< Key is being read
		eValue,	///< Value is being read
		eSkip	///< Do not add element
	};

	// Members
	std::bitset<32> mStatus;///< Reader status
	va_list & mArgs;///< Variable argument list
	lua_State * mL;	///< Lua state
	char const * mParams;	///< Parameter list
	int mHeight;///< Table height
	int mTop;	///< Original top of stack used to resolve negative indices

	Reader (va_list & args, lua_State * L, char const * params, int top) : mArgs(args), mL(L), mParams(params), mHeight(0), mTop(top) {}
	~Reader (void) { va_end(mArgs); }

	/// @brief Loads a value from the stack 
	void _a (void)
	{
		int arg = va_arg(mArgs, int);

		if (!(arg >= lua_upvalueindex(LUAI_MAXUPVALUES) && arg <= LUA_REGISTRYINDEX))
		{
			if (arg < 0) arg += 'a' == *mParams ? mTop : lua_gettop(mL) + 1;

			if (arg <= 0 || arg > lua_gettop(mL)) throw "Bad index";
		}

		if (mStatus.test(eKey) && lua_isnil(mL, arg)) throw "Null key";

		if (!mStatus.test(eSkip)) lua_pushvalue(mL, arg);	// ...[, arg]
	}

	/// @brief Loads a boolean
	void _b (void)
	{
		bool bArg = 'b' == *mParams ? va_arg(mArgs, bool) : 'T' == *mParams;

		if (!mStatus.test(eSkip)) lua_pushboolean(mL, bArg);// ...[, bArg]
	}

	/// @brief Loads a function
	void _f (void)
	{
		lua_CFunction func = va_arg(mArgs, lua_CFunction);

		if (!mStatus.test(eSkip)) lua_pushcfunction(mL, func);	// ...[, func]
	}

	/// @brief Loads an integer
	void _i (void)
	{
		int i = va_arg(mArgs, int);

		if (!mStatus.test(eSkip)) lua_pushinteger(mL, i);	// ...[, i]
	}

	/// @brief Loads a number
	void _n (void)
	{
		float n = va_arg(mArgs, float);

		if (!mStatus.test(eSkip)) lua_pushnumber(mL, n);// ...[, n]
	}

	/// @brief Loads a string
	void _s (void)
	{
		char const * str = va_arg(mArgs, char const *);

		if (!mStatus.test(eSkip)) lua_pushstring(mL, str);	// ...[, str]
	}

	/// @brief Loads a userdata
	void _u (void)
	{
		void * ud = va_arg(mArgs, void *);

		if (!mStatus.test(eSkip)) lua_pushlightuserdata(mL, ud);// ...[, ud]
	}

	/// @brief Loads a nil
	void _0 (void)
	{
		if (mStatus.test(eKey)) throw "Null key";

		if (!mStatus.test(eSkip)) lua_pushnil(mL);	// ...[, nil]
	}

	/// @brief Loads a global
	void _G (void)
	{
		char const * name = va_arg(mArgs, char const *);

		if (!mStatus.test(eSkip)) GetGlobal(mL, name);	// ...[, global]
	}

	/// @brief Loads a table
	void _table (void)
	{
		++mHeight;

		if (!mStatus.test(eSkip)) lua_newtable(mL);	// ...[, {}]

		for (++mParams; ; ++mParams)	// skip '{' at start, and skip over last parameter on each pass
		{
			int top = lua_gettop(mL);

			if (!ReadElement()) throw "Unclosed table";	// ..., { ... }[, element]

			// If the stack has grown, append the element to the table.
			if (lua_gettop(mL) > top) lua_rawseti(mL, -2, luaL_getn(mL, -2) + 1);	// ..., { ..., [new top] = element }

			// On a '}' terminate a table (skipped over by caller).
			else if ('}' == *mParams) break;
		}

		--mHeight;
	}

	/// @brief Processes a conditional
	void _C (void)
	{
		if (mStatus.test(eKey)) throw "Conditional key";
		if (mStatus.test(eValue)) throw "Conditional value";

		++mParams;	// Skip 'C' (value skipped by caller)

		bool bSkipSave = mStatus.test(eSkip), bDoSkip = !va_arg(mArgs, bool);

		if (!mStatus.test(eSkip)) mStatus.set(eSkip, bDoSkip);
	
		if (!ReadElement()) throw "Unfinished condition";	// ...[, value]

		mStatus.set(eSkip, bSkipSave);
	}

	/// @brief Processes a key
	void _K (void)
	{
		++mParams;	// Skip 'K'

		mStatus.set(eKey);

		if (!ReadElement()) throw "Missing key";	// ..., { ... }[, k]

		++mParams;	// Skip key (value skipped in table logic)

		mStatus.reset(eKey);
		mStatus.set(eValue);

		if (!ReadElement()) throw "Missing value";	// ..., { ... }[, k, v]

		mStatus.reset(eValue);

		if (!mStatus.test(eSkip)) lua_settable(mL, -3);	// ..., { ...[, k = v] }
	}

	/// @brief Reads an element from the parameter set
	/// @return If true, parameters remain
	bool ReadElement (void)
	{
		// Remove space characters.
		while (isspace(*mParams)) ++mParams;

		// Branch on argument type.
		switch (*mParams)
		{
		case '\0':	// End of list
			return false;
		case 'a':	// Add argument from the stack
		case 'r':
			_a();
			break;
		case 'b':	// Add boolean
		case 'T':
		case 'F':
			_b();
			break;
		case 'f':	// Add function
			_f();
			break;
		case 'i':	// Add integer
			_i();
			break;
		case 'n':	// Add number
			_n();
			break;
		case 's':	// Add string
			_s();
			break;
		case 'u':	// Add userdata
			_u();
			break;
		case '0':	// Add nil
			_0();
			break;
		case 'g':	// Add global
			_G();
			break;
		case '{':	// Begin table
			_table();
			break;
		case '}':	// End table (error)
			if (0 == mHeight) throw "Unopened table";
			break;
		case 'C':	// Evaluate condition
			_C();
			break;
		case 'K':	// Key
			if (0 == mHeight) throw "Key outside table";

			_K();
			break;
		default:
			throw "Bad type";
		}

		// Keep reading.
		return true;
	}
};

/// @brief
/// @param L Lua state
/// @param count Count of arguments already added to stack
/// @param retc Result count (may be MULT_RET)
/// @param params Parameter descriptors
///		   a Argument (stack index, relative to initial stack top if negative; also accepts pseudo-indices)
///		   r Relative argument (same as 'a', but relative to current stack top if negative)
///		   b Boolean
///		   T true
///		   F false
///		   f Function
///		   i Integer
///		   n Number
///		   s String
///		   u Light userdata
///		   0 Nil
///		   g Global (as per GetGlobal with no arguments)
///		   { Begin table (arguments added up to matching brace added)
///		   } End table
///		   C Condition boolean (if false, next parameter is skipped)
///		   K Next value is table key
/// @param args Variable argument list
/// @return Number of results of call
int Lua::CallCore (lua_State * L, int count, int retc, char const * params, va_list & args)
{
	int top = lua_gettop(L) - count;

	try {
		// Parse the arguments.
		int curtop = lua_gettop(L);

		for (Reader r(args, L, params, top); r.ReadElement(); ++r.mParams);

		count += lua_gettop(L) - curtop;

		// Invoke the function.
		int after = lua_gettop(L) - count - 1;

		if (lua_pcall(L, count, retc, 0) != 0) throw S(L, 1);

		return lua_gettop(L) - after;
	}
	
	catch (char const * error)
	{
		lua_settop(L, top - 1);	// ...

		throw error;
	}
}

/// @brief Calls a Lua routine from C/C++
/// @param L Lua state
/// @param name Routine name
/// @param retc Result count (q.v. CallCore)
/// @param params Parameter descriptors (q.v. CallCore)
/// @return Number of results of call
/// @note Vararg parameters are the arguments
int Lua::Call (lua_State * L, char const * name, int retc, char const * params, ...)
{
	try {
		GetGlobal(L, name);	// func

		va_list args;	va_start(args, params);

		return CallCore(L, 0, retc, params, args);
	}
	
	catch (char const * error)
	{
		lua_getglobal(L, "printf");	// printf
		lua_pushliteral(L, "Lua::Call(%s): %s");// printf, format
		lua_pushstring(L, name);// printf, format, name
		lua_pushstring(L, error);	// printf, format, name, error
		lua_call(L, 3, 0);
	}
	
	return 0;
}

/// @brief Calls a Lua method from C/C++
/// @param L Lua state
/// @param source Source name
/// @param name Routine name
/// @param retc Result count (q.v. CallCore)
/// @param params Parameter descriptors (q.v. CallCore)
/// @return Number of results of call
/// @note Vararg parameters are the arguments
int Lua::Invoke (lua_State * L, char const * source, char const * name, int retc, char const * params, ...)
{
	try {
		GetGlobal(L, source);	// source

		lua_getfield(L, -1, name);	// ..., source, source[name]
		lua_insert(L, -2);	// ..., source[name], source

		va_list args;	va_start(args, params);

		return CallCore(L, 1, retc, params, args);
	}
	
	catch (char const * error)
	{
		lua_getglobal(L, "printf");	// printf
		lua_pushliteral(L, "Lua::Invoke(%s): %s");	// printf, format
		lua_pushstring(L, name);// printf, format, name
		lua_pushstring(L, error);	// printf, format, name, error
		lua_call(L, 3, 0);
	}
	
	return 0;
}

/// @brief Calls a Lua method from C/C++
/// @param L Lua state
/// @param source Source argument stack index
/// @param name Routine name
/// @param retc Result count (q.v. CallCore)
/// @param params Parameter descriptors (q.v. CallCore)
/// @return Number of results of call
/// @note Vararg parameters are the arguments
int Lua::Invoke (lua_State * L, int source, char const * name, int retc, char const * params, ...)
{
	if (source < 0) source = lua_gettop(L) - source - 1;

	try {
		lua_getfield(L, source, name);	// ..., source[name]
		lua_pushvalue(L, source);	// ..., source[name], source

		va_list args;	va_start(args, params);

		return CallCore(L, 1, retc, params, args);
	}
	
	catch (char const * error)
	{
		lua_getglobal(L, "printf");	// printf
		lua_pushliteral(L, "Lua::Invoke(%s): %s");	// printf, format
		lua_pushstring(L, name);// printf, format, name
		lua_pushstring(L, error);	// printf, format, name, error
		lua_call(L, 3, 0);
	}
	
	return 0;
}

/// @brief Function stub
int Lua::NOP (lua_State * L)
{
	return 0;
}

/// @brief Gets a global variable, allowing nested paths
/// @param L Lua state
/// @param name Routine name (allows for nesting)
void Lua::GetGlobal (lua_State * L, char const * name)
{
	lua_pushvalue(L, LUA_GLOBALSINDEX);	// _G

	for (char const * pDot; (pDot = strchr(name, '.')) != 0; name = pDot + 1)
	{
		lua_pushlstring(L, name, pDot - name);	// table, name
		lua_gettable(L, -2);// table, level
		lua_replace(L, -2);	// level
	}

	lua_getfield(L, -1, name);	// table, value
	lua_replace(L, -2);	// value
}

/// @brief Instantiates a class with an overloaded new function
/// @param L Lua state
/// @param type Type to instantiate
/// @param argc Minimum argument count
/// @return 1 (new instance on top of stack)
int Lua::GenericNew (lua_State * L, char const * type, int argc)
{
	if (lua_gettop(L) < argc) lua_settop(L, argc);

	class_New(L, type, lua_gettop(L));

	return 1;
}

/// @brief Data indices
enum {
	eDescriptors = 1,	///< Descriptor table index
	eNoRead,///< Non-read permission table index
	eNoWrite,	///< Non-write permission table index
	eLookup	///< Lookup means index
};

/// @brief Descriptor indices
enum {
	eDOffset = 1,	///< Offset index
	eDName,	///< Name index
	eDType	///< Type index
};

/// @brief Lookup indices
enum {
	eLType = 1,	///< Lookup type index
	eLKey	///< Key index
};

/// @brief Gets member fields
/// @note upvalue 1: Member descriptor, non-permissions, and lookup data
static u8 * GetFields (lua_State * L)
{
	lua_rawgeti(L, -1, eDOffset);	// data, key[, value], D, D[key], offset
	lua_rawgeti(L, -2, eDType);	// data, key[, value], D, D[key], offset, type

	return PU8(L, 1) + uI(L, -2);
}

#define F_(t, p) lua_pushnumber(L, *(t*)p)
#define I_(t, p) lua_pushinteger(L, *(t*)p)

/// @brief Indexes a member
/// @note upvalue 1: Member descriptor, non-permissions, and lookup data
/// @note data: Object memory
static void IndexMember (lua_State * L)
{
	// Point to the requested member.
	u8 * pData = GetFields(L);	// data, key, D, D[key], offset, type

	// Return the appropriate type.
	switch (uI(L, 6))
	{
	case Member_Reg::ePointer:
		lua_pushlightuserdata(L, *(void**)pData);	// data, key, D, D[key], offset, type, pointer
		break;
	case Member_Reg::eS8:
		I_(s8, pData);	// data, key, D, D[key], offset, type, s8
		break;
	case Member_Reg::eS16:
		I_(s16, pData);	// data, key, D, D[key], offset, type, s16
		break;
	case Member_Reg::eS32:
		I_(s32, pData);	// data, key, D, D[key], offset, type, s32
		break;
	case Member_Reg::eS64:
//			I_(s64, pData);	// data, key, D, D[key], offset, type, s64
		break;
	case Member_Reg::eSChar:
		I_(Schar, pData);	// data, key, D, D[key], offset, type, schar
		break;
	case Member_Reg::eSShort:
		I_(Sshort, pData);	// data, key, D, D[key], offset, type, sshort
		break;
	case Member_Reg::eSLong:
		I_(Slong, pData);	// data, key, D, D[key], offset, type, slong
		break;
	case Member_Reg::eSInt:
		I_(Sint, pData);	// data, key, D, D[key], offset, type, sint
		break;
	case Member_Reg::eU8:
		I_(u8, pData);	// data, key, D, D[key], offset, type, u8
		break;
	case Member_Reg::eU16:
		I_(u16, pData);	// data, key, D, D[key], offset, type, u16
		break;
	case Member_Reg::eU32:
		I_(u32, pData);	// data, key, D, D[key], offset, type, u32
		break;
	case Member_Reg::eU64:
//			I_(u64, pData);	// data, key, D, D[key], offset, type, u64
		break;
	case Member_Reg::eUChar:
		I_(Uchar, pData);	// data, key, D, D[key], offset, type, uchar
		break;
	case Member_Reg::eUShort:
		I_(Ushort, pData);	// data, key, D, D[key], offset, type, ushort
		break;
	case Member_Reg::eULong:
		I_(Ulong, pData);	// data, key, D, D[key], offset, type, ulong
		break;
	case Member_Reg::eUInt:
		I_(Uint, pData);// data, key, D, D[key], offset, type, uint
		break;
	case Member_Reg::eFloat:	
		F_(f32, pData);	// data, key, D, D[key], offset, type, fsingle
		break;
	case Member_Reg::eDouble:
//			F_(f64, pData);	// data, key, D, D[key], offset, type, fdouble
		break;
	case Member_Reg::eString:
		lua_pushstring(L, *(char**)pData);	// data, key, D, D[key], offset, type, string
		break;
	case Member_Reg::eBoolean:
		lua_pushboolean(L, *(bool*)pData);	// data, key, D, D[key], offset, type, boolean
		break;
	default:
		luaL_error(L, "Member __index: Bad type");
	}
}

#undef F_
#undef I_

/// @brief Assigns to a member
/// @note upvalue 1: Member descriptor, non-permissions, and lookup data
/// @note data: Object memory
static void NewIndexMember (lua_State * L)
{
	// Point to the requested member.
	u8 * pData = GetFields(L);	// data, key, value, D, D[key], offset, type

	// Assign the appropriate type.
	switch (uI(L, 7))
	{
	case Member_Reg::ePointer:
		*(void**)pData = UD(L, 3);
		break;
	case Member_Reg::eS8:
		*(s8*)pData = S8(L, 3);
		break;
	case Member_Reg::eS16:
		*(s16*)pData = S16(L, 3);
		break;
	case Member_Reg::eS32:
		*(s32*)pData = S32(L, 3);
		break;
	case Member_Reg::eS64:
		*(s64*)pData = S64(L, 3);
		break;
	case Member_Reg::eSChar:
		*(Schar*)pData = sC(L, 3);
		break;
	case Member_Reg::eSShort:
		*(Sshort*)pData = sS(L, 3);
		break;
	case Member_Reg::eSLong:
		*(Slong*)pData = sL(L, 3);
		break;
	case Member_Reg::eSInt:
		*(Sint*)pData = sI(L, 3);
		break;
	case Member_Reg::eU8:
		*(u8*)pData = U8(L, 3);
		break;
	case Member_Reg::eU16:
		*(u16*)pData = U16(L, 3);
		break;
	case Member_Reg::eU32:
		*(u32*)pData = U32(L, 3);
		break;
	case Member_Reg::eU64:
		*(u64*)pData = U64(L, 3);
		break;
	case Member_Reg::eUChar:
		*(Uchar*)pData = uC(L, 3);
		break;
	case Member_Reg::eUShort:
		*(Ushort*)pData = uS(L, 3);
		break;
	case Member_Reg::eULong:
		*(Ulong*)pData = uL(L, 3);
		break;
	case Member_Reg::eUInt:
		*(Uint*)pData = uI(L, 3);
		break;
	case Member_Reg::eFloat:
		*(float*)pData = F(L, 3);
		break;
	case Member_Reg::eDouble:
		*(double*)pData = D(L, 3);
		break;
	case Member_Reg::eString:
		*(char const**)pData = S(L, 3);
		break;
	case Member_Reg::eBoolean:
		*(bool*)pData = B(L, 3);
		break;
	default:
		luaL_error(L, "Member __newindex: Bad type");
	}
}

/// @brief Looks up member data
/// @note upvalue 1: Member descriptor, non-permissions, and lookup data
static void Lookup (lua_State * L)
{
	lua_rawgeti(L, lua_upvalueindex(1), eLookup);	// object, key[, value], lookup
	lua_rawgeti(L, -1, eLType);	// object, key[, value], lookup, type

	lua_Integer type = lua_tointeger(L, -1);

	lua_pop(L, 1);	// object, key[, value], lookup

	switch (type)
	{
	case eThis:
	case eEnviron:
	case eRegistry:
		lua_pushvalue(L, 1);// object, key[, value], lookup, object

		if (type != eThis) lua_gettable(L, eEnviron == type ? LUA_ENVIRONINDEX : LUA_REGISTRYINDEX);// object, key[, value], lookup, data;
		break;
	case ePointerTo:
		lua_pushlightuserdata(L, *(void**)UD(L, 1));// object, key[, value], lookup, data
		break;
	case eKey:
		lua_rawgeti(L, -2, eLKey);	// object, key[, value], lookup, key
		lua_gettable(L, 1);	// object, key[, value], lookup, data
		break;
	}

	lua_replace(L, -2);	// object, key[, value], data
}

/// @brief __index closure
/// @note upvalue 1: Member descriptor, non-permissions, and lookup data
/// @note upvalue 2: Member getter table
/// @note object: Object being accessed
/// @note key: Lookup key
static int Index (lua_State * L)
{
	Lookup(L);	// object, key, data

	// If the member is readable, perform indexing on it.
	lua_rawgeti(L, lua_upvalueindex(1), eNoRead);	// object, key, data, noread
	lua_pushvalue(L, 2);// object, key, data, noread, key
	lua_gettable(L, 4);	// object, key, data, noread, bNoRead

	if (!lua_toboolean(L, 5))
	{
		lua_pop(L, 2);	// object, key, data

		// If a getter exists for this member, return the result of its invocation.
		lua_pushvalue(L, 2);// object, key, data, key
		lua_gettable(L, lua_upvalueindex(2));	// object, key, data, getter

		if (!lua_isnil(L, 4))
		{
			lua_insert(L, 1);	// getter, object, key, data
			lua_call(L, 3, 1);	// result
		}

		// Otherwise, look up the member. If it exists, index it; otherwise, return nil
		// to let the __index metamethod continue.
		else
		{
			lua_pop(L, 1);	// object, key, data
			lua_replace(L, 1);	// data, key
			
			// Look up the member. If it exists, index it; otherwise, return nil to let
			// the __index metamethod continue.
			lua_rawgeti(L, lua_upvalueindex(1), eDescriptors);	// data, key, D
			lua_pushvalue(L, 2);// data, key, D, key
			lua_gettable(L, 3);	// data, key, D, D[key]

			if (!lua_isnil(L, 4)) IndexMember(L);	// result
		}
	}

	return 1;
}

/// @brief __newindex closure
/// @note upvalue 1: Member descriptor, non-permissions, and lookup data
/// @note upvalue 2: Member setter table
/// @note upvalue 3: Propagation boolean
/// @note object: Object being accessed
/// @note key: Lookup key
/// @note value: Value to assign
static int NewIndex (lua_State * L)
{
	Lookup(L);	// object, key, value, data

	// If the member is writeable, perform assignment to it.
	lua_rawgeti(L, lua_upvalueindex(1), eNoWrite);	// object, key, value, data, nowrite
	lua_pushvalue(L, 2);// object, key, value, data, nowrite, key
	lua_gettable(L, 5);	// object, key, value, data, nowrite, bNoWrite

	if (!lua_toboolean(L, 6))
	{
		lua_pop(L, 2);	// object, key, value, data

		// If a setter exists for this member, invoke it. If it returns a result, propagate
		// the __newindex call if allowed.
		lua_pushvalue(L, 2);// object, key, value, data, key
		lua_gettable(L, lua_upvalueindex(2));	// object, key, value, data, setter

		if (!lua_isnil(L, 5))
		{
			lua_insert(L, 1);	// setter, object, key, value, data
			lua_call(L, 4, 1);	// result

			if (!lua_isnil(L, 1) && lua_toboolean(L, lua_upvalueindex(3))) return 1;
		}

		// Otherwise, look up the member. If it exists, assign to it; otherwise, return a
		// result to let the __newindex metamethod continue.
		else
		{
			lua_pop(L, 1);	// object, key, value, data
			lua_replace(L, 1);	// data, key, value
			lua_rawgeti(L, lua_upvalueindex(1), eDescriptors);	// data, key, value, D
			lua_pushvalue(L, 2);// data, key, value, D, key
			lua_gettable(L, 4);	// data, key, value, D, D[key]

			if (!lua_isnil(L, 5)) NewIndexMember(L);

			else if (lua_toboolean(L, lua_upvalueindex(3))) return 1;
		}
	}

	return 0;
}

/// @brief Pushes __index and __newindex member binding closures onto stack
/// @param L Lua state
/// @param getters [optional] Member getter functions
/// @param setters [optional] Member setter functions
/// @param members [optional] Member descriptors
/// @param count Count of member descriptors
/// @param lookup Means of datum lookup
/// @param where [optional] If lookup is eKey, used as lookup key
/// @param bPropagateOnFail If true, __newindex calls will propagate if no setter or member exists
/// @note At least one of getters, setters, or members must be non-0 (if members, count must also be non-0)
void Lua::MembersBind (lua_State * L, luaL_Reg const * getters, luaL_Reg const * setters, Member_Reg const * members, int count, Members_Lookup lookup, char const * where, bool bPropagateOnFail)
{
	assert(0 == count || members != 0);
	assert(where != 0 || lookup != eKey);
	assert(getters != 0 || setters != 0 || (members != 0 && count > 0));

	lua_createtable(L, 4, 0);	// data

	// Build up member data.
	lua_createtable(L, 0, count);	// data, M
	lua_newtable(L);// data, M, RP
	lua_newtable(L);// data, M, RP, WP

	for (int index = 0; index < count; ++index)
	{
		// Install any permission information.
		switch (members[index].mPermissions)
		{
		case Member_Reg::eRO:
		case Member_Reg::eWO:
			lua_pushstring(L, members[index].mName.c_str());// data, M, RP, WP, name
			lua_pushboolean(L, true);	// data, M, RP, WP, name, true
			lua_settable(L, Member_Reg::eRO == members[index].mPermissions ? -4 : -3);	// data, M, RP = { ..., name = true }, WP / data, M, RP, WP = { ..., name = true }
			break;
		}

		// Load this member's information.
		lua_pushstring(L, members[index].mName.c_str());	// data, M, RP, WP, name
		lua_createtable(L, 2, 0);	// data, M, RP, WP, name, {}
		lua_pushinteger(L, members[index].mOffset);	// data, M, RP, WP, name, D, offset
		lua_pushinteger(L, members[index].mType);	// data, M, RP, WP, name, D, offset, type
		lua_rawseti(L, -3, eDType);	// data, M, RP, WP, name, D = { type = type }, offset
		lua_rawseti(L, -2, eDOffset);	// data, M, RP, WP, name, D = { type, offset = offset }
		lua_settable(L, -5);// data, M = { ..., name = D }, RP, WP
	}

	lua_rawseti(L, -4, eNoWrite);	// data = { nowrite = WP }, M, RP
	lua_rawseti(L, -3, eNoRead);// data = { nowrite, noread = RP }, M
	lua_rawseti(L, -2, eDescriptors);	// data = { nowrite, noread, descriptors = M }

	// Install lookup information.
	lua_createtable(L, 2, 0);	// data, L
	lua_pushinteger(L, lookup);	// data, L, lookup
	lua_rawseti(L, -2, eLType);	// data, L = { type = lookup }

	if (eKey == lookup)
	{
		lua_pushstring(L, where);	// data, L, where
		lua_rawseti(L, -2, eLKey);	// data, L = { type, where = where }
	}

	lua_rawseti(L, -2, eLookup);// data = { nowrite, noread, descriptors, lookup = L }

	// Build __index closure.
	lua_pushvalue(L, -1);	// data, data
	lua_newtable(L);// data, data, {}

	if (getters != 0) luaL_register(L, 0, getters);

	lua_pushcclosure(L, Index, 2);	// data, __index

	// Build __newindex closure.
	lua_insert(L, -2);	// __index, data
	lua_newtable(L);// __index, data, {}

	if (setters != 0) luaL_register(L, 0, setters);

	lua_pushboolean(L, bPropagateOnFail);	// __index, data, setters, bPropagateOnFail
	lua_pushcclosure(L, NewIndex, 3);	// __index, __newindex
}

/// @brief Resumes a coroutine
/// @param L Lua state
/// @param co Coroutine reference
void Lua::Resume (lua_State * L, int co)
{
	if (LUA_REFNIL == co) return;

	lua_rawgeti(L, LUA_REGISTRYINDEX, co);	// proc
	lua_resume(lua_tothread(L, -1), 0);
	lua_pop(L, 1);
}

/// @brief Sets a global variable, allowing nested paths
/// @param L Lua state
/// @param name Global name (allows for nesting)
/// @note value: Value to assign
void Lua::SetGlobal (lua_State * L, char const * name)
{
	lua_pushvalue(L, LUA_GLOBALSINDEX);	// value, _G

	for (char const * pDot; (pDot = strchr(name, '.')) != 0; name = pDot + 1)
	{
		lua_pushlstring(L, name, pDot - name);	// value, table, name
		lua_gettable(L, -2);// value, table, level
		lua_replace(L, -2);	// value, level
	}

	lua_insert(L, -2);	// table, value
	lua_setfield(L, -2, name);	// table[name] = value
	lua_pop(L, 1);	// stack clear
}

/// @brief Constructs an Overload
/// @param L Lua state
/// @param argc Count of arguments to overloaded function
Overload::Overload (lua_State * L, int argc) : mL(L), mArgs(argc, 's')
{
	class_New(L, "Generic", "i", argc);	// ..., G
}

/// @brief Adds a function defintion
/// @param func Function to be invoked
/// @note Vararg parameters are the argument types on which to invoke
/// @note Overload must be on the stack top
void Overload::AddDef (lua_CFunction func, ...)
{
	va_list args;	va_start(args, func);

	lua_getfield(mL, -1, "Define");	// ..., G, G.Define
	lua_pushvalue(mL, -2);	// ..., G, G.Define, G
	lua_pushcfunction(mL, func);// ..., G, G.Define, G, func

	CallCore(mL, 2, 0, mArgs.c_str(), args);
}