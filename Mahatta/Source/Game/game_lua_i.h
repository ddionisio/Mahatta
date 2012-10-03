#ifndef GAME_LUA_I_H
#define GAME_LUA_I_H

#include "game_share.h"

namespace LuaI
{
	// Library
	void open_class (lua_State * L);
	void open_state (lua_State * L);
	void open_std (lua_State * L);

	// Extensions
	void open_bit (lua_State * L);
	void open_graphics (lua_State * L);
	void open_input (lua_State * L);
	void open_mathops (lua_State * L);
	void open_sequence (lua_State * L);
	void open_voxel (lua_State * L);
}

#endif // GAME_LUA_I_H