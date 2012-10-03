#include "engine_local.h"

#include "timer.h"

#include <direct.h>
#include <shellapi.h>

//interface modules
GFX_INSTANCE	*g_GFX = 0;
INPUT_INSTANCE	*g_INP = 0;
AUDIO_INSTANCE  *g_SFX = 0;

//engine data globals
tWinInfo		 g_winInfo={0};
u8				 g_bGFXRdy = FALSE;		//stupid WM_PAINT
										//this global is only used by engine_cfg.cpp

f32				 g_tick, g_lastTick;

#define			 FPS_COUNTER		1.0f	//set avgFPS per second

u32				 g_FPS, g_avgFPS=0, g_cFPS=0;
f32				 g_FPSctr=0; //FPS counter

wstring			 g_saveFile;			//the filepath we want to save to
u8				 g_autoSave=FALSE;

eTextureQuality  g_txtQuality=TXTQUALITY_HIGHEST; //the texture quality

FLAGS			 g_flags;				//engine flags

//module exports
GameExport		*g_ge;

////////////////////////////////////////////////////
hFMAIN			 g_fmain=0;

/////////////////////////////////////
// Name:	EngineCreateFSMain
// Purpose:	Initialize the Main
//			File Sys. interface.
//			call this once in init.
//			(after loading the 
//			gfx/sfx modules)
// Output:	stuff
// Return:	none
/////////////////////////////////////
void EngineCreateFSMain()
{
	g_fmain = FSMainCreate();

	FSMainSet(g_fmain);

	//make sure the right modules are loaded,
	//otherwise, don't even bother adding the dir
	if(g_GFX) g_GFX->GFXSetFS(g_fmain);
	if(g_SFX) g_SFX->SFXSetFS(g_fmain);
}

/////////////////////////////////////
// Name:	EngineDestroyFSMain
// Purpose:	destroy the Main
//			File Sys. interface.
//			call this in exit
// Output:	stuff
// Return:	none
/////////////////////////////////////
void EngineDestroyFSMain()
{
	if(g_fmain)
	{
		FSMainSet(0);
		FSMainDestroy(g_fmain);

		if(g_GFX) g_GFX->GFXSetFS(0);
		if(g_SFX) g_SFX->SFXSetFS(0);
	}
}

void PakSearchCB(tCHAR *filepath, void *userData)
{
	FSMainAddDir(filepath, FS_DIR_ARCHIVE);
}

//adding a directory and pak files inside
void EngineAddDir(const tCHAR *dir, u8 bReadOnly)
{
	//add the directory
	FSMainAddDir(dir, bReadOnly ? FS_DIR_READONLY : 0);

	//add any pak files inside
	ParserFindFiles((tCHAR*)dir, L"*.pak", FALSE, PakSearchCB, 0);
}

////////////////////////////////////////////////////

/////////////////////////////////////
// Name:	EngineSetSave
// Purpose:	set the current save file
// Output:	filename set
// Return:	none
/////////////////////////////////////
void EngineSetSave(const tCHAR *filename, u8 bAutoSave)
{
	g_saveFile = filename;
	g_autoSave = bAutoSave;
}

/////////////////////////////////////
// Name:	EngineGetTick
// Purpose:	get the elapse time
//			of current frame
// Output:	none
// Return:	tick in millisecond
/////////////////////////////////////
f32 EngineGetTick() { return g_tick; }

/////////////////////////////////////
// Name:	EngineGetFPS
// Purpose:	get the FPS
// Output:	none
// Return:	FPS
/////////////////////////////////////
u32 EngineGetFPS() { return g_FPS; }
u32 EngineGetAvgFPS() { if(g_cFPS==0) return 0; return g_avgFPS/g_cFPS; }

list<wstring> g_menuReq; //menu requests

//Engine flag manipulation

void EngineFlagSet(u32 bit, u8 bSet)
{
	if(bSet)
		SETFLAG(g_flags, bit);
	else
		CLEARFLAG(g_flags, bit);
}

u8 EngineFlagCheck(u32 bits)
{
	return TESTFLAGS(g_flags, bits);
}

s32 EngineVideoPlay(const tCHAR *filename)
{
	EngineInputClear();
	return g_GFX->GFXVideoPlay(WinGetHandle(), filename);
}

