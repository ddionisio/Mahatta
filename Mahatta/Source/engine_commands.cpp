#include "engine_local.h"

#include "command.h"
#include "value.h"
#include "bind.h"
#include "world.h"

//All engine commands

//execute a script file
void CMD_ShowBoundBox()
{
	if(EngineFlagCheck(ENGINE_FLAG_SHOWBBOX))
		EngineFlagSet(ENGINE_FLAG_SHOWBBOX, FALSE);
	else
		EngineFlagSet(ENGINE_FLAG_SHOWBBOX, TRUE);
}

/////////////////////////////////////
// Name:	EngineCmdInit
// Purpose:	initialize all engine 
//			commands
// Output:	commands added
// Return:	none
/////////////////////////////////////
void EngineCmdInit()
{
	CmdAdd(L"show_bbox", CMD_ShowBoundBox);
}