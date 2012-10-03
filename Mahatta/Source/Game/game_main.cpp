#include "game_local.h"

GameImport	g_gi;

GameExport	g_ge; //also contains game engine globals

GameLocal	g_game={0};

//other globals

void GameInit();
void GameDestroy();
void GameSpawnEntity(const EntityParse & entDat);
void GameWriteGame(const tCHAR *filename, u8 bAutosave);
void GameReadGame(const tCHAR *filename);
void GameWriteLevel(const tCHAR *filename);
void GameReadLevel(const tCHAR *filename);
void GameUpdateFrame();
void GameRenderFrame();

/////////////////////////////////////
// Name:	GetAPI
// Purpose:	Initialize module here
//			This is where we get our
//			imports.
// Output:	g_gi set, g_ge initialized
// Return:	the export for the game engine
/////////////////////////////////////
GameExport * F_API GetAPI (GameImport *import)
{
	g_gi = *import;

	g_ge.version = GAME_VERSION;

	//set the function pointers (interfaces)
	g_ge.Init = GameInit;
	g_ge.Destroy = GameDestroy;
	g_ge.SpawnEntity = GameSpawnEntity;
	g_ge.WriteGame = GameWriteGame;
	g_ge.ReadGame = GameReadGame;
	g_ge.WriteLevel = GameWriteLevel;
	g_ge.ReadLevel = GameReadLevel;
	g_ge.UpdateFrame = GameUpdateFrame;
	g_ge.RenderFrame = GameRenderFrame;

	return &g_ge;
}

/////////////////////////////////////
// Name:	GameDisplayLoad
// Purpose:	display the loading thing
//			centered
// Output:	stuff
// Return:	none
/////////////////////////////////////
void GameDisplayLoad(u8 bClear)
{
	if(g_game.loadImg)
	{
		gfxMode m; g_gi.GFXGetMode(&m);

		u32 tW = g_game.loadImg->GetWidth();
		u32 tH = g_game.loadImg->GetHeight();

		g_gi.EngineLoadDisplay(g_game.loadImg, 
			(f32)((m.width>>1)-(tW>>1)), 
			(f32)(f32)((m.height>>1)-(tH>>1)), bClear);
	}
}