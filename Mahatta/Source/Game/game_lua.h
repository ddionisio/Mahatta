#ifndef GAME_LUA_H
#define GAME_LUA_H

#include "game_local.h"
#include "game_share.h"
#include <complex>

namespace Lua
{
	lua_State * NewState (void);

	// Signed access
	s8 S8 (lua_State * L, int index);
	s16 S16 (lua_State * L, int index);
	s32 S32 (lua_State * L, int index);
	s64 S64 (lua_State * L, int index);
	Schar sC (lua_State * L, int index);
	Sshort sS (lua_State * L, int index);
	Slong sL (lua_State * L, int index);
	Sint sI (lua_State * L, int index);

	// Unsigned access
	u8 U8 (lua_State * L, int index);
	u16 U16 (lua_State * L, int index);
	u32 U32 (lua_State * L, int index);
	u64 U64 (lua_State * L, int index);
	Uchar uC (lua_State * L, int index);
	Ushort uS (lua_State * L, int index);
	Ulong uL (lua_State * L, int index);
	Uint uI (lua_State * L, int index);

	// Floating point access
	f32 F (lua_State * L, int index);
	f64 D (lua_State * L, int index);

	// Memory access
	void * UD (lua_State * L, int index);
	u8 * PU8 (lua_State * L, int index);

	// String access
	char const * S (lua_State * L, int index);

	// Boolean access
	bool B (lua_State * L, int index);

	// Type access
	std::complex<float> * Complex_ (lua_State * L, int index, char const * what = "");
	Matrix * Matrix_ (lua_State * L, int index, char const * what = "");
	Vec3D * Vec3D_ (lua_State * L, int index, char const * what = "");

	std::wstring * WS_ (lua_State * L, int index);

	class Steering * Steering_ (lua_State * L, int index);
	class Team * Team_ (lua_State * L, int index);
	class TeamPlayer * TeamPlayer_ (lua_State * L, int index);

	// Call support
	int CallCore (lua_State * L, int count, int retc, char const * params, va_list & args);
	int NOP (lua_State * L);

	// Type support
	int GenericNew (lua_State * L, char const * type, int argc);

	/// @brief Member specification
	struct Member_Reg {
		size_t mOffset;	///< Member offset
		std::string mName;	///< Member name
		enum Type {
			ePointer,	///< Pointer member
			eS8, eS16, eS32, eS64,	///< Sized members, signed
			eU8, eU16, eU32, eU64,	///< Sized members, unsigned
			eSChar, eSShort, eSLong, eSInt,	///< Integer primitives, signed
			eUChar, eUShort, eULong, eUInt,	///< Integer primitives, unsigned
			eString,///< String
			eBoolean,	///< Boolean
			eFloat, eDouble	///< Single- and double-precision floating point
		} mType;///< Member type
		enum Permissions {
			eFull,	///< Full permissions
			eRO, eWO	///< Read-only or write-only
		} mPermissions;	///< Member permissions

		Member_Reg (void) : mPermissions(eFull)
		{
		}

		void Set (size_t offset, std::string const & name, Type type)
		{
			mOffset = offset;
			mName = name;
			mType = type;
		}
	};

	/// @brief Means of member data lookup
	enum Members_Lookup {
		eThis,	///< The datum occupies the instance memory
		eEnviron,	///< The datum is stored in the environment, with the instance as key
		ePointerTo,	///< A pointer to the datum occupies the instance memory
		eKey,	///<  The datum is stored with a given key by the instance
		eRegistry	///< The datum is stored in the registry, with the instance as key
	};

	// Library calls
	void class_Define (lua_State * L, char const * name, luaL_Reg const * methods, lua_CFunction newf, char const * base, Uint size);
	void class_Define (lua_State * L, char const * name, luaL_Reg const * methods, char const * base, Uint size);
	void class_Define (lua_State * L, char const * name, luaL_Reg const * methods, char const * closures[], lua_CFunction newf, char const * base, Uint size);
	void class_Define (lua_State * L, char const * name, luaL_Reg const * methods, char const * closures[], char const * base, Uint size);
	void class_New (lua_State * L, char const * name, int count);
	void class_New (lua_State * L, char const * name, char const * params, ...);
	void class_SCons (lua_State * L, char const * base, int count);
	void class_SCons (lua_State * L, char const * base, char const * params, ...);

