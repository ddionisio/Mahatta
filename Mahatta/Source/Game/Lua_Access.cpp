#include "game_lua.h"

namespace Lua
{
	/// @brief Validates and returns a s8 argument
	/// @param L Lua state
	/// @param index Argument index
	/// @return s8 value
	s8 S8 (lua_State * L, int index)
	{
		return static_cast<s8>(luaL_checkint(L, index));
	}

	/// @brief Validates and returns a s16 argument
	/// @param L Lua state
	/// @param index Argument index
	/// @return s16 value
	s16 S16 (lua_State * L, int index)
	{
		return static_cast<s16>(luaL_checkint(L, index));
	}

	/// @brief Validates and returns a s32 argument
	/// @param L Lua state
	/// @param index Argument index
	/// @return s32 value
	s32 S32 (lua_State * L, int index)
	{
		return static_cast<s32>(luaL_checkint(L, index));
	}

	/// @brief Validates and returns a s64 argument
	/// @param L Lua state
	/// @param index Argument index
	/// @return s64 value
	s64 S64 (lua_State * L, int index)
	{
		return static_cast<s64>(luaL_checkint(L, index));
	}

	/// @brief Validates and returns a signed char argument
	/// @param L Lua state
	/// @param index Argument index
	/// @return signed char value
	Schar sC (lua_State * L, int index)
	{
		return static_cast<signed char>(luaL_checkint(L, index));
	}

	/// @brief Validates and returns a signed short argument
	/// @param L Lua state
	/// @param index Argument index
	/// @return signed short value
	Sshort sS (lua_State * L, int index)
	{
		return static_cast<signed short>(luaL_checkint(L, index));
	}

	/// @brief Validates and returns a signed long argument
	/// @param L Lua state
	/// @param index Argument index
	/// @return signed long value
	Slong sL (lua_State * L, int index)
	{
		return static_cast<signed long>(luaL_checkint(L, index));
	}

	/// @brief Validates and returns a signed int argument
	/// @param L Lua state
	/// @param index Argument index
	/// @return signed int value
	Sint sI (lua_State * L, int index)
	{
		return static_cast<signed int>(luaL_checkint(L, index));
	}

	/// @brief Validates and returns a u8 argument
	/// @param L Lua state
	/// @param index Argument index
	/// @return u8 value
	u8 U8 (lua_State * L, int index)
	{
		return static_cast<u8>(luaL_checkint(L, index));
	}

	/// @brief Validates and returns a u16 argument
	/// @param L Lua state
	/// @param index Argument index
	/// @return u16 value
	u16 U16 (lua_State * L, int index)
	{
		return static_cast<u16>(luaL_checkint(L, index));
	}

	/// @brief Validates and returns a u32 argument
	/// @param L Lua state
	/// @param index Argument index
	/// @return u32 value
	u32 U32 (lua_State * L, int index)
	{
		return static_cast<u32>(luaL_checkint(L, index));
	}

	/// @brief Validates and returns a u64 argument
	/// @param L Lua state
	/// @param index Argument index
	/// @return u64 value
	u64 U64 (lua_State * L, int index)
	{
		return static_cast<u64>(luaL_checkint(L, index));
	}

	/// @brief Validates and returns an unsigned char argument
	/// @param L Lua state
	/// @param index Argument index
	/// @return unsigned char value
	Uchar uC (lua_State * L, int index)
	{
		return static_cast<unsigned char>(luaL_checkint(L, index));
	}

	/// @brief Validates and returns an unsigned short argument
	/// @param L Lua state
	/// @param index Argument index
	/// @return unsigned short value
	Ushort uS (lua_State * L, int index)
	{
		return static_cast<unsigned short>(luaL_checkint(L, index));
	}

	/// @brief Validates and returns an unsigned long argument
	/// @param L Lua state
	/// @param index Argument index
	/// @return unsigned long value
	Ulong uL (lua_State * L, int index)
	{
		return static_cast<unsigned long>(luaL_checkint(L, index));
	}

	/// @brief Validates and returns a signed int argument
	/// @param L Lua state
	/// @param index Argument index
	/// @return unsigned int value
	Uint uI (lua_State * L, int index)
	{
		return static_cast<unsigned int>(luaL_checkint(L, index));
	}

	/// @brief Validates and return a float argument
	/// @param L Lua state
	/// @param index Argument index
	/// @return float value
	f32 F (lua_State * L, int index)
	{
		return static_cast<float>(luaL_checknumber(L, index));
	}

	/// @brief Validates and return a double argument
	/// @param L Lua state
	/// @param index Argument index
	/// @return double value
	f64 D (lua_State * L, int index)
	{
		return static_cast<double>(luaL_checknumber(L, index));
	}

	/// @brief Validates and returns a void * argument
	/// @param L Lua state
	/// @param index Argument index
	/// @return void * value
	void * UD (lua_State * L, int index)
	{
		if (!lua_isuserdata(L, index)) luaL_error(L, "Argument %d is not a userdata", index);

		return lua_touserdata(L, index);
	}

	/// @brief Validates and returns a Uint8 * argument
	/// @param L Lua state
	/// @param index Argument index
	/// @return u8 * value
	u8 * PU8 (lua_State * L, int index)
	{
		return static_cast<u8*>(UD(L, index));
	}

	/// @brief Validates and returns a char const * argument
	/// @param L Lua state
	/// @param index Argument index
	/// @return char const * value
	char const * S (lua_State * L, int index)
	{
		return luaL_checkstring(L, index);
	}

	/// @brief Validates and returns a bool argument
	/// @param L Lua state
	/// @param index Argument index
	/// @return bool value
	bool B (lua_State * L, int index)
	{
		luaL_checktype(L, index, LUA_TBOOLEAN);

		return lua_toboolean(L, index) != 0;
	}
}