#include "engine_local.h"

#include "bind.h"

#include "command.h"

/////////////////////////////////////
// Name:	EngineUpdate
// Purpose:	all update calls are here
// Output:	stuff
// Return:	FALSE if something went
//			wrong
/////////////////////////////////////
s32 EngineUpdate()
{
	//update graphics
	g_GFX->GFXUpdate(EngineGetTick());

	//update input
	EngineInputUpdate();

	//update game
	g_ge->UpdateFrame();

	//execute all posted commands
	CmdExec();

	// Update the sound system
	EngineSoundUpdate();

	//clear out particles that are dead
	g_GFX->ParSysClear();

	return TRUE;
}