#include "game_local.h"

// Bind Information
eInputType *theInputType; // assume for now that all bind keys are on the same device
					      // TODO: contemplate use of multiple devices?
u8 *kForward, *kBack, *kLeft, *kRight; // keys associated to "+moveleft", "+moveright", "+forward", "+back"

void CMD_Player_Rot_L();
void CMD_Player_Rot_R();

void CMD_Player_Forward();
void CMD_Player_Backward();

void CMD_Player_MoveLeft();
void CMD_Player_MoveRight();

void CMD_Player_Run_Enable();
void CMD_Player_Run_Disable();

void CMD_Player_LookAt_Enable();
void CMD_Player_LookAt_Disable();

void CMD_Player_Run_Toggle();

void CMD_Player_Shoot();

void CMD_Demo_Change_SceneFX();
void CMD_Demo_Quit();

void CMD_Open_Menu();

void CMD_Console();

void CMD_HelpToggle();
void CMD_FPSToggle();

void CMD_Player (void);
void CMD_Change_Player();

void CMD_Reset_Ball();

void CMD_God_Mode();

void CMD_Player_Up();
void CMD_Player_Down();

/////////////////////////////////////
// Name:	GameCmdInit
// Purpose:	initialize all commands
//			for game module
// Output:	all commands initialized
// Return:	none
/////////////////////////////////////
void GameCmdInit()
{
	g_gi.CmdAdd(L"+turnleft", CMD_Player_Rot_L);
	g_gi.CmdAdd(L"-turnleft", CMD_Player_Rot_L);

	g_gi.CmdAdd(L"+turnright", CMD_Player_Rot_R);
	g_gi.CmdAdd(L"-turnright", CMD_Player_Rot_R);

	g_gi.CmdAdd(L"+forward", CMD_Player_Forward);
	g_gi.CmdAdd(L"-forward", CMD_Player_Forward);

	g_gi.CmdAdd(L"+back", CMD_Player_Backward);
	g_gi.CmdAdd(L"-back", CMD_Player_Backward);

	g_gi.CmdAdd(L"+moveleft", CMD_Player_MoveLeft);
	g_gi.CmdAdd(L"-moveleft", CMD_Player_MoveLeft);

	g_gi.CmdAdd(L"+moveright", CMD_Player_MoveRight);
	g_gi.CmdAdd(L"-moveright", CMD_Player_MoveRight);

	g_gi.CmdAdd(L"+run", CMD_Player_Run_Enable);
	g_gi.CmdAdd(L"-run", CMD_Player_Run_Disable);

	g_gi.CmdAdd(L"runtoggle", CMD_Player_Run_Toggle);

	g_gi.CmdAdd(L"+lookat", CMD_Player_LookAt_Enable);
	g_gi.CmdAdd(L"-lookat", CMD_Player_LookAt_Disable);

	g_gi.CmdAdd(L"ball", CMD_Player_Shoot);

	g_gi.CmdAdd(L"changescenefx", CMD_Demo_Change_SceneFX);
	
	g_gi.CmdAdd(L"quit", CMD_Demo_Quit);

	g_gi.CmdAdd(L"menu", CMD_Open_Menu);

	g_gi.CmdAdd(L"console", CMD_Console);

	g_gi.CmdAdd(L"help", CMD_HelpToggle);
	g_gi.CmdAdd(L"displayinfo", CMD_FPSToggle);

	g_gi.CmdAdd(L"Player", CMD_Player);

	g_gi.CmdAdd(L"changeplayer", CMD_Change_Player);

	g_gi.CmdAdd(L"resetball", CMD_Reset_Ball);

	g_gi.CmdAdd(L"godmode", CMD_God_Mode);

	g_gi.CmdAdd(L"playerup", CMD_Player_Up);
	g_gi.CmdAdd(L"playerdown", CMD_Player_Down);

	// Get keys and input types associated to bind parameters
	// bind parameters: "+moveleft", "+moveright", "+forward", "+back"
	g_gi.BindGetKey(L"+forward", theInputType, kForward);
	g_gi.BindGetKey(L"+back", theInputType, kBack);
	g_gi.BindGetKey(L"+moveleft", theInputType, kLeft);
	g_gi.BindGetKey(L"+moveright", theInputType, kRight);
	
	// check bind keys to see if they are pressed individually or simultaneously
	//g_gi.INPGetKey(const tCHAR *str, INP_TYPE_KEYBOARD, &keycode);

}