	bool class_IsInstance (lua_State * L, int index);
	bool class_IsType (lua_State * L, int index, char const * type);

	// Helper calls
	int Call (lua_State * L, char const * name, int retc, char const * params, ...);
	int Invoke (lua_State * L, char const * source, char const * name, int retc, char const * params, ...);
	int Invoke (lua_State * L, int source, char const * name, int retc, char const * params, ...);
	void GetGlobal (lua_State * L, char const * name);
	void MembersBind (lua_State * L, luaL_Reg const * getters, luaL_Reg const * setters, Member_Reg const * members, int count, Members_Lookup lookup, char const * where = 0, bool bPropagateOnFail = false);
	void Resume (lua_State * L, int co);
	void SetGlobal (lua_State * L, char const * name);

	/// @brief Overloaded function builder
	struct Overload {
		std::string mArgs;	///< String used to fetch arguments
		lua_State * mL;	///< Lua state

		Overload (lua_State * L, int argc);

		void AddDef (lua_CFunction func, ...);
	};

	// Game bindings
	void BindGameLogic (void);

	/// @brief Templated finalizer (handle)
	template<typename T> static int _gcT (lua_State * L)
	{
		(*(T*)UD(L, 1))->Release();

		return 0;	
	}

	/// @brief Templated type accessor (handle)
	template<typename T> T _hT (lua_State * L, int index)
	{
		return *(T*)UD(L, index);
	}

	/// @brief Templated type stub
	template<typename T> char const * _typeT (void)
	{
		return "";
	}

	/// @brief Templated reference type stub
	template<typename T> char const * _rtypeT (void)
	{
		return "";
	}

	/// @brief Templated type accessor (embedded)
	template<typename T> T * _pT (lua_State * L, int index, char const * what = "access")
	{
		// Given an instance, supply its memory; if it is a non-T type, report an error.
		// Otherwise, simply return the non-instance's memory.
		if (class_IsInstance(L, index))
		{
			// If the instance is a T reference, look up its memory.
			if (class_IsType(L, index, _rtypeT<T>())) return *(T**)UD(L, index);

			// Otherwise, point to its memory.
			if (class_IsType(L, index, _typeT<T>())) return (T*)UD(L, index);

			luaL_error(L, "%s: non-%s/%s arg %i", what, _typeT<T>(), _rtypeT<T>(), index);
		}

		return (T*)UD(L, index);
	}

	/// @brief Templated copy constructor (handle)
	template<typename T> static int _refT (lua_State * L)
	{
		T t = T(UD(L, 2));

		t->AddRef();

		*(T*)UD(L, 1) = t;

		return 0;	
	}

	/// @brief Templated copy constructor (embedded)
	template<typename T> int _copyT (lua_State * L, T & t)
	{
		class_New(L, _typeT<T>(), "u", &t);	// t

		return 1;
	}

	/// @brief Lua object to C++ class pointer
	template<typename T> T * _toclassT (lua_State * L, char const * name)
	{
		lua_getglobal(L, name);
	
		T * pT = static_cast<T*>(UD(L, -1));

		lua_pop(L, 1);

		return pT;
	}

	// @brief Lua object to C++ class pointer
	template<typename T> T * _toclassT (lua_State * L, int index, bool bRegistry = true)
	{
		lua_rawgeti(L, bRegistry ? LUA_REGISTRYINDEX : LUA_ENVIRONINDEX, index);

		T * pT = static_cast<T*>(UD(L, -1));

		lua_pop(L, 1);

		return pT;
	}
}

#endif // GAME_LUA_H