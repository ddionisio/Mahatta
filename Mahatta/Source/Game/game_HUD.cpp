#include "game_local.h"
#include "game_share.h"


#define LOGO_REL_SIZE	0.15f	//96/640	

hTXT g_logo;	//Xibalba Studios logo
hCFG g_cfgFile;
struct _LocalizedHudText
{
	tCHAR HELP[MAXCHARBUFF];
	tCHAR MOVE_FORWARD[MAXCHARBUFF];
	tCHAR MOVE_BACKWARD[MAXCHARBUFF];
	tCHAR MOVE_LEFT[MAXCHARBUFF];
	tCHAR MOVE_RIGHT[MAXCHARBUFF];
	tCHAR HOLD[MAXCHARBUFF];
	tCHAR TOGGLE[MAXCHARBUFF];
	tCHAR LOOK_AROUND[MAXCHARBUFF];
	tCHAR CYCLE[MAXCHARBUFF];
	tCHAR MAIN_MENU[MAXCHARBUFF];
	tCHAR HELPNOTSET[MAXCHARBUFF];
	tCHAR HIDEINFO[MAXCHARBUFF];
}LocalizedHudText;

void HUDInit()
{
	tCHAR buff[MAXCHARBUFF];
	
	if(g_gi.ValueGetStr(L"logo", buff, MAXCHARBUFF))
		g_logo = g_gi.TextureLoad(buff, 0,0);

	// LOCALIZATION STUFF
	g_cfgFile = g_gi.CfgFileLoad(L"language.ini");

	if(g_cfgFile)
	{
		//memset (&LocalizedHudText, 0, sizeof (struct LocalizedHudText));
		g_cfgFile->GetItemStr(L"help", L"HELP", (tCHAR *)&LocalizedHudText.HELP, MAXCHARBUFF);
		g_cfgFile->GetItemStr(L"help", L"MOVE_FORWARD", (tCHAR *)&LocalizedHudText.MOVE_FORWARD, MAXCHARBUFF);
		g_cfgFile->GetItemStr(L"help", L"MOVE_BACKWARD", (tCHAR *)&LocalizedHudText.MOVE_BACKWARD, MAXCHARBUFF);
		g_cfgFile->GetItemStr(L"help", L"MOVE_LEFT", (tCHAR *)&LocalizedHudText.MOVE_LEFT, MAXCHARBUFF);
		g_cfgFile->GetItemStr(L"help", L"MOVE_RIGHT", (tCHAR *)&LocalizedHudText.MOVE_RIGHT, MAXCHARBUFF);
		g_cfgFile->GetItemStr(L"help", L"HOLD", (tCHAR *)&LocalizedHudText.HOLD, MAXCHARBUFF);
		g_cfgFile->GetItemStr(L"help", L"TOGGLE", (tCHAR *)&LocalizedHudText.TOGGLE, MAXCHARBUFF);
		g_cfgFile->GetItemStr(L"help", L"LOOK_AROUND", (tCHAR *)&LocalizedHudText.LOOK_AROUND, MAXCHARBUFF);
		g_cfgFile->GetItemStr(L"help", L"CYCLE", (tCHAR *)&LocalizedHudText.CYCLE, MAXCHARBUFF);
		g_cfgFile->GetItemStr(L"help", L"MAIN_MENU", (tCHAR *)&LocalizedHudText.MAIN_MENU, MAXCHARBUFF);
		g_cfgFile->GetItemStr(L"help", L"HELPNOTSET", (tCHAR *)&LocalizedHudText.HELPNOTSET, MAXCHARBUFF);
		g_cfgFile->GetItemStr(L"help", L"HIDEINFO", (tCHAR *)&LocalizedHudText.HIDEINFO, MAXCHARBUFF);
		g_cfgFile->Release();
	}
	

}

void HUDDestroy()
{
	if(g_logo)
	{ g_logo->Release(); g_logo = 0; }
}

