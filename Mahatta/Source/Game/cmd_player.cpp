#include "game_ball.h"
#include "game_match.h"
#include "game_team.h"
#include "game_teamplayer.h"
#include "game_local.h"

//player rotates left
void CMD_Player_Rot_L()
{
	f32 tick = g_gi.EngineGetTick();

	if(g_game.player)
	{
		g_game.player->Rotate(tick*2);
	}
}

//player rotates right
void CMD_Player_Rot_R()
{
	f32 tick = g_gi.EngineGetTick();

	if(g_game.player)
	{
		g_game.player->Rotate(-tick*2);
	}
}

//player moves forward its current dir
void CMD_Player_Forward()
{
	if(g_game.player->isControllable())
		Match::getMe()->setMovementDir(MOV_FORWARD);
}

//player moves backward its current dir
void CMD_Player_Backward()
{
	if(g_game.player->isControllable())
		Match::getMe()->setMovementDir(MOV_BACKWARD);
}

//player strafe left
void CMD_Player_MoveLeft()
{
	if(g_game.player->isControllable())
		Match::getMe()->setMovementDir(MOV_LEFT);
}

//player strafe right
void CMD_Player_MoveRight()
{
	if(g_game.player->isControllable())
		Match::getMe()->setMovementDir(MOV_RIGHT);
}

//player run
void CMD_Player_Run_Enable()
{
	if(g_game.player)
		g_game.player->SetFlag(PLYR_FLAG_RUN, TRUE);
}

void CMD_Player_Run_Disable()
{
	if(g_game.player)
		g_game.player->SetFlag(PLYR_FLAG_RUN, FALSE);
}

//look at
void CMD_Player_LookAt_Enable()
{
	g_game.viewbLook = TRUE;
}

void CMD_Player_LookAt_Disable()
{
	g_game.viewbLook = FALSE;
}

void CMD_Player_Run_Toggle()
{
	if(g_game.player)
	{
		if(g_game.player->CheckFlag(PLYR_FLAG_RUNTOGGLE))
			g_game.player->SetFlag(PLYR_FLAG_RUNTOGGLE, FALSE);
		else
			g_game.player->SetFlag(PLYR_FLAG_RUNTOGGLE, TRUE);
	}
}

void CMD_Player_Shoot()
{
	if (g_game.player != 0 && g_game.player->isControllable())
		g_game.player->determineShootAction();
}

void CMD_Change_Player(){
	if(g_game.localteam)
		g_game.localteam->changeToNextAvailablePlayer();
}

void CMD_Reset_Ball(){
	if(g_game.player)
		Ball::getMe()->reset();
}

void CMD_God_Mode(){
	if(g_game.player)
		g_game.player->activateGodMode();
}

void CMD_Player_Up(){
	if(g_game.player && g_game.player->isGodModeOn())
		g_game.player->MoveV(50.0f);
}

void CMD_Player_Down(){
	if(g_game.player && g_game.player->isGodModeOn())
		g_game.player->MoveV(-50.0f);
}
