#include "game_lua.h"
#include "game_lua_i.h"
#include "game_sequence.h"

using namespace Lua;

///
/// Type handlers
///
template<typename T> static T * UDT (lua_State * L, int index)
{
	return *static_cast<T**>(UD(L, index));
}

static inline Sequence::Sequence * US (lua_State * L, int index)
{
	return UDT<Sequence::Sequence>(L, index);
}

static inline Sequence::Interval * Ui (lua_State * L, int index)
{
	return UDT<Sequence::Interval>(L, index);
}

static inline Sequence::Spot * Us (lua_State * L, int index)
{
	return UDT<Sequence::Spot>(L, index);
}

/// @brief Void return; typed, unsigned argument; registry
template<typename T> static int V_T_r (lua_State * L, void (T::* func)(void))
{
	(UDT<T>(L, 1)->*func)();

	return 0;
}

/// @brief Unsigned return; typed, unsigned, unsigned argument; registry
template<typename T> static int U_TUU_r (lua_State * L, Uint (T::* func)(Uint, Uint))
{
	lua_pushinteger(L, (UDT<T>(L, 1)->*func)(uI(L, 2), uI(L, 3)));

	return 1;
}

///
/// Interval functions
///
static int IntervalClear (lua_State * L)
{
	return V_T_r<Sequence::Interval>(L, &Sequence::Interval::Clear);
}

static int IntervalRemove (lua_State * L)
{
	return V_T_r<Sequence::Interval>(L, &Sequence::Interval::Remove);
}

static int IntervalGet (lua_State * L)
{
	Uint where, count;

	count = Ui(L, 1)->Get(where);

	if (count != 0)
	{
		lua_pushinteger(L, where);
		lua_pushinteger(L, count);

		return 2;
	}

	return 0;
}

static int IntervalSet (lua_State * L)
{
	return U_TUU_r<Sequence::Interval>(L, &Sequence::Interval::Set);
}

///
/// Spot functions
///
static int SpotClear (lua_State * L)
{
	return V_T_r<Sequence::Spot>(L, &Sequence::Spot::Clear);
}

static int SpotRemove (lua_State * L)
{
	return V_T_r<Sequence::Spot>(L, &Sequence::Spot::Remove);
}

static int SpotSet (lua_State * L)
{
	Us(L, 1)->Set(uI(L, 2));

	return 0;
}

static int SpotGet (lua_State * L)
{
	Uint where;

	if (Us(L, 1)->Get(where))
	{
		lua_pushinteger(L, where);

		return 1;
	}

	return 0;
}

///
/// Sequence functions
///
static int SequenceInsertItems (lua_State * L)
{
	US(L, 1)->InsertItems(uI(L, 2), uI(L, 3));

	return 0;
}

static int SequenceCreateInterval (lua_State * L)
{
	lua_pushlightuserdata(L, US(L, 1)->CreateInterval());	// ni

	class_New(L, "Interval", 1);// interval

	return 1;
}

static int SequenceCreateSpot (lua_State * L)
{
	lua_pushlightuserdata(L, US(L, 1)->CreateSpot(B(L, 2), B(L, 3)));	// ns

	class_New(L, "Spot", 1);// spot

	return 1;
}

static int SequenceIsItemValid (lua_State * L)
{
	lua_pushboolean(L, US(L, 1)->IsItemValid(uI(L, 2), B(L, 3)));

	return 1;
}

static int SequenceGetItemCount (lua_State * L)
{
	lua_pushinteger(L, US(L, 1)->GetItemCount());

	return 1;
}

static int SequenceRemoveItems (lua_State * L)
{
	return U_TUU_r<Sequence::Sequence>(L, &Sequence::Sequence::RemoveItems);
}

static int SequenceTrim (lua_State * L)
{
	return U_TUU_r<Sequence::Sequence>(L, &Sequence::Sequence::Trim);
}

#define M_(w) { #w, Interval##w }

///
/// Garbage collectors
///
int Sequence__gc (lua_State * L)
{
	delete US(L, 1);

	return 0;
}

///
/// Function tables
///
static const luaL_reg IntervalFuncs[] = {
	M_(Clear),
	M_(Remove),
	M_(Get),
	M_(Set),
	{ 0, 0 }
};

#undef M_
#define M_(w) { #w, Spot##w }

static const luaL_reg SpotFuncs[] = {
	M_(Clear),
	M_(Remove),
	M_(Set),
	M_(Get),
	{ 0, 0 }
};

#undef M_
#define M_(w) { #w, Sequence##w }

static const luaL_reg SequenceFuncs[] = {
	M_(__gc),
	M_(InsertItems),
	M_(CreateInterval),
	M_(CreateSpot),
	M_(IsItemValid),
	M_(GetItemCount),
	M_(RemoveItems),
	M_(Trim),
	{ 0, 0 }
};

#undef M_

///
/// New functions
///
static int NewF (lua_State * L, void * data, size_t size)
{
	memcpy(UD(L, 1), &data, size);

	return 0;
}

static int IntervalNew (lua_State * L)
{
	return NewF(L, UD(L, 2), sizeof(Sequence::Interval*));
}

static int SpotNew (lua_State * L)
{
	return NewF(L, UD(L, 2), sizeof(Sequence::Spot*));
}

static int SequenceNew (lua_State * L)
{
	return NewF(L, new Sequence::Sequence, sizeof(Sequence::Sequence*));
}

/// @brief Binds the sequence system to the Lua scripting system
void LuaI::open_sequence (lua_State * L)
{
	class_Define(L, "Interval", IntervalFuncs, IntervalNew, 0, sizeof(Sequence::Interval*));
	class_Define(L, "Spot", SpotFuncs, SpotNew, 0, sizeof(Sequence::Spot*));
	class_Define(L, "Sequence", SequenceFuncs, SequenceNew, 0, sizeof(Sequence::Sequence*));
}