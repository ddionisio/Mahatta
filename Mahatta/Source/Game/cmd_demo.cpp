#include "game_teamplayer.h"
#include "game_local.h"

void CMD_Demo_Change_SceneFX()
{
	if(g_game.curFX == g_game.sceneFX.size()-1)
		g_game.curFX = -1;
	else
		g_game.curFX++;
//	g_gi.EngineVideoPlay(L"alienwalk2.avi");
}

void CMD_Demo_Quit()
{
	g_gi.EngineQuit();
}

void CMD_Open_Menu()
{
//	g_gi.EngineOpenMenu(L"mainmenu", TRUE);
//	TODO: Merge with script
}

void CMD_Console()
{
//	g_gi.EngineOpenMenu(L"console", !g_gi.EngineIsMenuActive(L"console"));
//	TODO: Move to script (probably S_Game)
}

void CMD_HelpToggle()
{
	if(TESTFLAGS(g_game.flag, GAME_HELP_TOGGLE))
		CLEARFLAG(g_game.flag, GAME_HELP_TOGGLE);
	else
		SETFLAG(g_game.flag, GAME_HELP_TOGGLE);
}

void CMD_FPSToggle()
{
	if(TESTFLAGS(g_game.flag, GAME_SHOW_FPS))
		CLEARFLAG(g_game.flag, GAME_SHOW_FPS);
	else
		SETFLAG(g_game.flag, GAME_SHOW_FPS);
}
/**
*/
void CMD_Player (void)
{
	g_game.player->GetEnt()->SetMDL(g_gi.CmdArgV(0));
}
/*
**/
