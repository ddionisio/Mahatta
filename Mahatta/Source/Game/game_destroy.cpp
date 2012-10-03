#include "game_local.h"
#include "game_lua.h"

void GameDestroy()
{
	//destroy world
	g_gi.WorldDestroy();

	//destroy game globals
	g_game.player = 0;

	if(g_game.view) { g_game.view->Release(); g_game.view = 0; }

	for(u32 i = 0; i < g_game.sceneFX.size(); i++)
	{
		if(g_game.sceneFX[i])
		{
			g_game.sceneFX[i]->Release();
			g_game.sceneFX[i] = 0;
		}
	}

	if(g_game.loadImg)
	{ g_game.loadImg->Release(); g_game.loadImg = 0; }

	if(g_game.dbgFnt)
	{ g_game.dbgFnt->Release(); g_game.dbgFnt = 0; }

	if(g_game.flareImg)
	{ g_game.flareImg->Release(); g_game.flareImg = 0; }

	// Shut down Lua.
	if (g_game.mLua != 0)
	{
		Lua::Call(g_game.mLua, "CloseLua", 0, "");

		lua_close(g_game.mLua);

		g_game.mLua = 0;
	}

	//clear out particle system
	g_gi.ParSysShutdown();

	HUDDestroy();
}