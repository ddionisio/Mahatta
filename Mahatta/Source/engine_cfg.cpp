#include "engine_local.h"

#include "value.h"
#include "bind.h"

#define SCN_W_DFLT		640
#define SCN_H_DFLT		480
#define SCN_FMT_DFLT	GFXFMT_X8R8G8B8

#define GFX_SEC				(L"graphics")

#define AUDIO_SEC			(L"audio")

#define AUDIO_SNDVOL_ITM	(L"sndvol")
#define AUDIO_MUSVOL_ITM	(L"musvol")

extern u8				 g_bGFXRdy;

/////////////////////////////////////
// Name:	EngineCfgLoad
// Purpose:	load the game engine 
//			settings
// Output:	display/sound initialized
// Return:	TRUE if success
/////////////////////////////////////
s32 EngineCfgLoad(HMAININST hinst)
{
	s32 ret = TRUE;

	//do we need to create a config file?
	u8 bSaveCFG = TRUE;
	hFILE fp = FSOpen(ENGINE_CFG_FILE, L"r");
	if(fp) { bSaveCFG = FALSE; FSClose(fp); }


	//Load up the system configuration
	hCFG cfg = CfgFileLoad(ENGINE_CFG_FILE);

	if(cfg)
	{
		eTextureQuality txtLvl;
		gfxMode mode={0};
		s32 iDat;

		////////////////////////////////////////////////////
		//load graphics and initialize windows

		//check for windowed mode
		iDat = cfg->GetItemInt(GFX_SEC, L"windowed");
		mode.bFullscrn = iDat == 1 ? FALSE : TRUE;

		//get screen resolution
		iDat = cfg->GetItemInt(GFX_SEC, L"width");
		mode.width = iDat > 0 ? iDat : SCN_W_DFLT;

		iDat = cfg->GetItemInt(GFX_SEC, L"height");
		mode.height = iDat > 0 ? iDat : SCN_H_DFLT;

		//get BPP
		iDat = cfg->GetItemInt(GFX_SEC, L"format");
		mode.fmt = iDat > 0 ? (GFXFORMAT)iDat : SCN_FMT_DFLT;

		//get resfresh rate
		iDat = cfg->GetItemInt(GFX_SEC, L"refresh");
		mode.refresh = iDat > 0 ? iDat : 0;

		//get vsync
		iDat = cfg->GetItemInt(GFX_SEC, L"vsync");
		mode.bVsync = iDat > 0 ? TRUE : FALSE;

		//get texture quality
		iDat = cfg->GetItemInt(GFX_SEC, L"txtQuality");
		txtLvl = iDat >= 0 ? (eTextureQuality)iDat : TXTQUALITY_HIGHEST;

		mode.depthNear = 1.0f;
		mode.depthFar = 10000.0f; // near-far thingy (prev. 2000)

		//initialize windows
		g_winInfo.style = mode.bFullscrn ? WIN_POPUP : WIN_OVERLAPPED | WIN_FIXED;
		g_winInfo.hinstance = hinst;
		g_winInfo.icon = IDI_ICON1;
		g_winInfo.winNameStr = ENGINE_TITLE;
		g_winInfo.menuStr = 0;
		g_winInfo.x = 0;
		g_winInfo.y = 0;
		g_winInfo.width =  (s32)mode.width;
		g_winInfo.height = (s32)mode.height;
		g_winInfo.bpp = 0;
		g_winInfo.refresh = 0;
		g_winInfo.callback = WindowsProc;
		g_winInfo.data = 0;
		
		WinInit(&g_winInfo);

		//adjust the client rect
		WinAdjustClientRect(WinGetHandle(), mode.width, mode.height);

		//initialize graphics
		if(g_GFX->GFXInit(WinGetHandle(), &mode) != TRUE)
		{ LogMsg(LOG_FILE | LOG_ASSERT, L"EngineCfgLoad", L"Failed to initialize GDI"); ret = FALSE; }

		//enable lighting
		g_GFX->LightEnableRender(TRUE);

		EngineSetTxtQuality(txtLvl);

		g_bGFXRdy = TRUE; //graphics is now ready

		////////////////////////////////////////////////////

		////////////////////////////////////////////////////
		//load sound
		EngineSoundSysLoad(L"sounds/Sounds.sfx");

		//get sound volume
		iDat = cfg->GetItemInt(AUDIO_SEC, AUDIO_SNDVOL_ITM);
		EngineSoundSetVolume(iDat == -1 ? 255 : (u8)iDat);

		//get music volume
		iDat = cfg->GetItemInt(AUDIO_SEC, AUDIO_MUSVOL_ITM);
		EngineMusicSetVolume(iDat == -1 ? 255 : (u8)iDat);

		////////////////////////////////////////////////////

		////////////////////////////////////////////////////
		//initialize/load input stuff
		EngineInputInit(cfg, hinst);

		////////////////////////////////////////////////////

		////////////////////////////////////////////////////
		//get the language, basically just add a directory
		//we make sure we cannot write on this directory

		tCHAR buff[MAXCHARBUFF];

		if(cfg->GetItemStr(L"language", L"lang", buff, MAXCHARBUFF))
		{
			wstring lang = L"Language\\";
			lang += buff;
			EngineAddDir(lang.c_str(), TRUE);
		}

		////////////////////////////////////////////////////

		cfg->Release();
	}

	if(bSaveCFG) EngineCfgSave();

	return ret;
}

/////////////////////////////////////
// Name:	EngineCfgSave
// Purpose:	save configuration
// Output:	game config saved
// Return:	TRUE if success
/////////////////////////////////////
s32 EngineCfgSave()
{
	//make sure the appropriate modules are loaded
	if(!g_GFX)
		return FALSE;

	hCFG cfg = CfgFileLoad(ENGINE_CFG_FILE);

	if(cfg)
	{
		gfxMode mode;

		//save graphic config
		g_GFX->GFXGetMode(&mode);

		cfg->AddItemInt(GFX_SEC, L"windowed", (s32)(!mode.bFullscrn));
		cfg->AddItemInt(GFX_SEC, L"width", (s32)mode.width);
		cfg->AddItemInt(GFX_SEC, L"height", (s32)mode.height);
		cfg->AddItemInt(GFX_SEC, L"format", (s32)mode.fmt);
		cfg->AddItemInt(GFX_SEC, L"refresh", (s32)mode.refresh);
		cfg->AddItemInt(GFX_SEC, L"vsync", (s32)mode.bVsync);
		cfg->AddItemInt(GFX_SEC, L"txtQuality", (s32)EngineGetTxtQuality());

		//save audio config
		cfg->AddItemInt(AUDIO_SEC, AUDIO_SNDVOL_ITM, (s32)EngineSoundGetVolume());
		cfg->AddItemInt(AUDIO_SEC, AUDIO_MUSVOL_ITM, (s32)EngineMusicGetVolume());

		//save input config
		EngineInputSave(cfg);

		cfg->FileSave();

		cfg->Release();
	}
	else
		return FALSE;

	return TRUE;
}

/////////////////////////////////////
// Name:	EngineAutoExecSave
// Purpose:	save value archives and
//			bindings to autoexec.cfg
// Output:	autoexec.cfg updated
// Return:	none
/////////////////////////////////////
void EngineAutoExecSave()
{
	hFILE fp = FSOpen(ENGINE_AUTOEXEC, L"wt");

	if(fp)
	{
		ValueSaveArchived(fp);

		FSPrintf(fp, L"unbindall\n");
		
		BindSave(fp);
		
		FSClose(fp);
	}
}