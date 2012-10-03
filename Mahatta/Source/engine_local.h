#ifndef _engine_local_h
#define _engine_local_h

//Only the game engine includes this header

#include "engine_common.h"
#include "resource.h"

////////////////////////////////////////////////////////////////
// defines

#define ENGINE_TITLE		(L"Underworld Tournament")

#define ENGINE_MAIN			(L"Main")		   //the main folder of the game

#define ENGINE_CFG_FILE		(L"..\\system\\settings.ini") //default config file

#define ENGINE_AUTOEXEC		(L"autoexec.cfg")	//the auto-saved engine command executions.
												//used for storing saved values and bindings
////////////////////////////////////////////////////////////////
// callbacks/procs
LRESULT CALLBACK WindowsProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

//
// Main Game Engine Functions
//

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
void EngineCreateFSMain();

/////////////////////////////////////
// Name:	EngineDestroyFSMain
// Purpose:	destroy the Main
//			File Sys. interface.
//			call this in exit
// Output:	stuff
// Return:	none
/////////////////////////////////////
void EngineDestroyFSMain();

//adding a directory and pak files inside
void EngineAddDir(const tCHAR *dir, u8 bReadOnly);

/////////////////////////////////////
// Name:	EngineSetSave
// Purpose:	set the current save file
// Output:	filename set
// Return:	none
/////////////////////////////////////
void EngineSetSave(const tCHAR *filename, u8 bAutoSave);

/////////////////////////////////////
// Name:	EngineGetTick
// Purpose:	get the elapse time
//			of current frame
// Output:	none
// Return:	tick in millisecond
/////////////////////////////////////
f32 EngineGetTick();

/////////////////////////////////////
// Name:	EngineGetFPS
// Purpose:	get the FPS
// Output:	none
// Return:	FPS
/////////////////////////////////////
u32 EngineGetFPS();
u32 EngineGetAvgFPS();

//Engine flag manipulation

void EngineFlagSet(u32 bit, u8 bSet);
u8 EngineFlagCheck(u32 bits);

//play a video
s32 EngineVideoPlay(const tCHAR *filename);

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
s32 EngineSetTxtQuality(eTextureQuality level);

/////////////////////////////////////
// Name:	EngineGetTxtQuality
// Purpose:	get the quality texture
// Output:	none
// Return:	the quality
/////////////////////////////////////
eTextureQuality EngineGetTxtQuality();

/////////////////////////////////////
// Name:	EngineCmdInit (engine_commands.cpp)
// Purpose:	initialize all engine 
//			commands
// Output:	commands added
// Return:	none
/////////////////////////////////////
void EngineCmdInit();

/////////////////////////////////////
// Name:	EngineInit (engine_init.cpp)
// Purpose:	initialize interfaces and
//			modules
// Output:	game engine initialized
// Return:	TRUE if success
/////////////////////////////////////
s32 EngineInit(HMAININST hInstance, const tCHAR *cmdline);

/////////////////////////////////////
// Name:	EngineDestroy (engine_destroy.cpp)
// Purpose:	destroy game engine
// Output:	everything destroyed!
// Return:	none
/////////////////////////////////////
void EngineDestroy();

/////////////////////////////////////
// Name:	EngineUpdate (engine_update.cpp)
// Purpose:	all update calls are here
// Output:	stuff
// Return:	FALSE if something went
//			wrong
/////////////////////////////////////
s32 EngineUpdate();

/////////////////////////////////////
// Name:	EngineRender (engine_render.cpp)
// Purpose:	render everything
// Output:	stuff rendered
// Return:	FALSE if something went
//			wrong
/////////////////////////////////////
s32 EngineRender();

//
// Game Engine Settings/Configuration (engine_cfg.cpp)
//

/////////////////////////////////////
// Name:	EngineCfgLoad
// Purpose:	load the game engine 
//			settings
// Output:	display/sound initialized
// Return:	TRUE if success
/////////////////////////////////////
s32 EngineCfgLoad(HMAININST hinst);

/////////////////////////////////////
// Name:	EngineCfgSave
// Purpose:	save game config, if
//			configFile is NULL, then
//			use filepath from
//			EngineCfgLoad(...)
// Output:	game config saved
// Return:	TRUE if success
/////////////////////////////////////
s32 EngineCfgSave();

