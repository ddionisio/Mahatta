#ifndef _engine_common_h
#define _engine_common_h

//types, definitions that are common to game engine/server, 
//but not included by modules (game.dll)

#include "Game\game_share.h"
#include "Game\game.h"

#include "STD\memTool.h"
#include "STD\log.h"
#include "STD\filesys.h"
#include "STD\parser.h"
#include "STD\cfg.h"

#include "winTool.h"

#include "GFX\gfxdyn.h" //include all graphics interface and math

#include "INP\inputdyn.h"

#include "AUDIO\audio_dyn.h"

////////////////////////////////////////////////
// Defines

////////////////////////////////////////////////
// Enumerates

//Game modules (dlls)
typedef enum { 
	MODULE_GAME,

	MODULE_MAX
} eGameModule;

////////////////////////////////////////////////////////////////
// Common Globals
extern GFX_INSTANCE		*g_GFX;
extern INPUT_INSTANCE	*g_INP;
extern AUDIO_INSTANCE   *g_SFX;

extern tWinInfo			 g_winInfo;

extern wstring			 g_saveFile;
extern u8				 g_autoSave;

//module exports
extern GameExport		*g_ge;

#endif