#include "engine_local.h"

//This is for loading the game modules

static HINSTANCE g_modules[MODULE_MAX];

/////////////////////////////////////
// Name:	EngineModDestroy
// Purpose:	destroy the given module
//			(game.dll...etc.)
// Output:	module terminated
// Return:	none
/////////////////////////////////////
void EngineModDestroy(eGameModule mod)
{
	if(g_modules[mod])
	{ FreeLibrary(g_modules[mod]); g_modules[mod] = 0; }
}

/////////////////////////////////////
// Name:	EngineModLoad
// Purpose:	load the given module
// Output:	module loaded
// Return:	export depending on mod
/////////////////////////////////////
void *EngineModLoad(eGameModule mod, void *param)
{
	wstring file;
	tCHAR path[MAXCHARBUFF];

	void	*(F_API *APIFunc) (void *);

#ifdef _DEBUG
#define MOD_SUB (L"_d")
#elif PRERELEASE
#define MOD_SUB (L"_r")
#else
#define MOD_SUB (L"")
#endif

	switch(mod)
	{
	case MODULE_GAME:
		file = L"game";
		break;
	}

	file += MOD_SUB;
	file += L".dll";

	//Get the path
	if(!FSGetPath(file.c_str(), path, MAXCHARBUFF))
	{ LogMsg(LOG_FILE | LOG_ASSERT, file.c_str(), L"Module File does not exist"); return 0; }

	//now load the module
	g_modules[mod] = LoadLibraryW(path);

	if(!g_modules[mod])
	{ LogMsg(LOG_FILE | LOG_ASSERT, file.c_str(), L"Unable to Load Module"); return 0; }

	//Get the API Entry Function
	*((u32*)&APIFunc) = (u32)GetProcAddress(g_modules[mod], "GetAPI");

	if(!APIFunc)
	{
		LogMsg(LOG_FILE | LOG_ASSERT, file.c_str(), L"Unable to Retrieve API Function");
		EngineModDestroy(mod);
		return 0;
	}

	//call the function and return whatever junk
	return APIFunc(param);
}