/////////////////////////////////////
// Name:	EngineAutoExecSave
// Purpose:	save value archives and
//			bindings to autoexec.cfg
// Output:	autoexec.cfg updated
// Return:	none
/////////////////////////////////////
void EngineAutoExecSave();

//
// Game Module Interface (engine_module.cpp)
//

/////////////////////////////////////
// Name:	EngineModLoad
// Purpose:	load the given module
// Output:	module loaded
// Return:	export depending on mod
/////////////////////////////////////
void *EngineModLoad(eGameModule mod, void *param);

/////////////////////////////////////
// Name:	EngineModDestroy
// Purpose:	destroy the given module
//			(game.dll...etc.)
// Output:	module terminated
// Return:	none
/////////////////////////////////////
void EngineModDestroy(eGameModule mod);

//
// game.dll Module Interface (module_game.cpp)
//

/////////////////////////////////////
// Name:	ModuleGameInit
// Purpose:	load up game.dll
// Output:	game.dll loaded and
//			initialized
// Return:	TRUE if success
/////////////////////////////////////
s32 ModuleGameInit();

/////////////////////////////////////
// Name:	ModuleGameDestroy
// Purpose:	destroy game module
// Output:	game.dll unloaded
// Return:	none
/////////////////////////////////////
void ModuleGameDestroy();

//
// ui.dll Module Interface (module_ui.cpp)
//

/////////////////////////////////////
// Name:	ModuleUIInit
// Purpose:	load up ui.dll
// Output:	ui.dll loaded and
//			initialized
// Return:	TRUE if success
/////////////////////////////////////
s32 ModuleUIInit();

/////////////////////////////////////
// Name:	ModuleUIDestroy
// Purpose:	destroy game module
// Output:	ui.dll unloaded
// Return:	none
/////////////////////////////////////
void ModuleUIDestroy();

//
// Game Engine Input Interface (engine_input.cpp)
//

/////////////////////////////////////
// Name:	EngineInputInit
// Purpose:	initialize input system
//			load joystick from given
//			cfg.
// Output:	input initialized
// Return:	TRUE if success
/////////////////////////////////////
s32 EngineInputInit(hCFG cfg, HMAININST hInst);

/////////////////////////////////////
// Name:	EngineInputDestroy
// Purpose:	destroy input system
// Output:	input destroyed
// Return:	none
/////////////////////////////////////
void EngineInputDestroy();

/////////////////////////////////////
// Name:	EngineInputIsPressed
// Purpose:	check to see if a button
//			is pressed from given device
// Output:	none
// Return:	TRUE if given key is pressed
/////////////////////////////////////
u8 EngineInputIsPressed(eInputType device, u8 key);

/////////////////////////////////////
// Name:	EngineInputIsReleased
// Purpose:	check to see if a button
//			is released from given device
// Output:	none
// Return:	TRUE if given key is released
/////////////////////////////////////
u8 EngineInputIsReleased(eInputType device, u8 key);

/////////////////////////////////////
// Name:	EngineInputAnyKeyPressed
// Purpose:	check to see if a button
//			is pressed from given device
// Output:	none
// Return:	TRUE if given key is pressed
/////////////////////////////////////
u8 EngineInputAnyKeyPressed(eInputType device);

/////////////////////////////////////
// Name:	EngineInputAnyKeyReleased
// Purpose:	check to see if a button
//			is pressed from given device
// Output:	none
// Return:	TRUE if given key is pressed
/////////////////////////////////////
u8 EngineInputAnyKeyReleased(eInputType device);

/////////////////////////////////////
// Name:	EngineInputJStickArrowReleased
// Purpose:	is given DPad dir released?
// Output:	none
// Return:	TRUE if so
/////////////////////////////////////
u8 EngineInputJStickArrowReleased(eGamePadArrows arrow);

/////////////////////////////////////
// Name:	EngineInputJStickArrowPressed
// Purpose:	is given DPad dir pressed?
// Output:	none
// Return:	TRUE if so
/////////////////////////////////////
u8 EngineInputJStickArrowPressed(eGamePadArrows arrow);

