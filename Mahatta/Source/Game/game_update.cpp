#include "game_local.h"
#include "game_lua.h"

void GameUpdateFrame()
{
	Lua::Call(g_game.mLua, "UpdateLua", 0, "");
}