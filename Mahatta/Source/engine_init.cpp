#include "engine_local.h"

#include "timer.h"

#include "command.h"
#include "value.h"
#include "bind.h"
#include "world.h"
#include "entity.h"

#include <direct.h>
#include <shellapi.h>
#include <time.h>

#ifdef _DEBUG
#define GFX_DLL "gfx_d3d_d.dll"
#define INP_DLL "input_dx8_d.dll"
#define SFX_DLL "Audio_d.dll"
#elif PRERELEASE
#define GFX_DLL "gfx_d3d_r.dll"
#define INP_DLL "input_dx8_r.dll"
#define SFX_DLL "Audio_r.dll"
#else
#define GFX_DLL "gfx_d3d.dll"
#define INP_DLL "input_dx8.dll"
#define SFX_DLL "Audio.dll"
#endif

//All game initializations go here

extern HRESULT GetDXVersion( DWORD* pdwDirectXVersion, TCHAR* strDirectXVersion, int cchDirectXVersion );


//parser callback for commandline
void CmdLineCallback(const tCHAR *cmdline, void *userData)
{
	tCHAR *pBuff=(tCHAR*)cmdline;
	tCHAR cmdStr[MAXCHARBUFF], paramStr[MAXCHARBUFF];
	tCHAR curdir[_MAX_PATH];

	if(cmdline)
	{
		ParserReadWordBuff(&pBuff, cmdStr, MAXCHARBUFF, ' ');
		ParserReadWordBuff(&pBuff, paramStr, MAXCHARBUFF, ' ');

		if(wcsicmp(cmdStr, L"set") == 0)
		{
			if(wcsicmp(paramStr, L"fs_game") == 0)
			{
				tCHAR dir[MAXCHARBUFF];
				ParserReadWordBuff(&pBuff, dir, MAXCHARBUFF, ' ');

				EngineAddDir(dir, FALSE);
			}
		}
		else if(wcsicmp(cmdStr, L"devmap") == 0)
		{
			wstring mapPath = L"Maps\\";
			mapPath += paramStr;
			mapPath += L".bsp";
			WorldPostMap(mapPath.c_str());
		}
		else if(wcsicmp(cmdStr, L"dir") == 0)
		{
			 //_wgetcwd(curdir, _MAX_PATH);
			 //wcscat(curdir, L"\\..");
			//_wchdir(curdir);
		}
	}
}

/////////////////////////////////////
// Name:	EngineInit
// Purpose:	initialize interfaces and
//			modules
// Output:	game engine initialized
// Return:	TRUE if success
/////////////////////////////////////
s32 EngineInit(HMAININST hInstance, const tCHAR *cmdline)
{
	//first check to see if we have a valid DX version
	DWORD  dxVer;
	GetDXVersion(&dxVer, 0, 0);

	if(dxVer < 0x00090002)
	{ 
		MESSAGE_BOX(L"Requires DirectX 9.0b or above to run.  Please download and install the latest Direct X from Microsoft", ENGINE_TITLE); 
		ShellExecute(WinGetHandle(), "open", "explorer", "http://www.microsoft.com/directx", 0, SW_MINIMIZE);
		return FALSE; 
	}

	//initialization
	srand((u32)time(0));

	TimerInit();

	//////////////////////////////////
	//create interface modules
	g_GFX = GFXCreateInstance(GFX_DLL); if(!g_GFX) { LogMsg(LOG_FILE | LOG_ASSERT, L"EngineInit", L"Unable to load GFX module"); return FALSE; }

	g_INP = InputCreateInstance(INP_DLL); if(!g_INP) { LogMsg(LOG_FILE | LOG_ASSERT, L"EngineInit", L"Unable to load Input module"); return FALSE; }

	g_SFX = AudioCreateInstance(SFX_DLL); if(!g_SFX) { LogMsg(LOG_FILE | LOG_ASSERT, L"EngineInit", L"Unable to load Audio module"); return FALSE; }

	//////////////////////////////////
	//Initialize main file system interface
	EngineCreateFSMain();

	//////////////////////////////////
	//Add the main folder to the File System
	EngineAddDir(ENGINE_MAIN, FALSE);

	//////////////////////////////////
	//Initialize pools

	//////////////////////////////////
	//Initialize basic commands
	CmdInit();
	ValueCmdInit();
	BindCmdInit();
	WorldCmdInit();
	EngineCmdInit();

	//////////////////////////////////
	//parse the command line
	ParserReadCommandLine(cmdline, '+', CmdLineCallback, 0);

	//////////////////////////////////
	//load up game engine settings
	//(include input)
	EngineCfgLoad(hInstance);

	//////////////////////////////////
	//load autoexec.cfg
	CmdExecFile(ENGINE_AUTOEXEC);

	//we don't want cursor to display
	WinShowCursor(FALSE);

	//////////////////////////////////
	//Load game module (game.dll)
	ModuleGameInit();

	return TRUE;
}