/////////////////////////////////////
// Name:	EngineSetTxtQuality
// Purpose:	set the texture quality
//			this only applies when
//			the game is launched,
//			however can be used to
//			save the new setting
// Output:	texture size limit set
// Return:	TRUE if success
/////////////////////////////////////
s32 EngineSetTxtQuality(eTextureQuality level)
{
	g_txtQuality = level;

	//set texture quality
	switch(g_txtQuality)
	{
	case TXTQUALITY_LOWEST:
		g_GFX->GFXSetTextureLimit(64, 64);
		break;
	case TXTQUALITY_LOW:
		g_GFX->GFXSetTextureLimit(64, 128);
		break;
	case TXTQUALITY_MEDIUM:
		g_GFX->GFXSetTextureLimit(64, 256);
		break;
	case TXTQUALITY_HIGH:
		g_GFX->GFXSetTextureLimit(64, 512);
		break;
	case TXTQUALITY_HIGHEST:
		g_GFX->GFXSetTextureLimit(-1, -1); //unlimited
		break;
	}

	return TRUE;
}

/////////////////////////////////////
// Name:	EngineGetTxtQuality
// Purpose:	get the quality texture
// Output:	none
// Return:	the quality
/////////////////////////////////////
eTextureQuality EngineGetTxtQuality()
{
	return g_txtQuality;
}

TheMain()
{
	if (FindWindowW (ENGINE_TITLE, ENGINE_TITLE) != NULL)	// Check whether game is already running
		return 0;	// If so, terminate now

	_chdir("..\\");

	//////////////////////////////////
	//initialize game
	tCHAR cmdLine[512]; mbstowcs(cmdLine, szCmdLine, 512);
	if(!EngineInit(hInstance, cmdLine))
		goto DEATH;

	while(WinGetMessage() != RETCODE_BREAK)
	{
		if(WinIsOpen())
		{
			TimerFPSBegin();

			g_tick = TimerGetElapsed();

			// In order to keep passing a reasonably stable value into the simulator's Advance
			// method, we'll try to stop occasional variations in the elapsed time from occurring.
			// We'll do this by preventing the elapsed time from being more than 20% greater or
			// less than the previous elapsed time.
			if (g_lastTick != 0)
			{
				if (g_tick > g_lastTick * 1.2f) g_tick = g_lastTick * 1.2f;
				if (g_tick < g_lastTick * 0.8f) g_tick = g_lastTick * 0.8f;
			}
			// To avoid particularly large spikes, we'll stop the elapsed time from exceeding
			// 1/45th of a second. Values larger than this can lead to an unstable simulation.
			if (g_tick > 1.0f / 45.0f) g_tick = 1.0f / 45.0f;
			// Store the elapsed time so that we can use it the next time around
			g_lastTick = g_tick;

			//update everything
			if(!EngineUpdate())
				break;

			//render everything
			if(!EngineRender())
				break;

			g_FPS = TimerFPSEnd();

			g_FPSctr += g_tick;
			if(g_FPSctr >= FPS_COUNTER && g_FPS > 5)
			{ g_FPSctr = 0; g_cFPS++; g_avgFPS += g_FPS; }
		}

		Sleep(1);
	}

DEATH:

	//Destroy Game!
	EngineDestroy();

	return 0;
}

//
// Windows Proc.
//
LRESULT CALLBACK WindowsProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    switch (message)
    {
	case WM_ACTIVATEAPP:
		return 0;

    case WM_CREATE:
		return 0;

	case WM_MOVE:
		//if(g_GFX->GFXVideoIsPlaying())
		//	break;
		if(g_bGFXRdy)
		{
			iRect r;
			// Our window position has changed, so
            // get the client (drawing) rectangle.
            GetClientRect(hwnd, (LPRECT)&r);
            // Convert the coordinates from client relative
            // to screen
            ClientToScreen(hwnd, ( LPPOINT )&r);
            ClientToScreen(hwnd, ( LPPOINT )&r + 1 );

			g_GFX->GFXUpdateClientRect(&r);
		}
        break;

	case WM_PAINT:
		//display game
		if(g_bGFXRdy)
			EngineRender();
		break;
    
    case WM_CLOSE:
		WinQuit();
        return 0;
    }

    return(DefWindowProc(hwnd, message, wparam, lparam));
}