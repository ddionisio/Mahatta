#include "game_local.h"
#include "game_lua.h"

void GameRenderFrame()
{
	if (g_game.view != 0)
	{
		g_game.view->SetScene();

		g_gi.SkyRender();

		g_gi.WorldDisplay(g_game.view->GetPt());

		//insert game specific stuff here
		if(g_game.curFX == -1 || g_game.sceneFX.size() == 0 || !g_game.sceneFX[g_game.curFX])
			g_gi.GFXSceneRender();
		else
			g_game.sceneFX[g_game.curFX]->SceneRender();

		g_game.numTri = g_gi.GFXSceneGetNumTri();

		g_gi.GFXSceneClear();
	}

	// Render scripted objects.
	if (g_game.mLua != 0) Lua::Call(g_game.mLua, "RenderLua", 0, "");

	/*g_gi.TextureBegin();

	Vec3D flareScnLoc;
	g_gi.GFXProjectPt(&g_game.flareLoc, &flareScnLoc);

	g_game.flareImg->Blt(flareScnLoc.x, flareScnLoc.y, 0,0);

	g_gi.TextureEnd();*/
}