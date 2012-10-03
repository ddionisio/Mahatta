#include "game_local.h"

//For saving and loading

// Read/Write Game is for storing persistant cross level information
// about the world state and the clients.
// WriteGame is called every time a level is exited.
// ReadGame is called on a loadgame.
void GameWriteGame(const tCHAR *filename, u8 bAutosave)
{
}

void GameReadGame(const tCHAR *filename)
{
}

// ReadLevel is called after the default map information has been
// loaded

//this is called everytime we destroy the world(level)
void GameWriteLevel(const tCHAR *filename)
{
	//destroy stuff we loaded in level
	g_gi.SkyDestroy();
}

void GameReadLevel(const tCHAR *filename)
{
	/////////////////////////////////////////////////
	//initialize camera
	if(g_game.view) { g_game.view->Release(); g_game.view = 0; }

	//get the targets: camLoc, camTgt
	Vec3D camLoc(0,0,0), camTgt(0,0,0);

	g_gi.WorldTargetGet(L"camLoc", &camLoc);
	g_gi.WorldTargetGet(L"camTgt", &camTgt);

	g_game.view = g_gi.ViewCreate(camLoc, camTgt);

	f32 vDelay=100;
	g_gi.ValueGetFloat(L"viewDelay", &vDelay);
	g_game.view->SetDelay(vDelay);

	g_game.view->m_bStereo = FALSE;
	/////////////////////////////////////////////////

	g_gi.WorldTargetGet(L"flare_test", &g_game.flareLoc);
}