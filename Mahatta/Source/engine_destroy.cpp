#include "engine_local.h"

#include "bind.h"

#include "skybox.h"

/////////////////////////////////////
// Name:	EngineDestroy
// Purpose:	destroy game engine
// Output:	everything destroyed!
// Return:	none
/////////////////////////////////////
void EngineDestroy()
{
	WinShowCursor(TRUE);

	//////////////////////////////////
	//Save values and bindings
	EngineAutoExecSave();

	//////////////////////////////////
	//destroy bindings
	BindUnSetAllKey();

	//////////////////////////////////
	//destroy modules
	ModuleGameDestroy();

	//destroy some leftovers in case user devs. get clumsy
	SkyDestroy();

	//////////////////////////////////
	//destroy input
	if(g_INP)
	{ EngineInputDestroy(); InputFreeInstance(g_INP); g_INP = 0; }

	//////////////////////////////////
	//destroy audio
	if(g_SFX) EngineSoundSysDestroy();

	//////////////////////////////////
	//destroy graphics
	if(g_GFX) g_GFX->GFXDestroy();

	//////////////////////////////////
	//destroy File System main interface
	EngineDestroyFSMain();

	//destroy modules
	AudioFreeInstance(g_SFX); g_SFX = 0;
	GFXFreeInstance(g_GFX); g_GFX = 0;
}