/////////////////////////////////////
// Name:	EngineInputJStickAnyArrowReleased
// Purpose:	check to see if any of
//			the joystick's dpad is released
// Output:	none
// Return:	TRUE if so
/////////////////////////////////////
u8 EngineInputJStickAnyArrowReleased();

/////////////////////////////////////
// Name:	EngineInputJStickAnyArrowPressed
// Purpose:	check to see if any of
//			the joystick's dpad is pressed
// Output:	none
// Return:	TRUE if so
/////////////////////////////////////
u8 EngineInputJStickAnyArrowPressed();

/////////////////////////////////////
// Name:	InputChangeJstick
// Purpose:	change the joystick device
// Output:	joystick changed
// Return:	success if success
/////////////////////////////////////
s32 EngineInputChangeJstick(s32 enumInd);

/////////////////////////////////////
// Name:	EngineInputGetJstickInd
// Purpose:	grab the enum. index of
//			joystick
// Output:	none
// Return:	the joystick enum. index,
//			-1 if no joystick.
/////////////////////////////////////
s32 EngineInputGetJstickInd();

/////////////////////////////////////
// Name:	EngineInputSave
// Purpose:	save the input config.
// Output:	cfg set
// Return:	success
/////////////////////////////////////
s32 EngineInputSave(hCFG cfg);

/////////////////////////////////////
// Name:	EngineInputGetKey
// Purpose:	get the keycode based on
//			given string.
// Output:	device and kCode set
// Return:	TRUE if kCode is found
//			and valid
/////////////////////////////////////
u8 EngineInputGetKey(const tCHAR *str, eInputType *deviceOut, u8 *kCodeOut);

/////////////////////////////////////
// Name:	EngineInputGetStr
// Purpose:	get the string based on
//			given device and keycode.
// Output:	strOut set
// Return:	TRUE if kCode is valid
/////////////////////////////////////
u8 EngineInputGetStr(eInputType device, u8 key, tCHAR *strOut, u32 size);

/////////////////////////////////////
// Name:	EngineInputGetName
// Purpose: gets the local name of
//			given key. The difference
//			between this and EngineInputGetStr
//			is that this will retrieve the
//			string based on country. So
//			don't use this to save to config.
// Output:	strOut set
// Return:	TRUE if key and device is valid
/////////////////////////////////////
u8 EngineInputGetName(eInputType device, u8 key, tCHAR *strOut, u32 size);

/////////////////////////////////////
// Name:	EngineInputUpdate
// Purpose:	update the input stuff
// Output:	input device updated
// Return:	FALSE if something went
//			wrong
/////////////////////////////////////
s32 EngineInputUpdate();

/////////////////////////////////////
// Name:	EngineInputClear
// Purpose:	clear the input stuff
// Output:	input device updated
// Return:	none
/////////////////////////////////////
void EngineInputClear();

//
// Engine Sound Interface (engine_sound.cpp)
//

/////////////////////////////////////
// Name:	EngineSoundSetVolume
// Purpose:	set the sound volume
// Output:	master sound volume set
// Return:	none
/////////////////////////////////////
void EngineSoundSetVolume(u8 vol);

/////////////////////////////////////
// Name:	EngineSoundGetVolume
// Purpose:	get the sound volume
// Output:	none
// Return:	the volume
/////////////////////////////////////
u8 EngineSoundGetVolume();

/////////////////////////////////////
// Name:	EngineSoundSysLoad
// Purpose:	load sound library
//			the file may contain several
//			sounds. This loads all the 
//			resources of sounds in the 
//			library.
// Output:	system loaded
// Return:	TRUE if success
/////////////////////////////////////
s32 EngineSoundSysLoad(const tCHAR *filename);

/////////////////////////////////////
// Name:	EngineSoundSysDestroy
// Purpose:	destroy the sound library,
//			this will also destroy all
//			instances of sounds
// Output:	everything destroyed!
// Return:	none
/////////////////////////////////////
void EngineSoundSysDestroy();

/////////////////////////////////////
// Name:	EngineSoundSysGetID
// Purpose:	get the index of a sound in
//			the library with given name
//			<name> { ... }
// Output:	sndSysIDOut is set
// Return:	FALSE if given name does not
//			exist.
/////////////////////////////////////
u8 EngineSoundSysGetID(const tCHAR *name, s32 *sndSysIDOut);