void HUDRender()
{
	iPoint fntS; g_game.dbgFnt->GetStrSize(L"X", &fntS);
	u32 txtY = 0;

	eInputType inpT;
	u8		   key;
	tCHAR keyStr[260];

	if(TESTFLAGS(g_game.flag, GAME_HELP_TOGGLE))
	{
		if(g_gi.BindGetKey(L"help", &inpT, &key))
		{//INPGetName
			g_gi.INPGetName(inpT, key, keyStr, 260);
			//g_gi.FontPrintf(g_game.dbgFnt, 0,txtY, 0xffffffff, 0, L"Press %s to hide this info", keyStr);
			g_game.dbgFnt->Printf(0,txtY, 0xffffffff, 0, LocalizedHudText.HIDEINFO, keyStr);
			txtY += fntS.y;
		}

		if(g_gi.BindGetKey(L"+forward", &inpT, &key))
		{
			g_gi.INPGetName(inpT, key, keyStr, 260);
			//g_gi.FontPrintf(g_game.dbgFnt, 0,txtY, 0xffffffff, 0, L"%s - Move Forward", keyStr);
			g_game.dbgFnt->Printf(0,txtY, 0xffffffff, 0, LocalizedHudText.MOVE_FORWARD, keyStr);
			txtY += fntS.y;
		}

		if(g_gi.BindGetKey(L"+back", &inpT, &key))
		{
			g_gi.INPGetName(inpT, key, keyStr, 260);
			//g_gi.FontPrintf(g_game.dbgFnt, 0,txtY, 0xffffffff, 0, L"%s - Move Backward", keyStr);
			g_game.dbgFnt->Printf(0,txtY, 0xffffffff, 0, LocalizedHudText.MOVE_BACKWARD, keyStr);
			txtY += fntS.y;
		}

		if(g_gi.BindGetKey(L"+moveleft", &inpT, &key))
		{
			g_gi.INPGetName(inpT, key, keyStr, 260);
			//g_gi.FontPrintf(g_game.dbgFnt, 0,txtY, 0xffffffff, 0, L"%s - Move Left", keyStr);
			g_game.dbgFnt->Printf(0,txtY, 0xffffffff, 0, LocalizedHudText.MOVE_LEFT, keyStr);
			txtY += fntS.y;
		}

		if(g_gi.BindGetKey(L"+moveright", &inpT, &key))
		{
			g_gi.INPGetName(inpT, key, keyStr, 260);
			//g_gi.FontPrintf(g_game.dbgFnt, 0,txtY, 0xffffffff, 0, L"%s - Move Right", keyStr);
			g_game.dbgFnt->Printf(0,txtY, 0xffffffff, 0, LocalizedHudText.MOVE_RIGHT, keyStr);
			txtY += fntS.y;
		}

		if(g_gi.BindGetKey(L"+run; -run", &inpT, &key))
		{
			g_gi.INPGetName(inpT, key, keyStr, 260);
			//g_gi.FontPrintf(g_game.dbgFnt, 0,txtY, 0xffffffff, 0, L"%s - Hold while moving forward to run.", keyStr);
			g_game.dbgFnt->Printf(0,txtY, 0xffffffff, 0, LocalizedHudText.HOLD, keyStr);
			txtY += fntS.y;
		}

		if(g_gi.BindGetKey(L"runtoggle", &inpT, &key))
		{
			g_gi.INPGetName(inpT, key, keyStr, 260);
			//g_gi.FontPrintf(g_game.dbgFnt, 0,txtY, 0xffffffff, 0, L"%s - Toggles run on/off.", keyStr);
			g_game.dbgFnt->Printf(0,txtY, 0xffffffff, 0, LocalizedHudText.TOGGLE, keyStr);
			txtY += fntS.y;
		}

		if(g_gi.BindGetKey(L"+lookat; -lookat", &inpT, &key))
		{
			g_gi.INPGetName(inpT, key, keyStr, 260);
			//g_gi.FontPrintf(g_game.dbgFnt, 0,txtY, 0xffffffff, 0, L"%s - Hold while moving the mouse to look around.", keyStr);
			g_game.dbgFnt->Printf(0,txtY, 0xffffffff, 0, LocalizedHudText.LOOK_AROUND, keyStr);
			txtY += fntS.y;
		}

		if(g_gi.BindGetKey(L"changescenefx", &inpT, &key))
		{
			g_gi.INPGetName(inpT, key, keyStr, 260);
			//g_gi.FontPrintf(g_game.dbgFnt, 0,txtY, 0xffffffff, 0, L"%s - Cycle through Scene FXs.", keyStr);
			g_game.dbgFnt->Printf(0,txtY, 0xffffffff, 0, LocalizedHudText.CYCLE, keyStr);
			txtY += fntS.y;
		}

		if(g_gi.BindGetKey(L"menu", &inpT, &key))
		{
			g_gi.INPGetName(inpT, key, keyStr, 260);
			//g_gi.FontPrintf(g_game.dbgFnt, 0,txtY, 0xffffffff, 0, L"%s - Opens Main Menu.", keyStr);
			g_game.dbgFnt->Printf(0,txtY, 0xffffffff, 0, LocalizedHudText.MAIN_MENU, keyStr);
			txtY += fntS.y;
		}
	}
	else
	{
		if(g_gi.BindGetKey(L"help", &inpT, &key))
		{
			g_gi.INPGetName(inpT, key, keyStr, 260);
			g_game.dbgFnt->Printf(0,txtY, 0xffffffff, 0, LocalizedHudText.HELP, keyStr);
		}
		else
			g_game.dbgFnt->Printf(0,txtY, 0xffffffff, 0, LocalizedHudText.HELPNOTSET);


		txtY += fntS.y;
	}

	//insert game specific stuff here (2D stuff)
	if(g_game.dbgFnt && TESTFLAGS(g_game.flag, GAME_SHOW_FPS))
	{
		g_game.dbgFnt->Printf(0,txtY, 0xffffffff, 0, L"FPS: %d avg: %d", g_gi.EngineGetFPS(), g_gi.EngineGetAvgFPS());

		txtY += fntS.y;

		g_game.dbgFnt->Printf(0,txtY, 0xffffffff, 0, L"Num Tris: %d", g_game.numTri);
	}

	/*if(g_logo)
	{//LOGO_REL_SIZE
		g_gi.TextureBegin();

		gfxMode mode; g_gi.GFXGetMode(&mode);
		f32 txtSize = LOGO_REL_SIZE*((f32)mode.width);
		g_logo->StretchBlt(((f32)mode.width)-txtSize, ((f32)mode.height)-txtSize, txtSize,txtSize, 0,0);

		g_gi.TextureEnd();
	}*/

	/*if(g_prueba)
	{//LOGO_REL_SIZE
		g_gi.TextureBegin();

		g_prueba->StretchBlt(0, 0, 150, 150, 0, 0);

		g_gi.TextureEnd();
	}*/
}