/////////////////////////////////////
// Name:	EngineSoundIsValid
// Purpose:	check to see if given sndID
//			is still valid, o.w. it must've
//			expired already.
// Output:	none
// Return:	TRUE if sndID is valid and
//			still kickin'
/////////////////////////////////////
u8 EngineSoundIsValid(s32 sndID);

/////////////////////////////////////
// Name:	EngineSoundPlay
// Purpose:	emits the given sndSysID (from
//			EngineSoundSysGetID).
// Output:	a channel is chosen for the sysID
// Return:	the sndID (instance of sndSysID)
/////////////////////////////////////
s32 EngineSoundPlay(s32 sndSysID, const Vec3D & pos);

/////////////////////////////////////
// Name:	EngineSoundSet
// Purpose:	update given sndID (from
//			EngineSoundPlay)
// Output:	given sndID updated
// Return:	TRUE if success.
/////////////////////////////////////
u8 EngineSoundSet(s32 sndID, const Vec3D & pos, const Vec3D & vel);

/////////////////////////////////////
// Name:	EngineSoundPause
// Purpose:	(un)pause given sndID
// Output:	snd paused
// Return:	TRUE if success
/////////////////////////////////////
u8 EngineSoundPause(s32 sndID, u8 bPause);

/////////////////////////////////////
// Name:	EngineSoundIsPause
// Purpose:	is given sound paused?
// Output:	none
// Return:	TRUE if snd is paused.
/////////////////////////////////////
u8 EngineSoundIsPause(s32 sndID);

/////////////////////////////////////
// Name:	EngineSoundStop
// Purpose:	stop given sndID, this will
//			invalidate sndID as well.
// Output:	sndID invalidated
// Return:	TRUE if success
/////////////////////////////////////
u8 EngineSoundStop(s32 sndID);

/////////////////////////////////////
// Name:	EngineSoundStopAll
// Purpose:	invalidate all sounds playing
// Output:	silence.
// Return:	TRUE if success
/////////////////////////////////////
u8 EngineSoundStopAll();

/////////////////////////////////////
// Name:	EngineSoundSetListener
// Purpose:	update listener's
//			3d sound position
// Output:	none
// Return:	TRUE if success
/////////////////////////////////////
u8 EngineSoundSetListener(const Vec3D &pos, const Vec3D &vel, const Vec3D &fwd, const Vec3D &top);

/////////////////////////////////////
// Name:	EngineSoundUpdate
// Purpose:	update the 3d sound
//			system
// Output:	none
// Return:	TRUE if success
/////////////////////////////////////
u8 EngineSoundUpdate();

//
// Music
//

/////////////////////////////////////
// Name:	EngineMusicGetVolume
// Purpose:	get the music volume
// Output:	none
// Return:	the volume
/////////////////////////////////////
u8 EngineMusicGetVolume();

/////////////////////////////////////
// Name:	EngineMusicSetVolume
// Purpose:	set the music volume
// Output:	none
// Return:	none
/////////////////////////////////////
void EngineMusicSetVolume(u8 vol);

/////////////////////////////////////
// Name:	EngineMusicPlay
// Purpose:	play a music from file
// Output:	music played
// Return:	TRUE if success
/////////////////////////////////////
u8 EngineMusicPlay(const tCHAR *filename);

/////////////////////////////////////
// Name:	EngineMusicStop
// Purpose:	stop current music played
// Output:	music is stopped & destroyed
// Return:	none
/////////////////////////////////////
void EngineMusicStop(f32 fadeOutTime);

//
// Loader Display stuff
//

/////////////////////////////////////
// Name:	EngineLoadDisplay
// Purpose:	display an image instantly
//			use this for load screens.
// Output:	image displayed when this is
//			called
// Return:	none
/////////////////////////////////////
void EngineLoadDisplay(hTXT txt, f32 x, f32 y, u8 bClear);

/////////////////////////////////////
// Name:	EngineLoadDisplayText
// Purpose:	display text at bottom instantly
//			use this for load screens.
// Output:	text displayed when this is
//			called
// Return:	none
/////////////////////////////////////
void EngineLoadDisplayText(hFNT fnt, const tCHAR *str, ...);

#endif