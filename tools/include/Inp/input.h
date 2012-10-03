#ifndef _input_h
#define _input_h

#include "INP\input_types.h"

//functions

//
// Main
//

/////////////////////////////////////
// Name:	INPInit
// Purpose:	initialize input
// Output:	input initialized
// Return:	success, failure o.w.
/////////////////////////////////////
DLL_API s32 F_API INPInit(void *hMain, void *hinst, u32 coopFlag);

/////////////////////////////////////
// Name:	INPReInit
// Purpose:	reinitialize input
// Output:	
// Return:	
/////////////////////////////////////
DLL_API s32 F_API INPReInit(void *hMain, void *hinst);

/////////////////////////////////////
// Name:	INPDestroy
// Purpose:	destroy input
// Output:	input destroyed
// Return:	none
/////////////////////////////////////
DLL_API void F_API INPDestroy();

//
// Keyboard
//

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API s32 F_API INPKBInit(void *hMain);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API void F_API INPKBDestroy();

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API s32 F_API INPKBUpdate();

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API u8 F_API INPKBGetBufferStates(inpState buff[SAMPLE_BUFFER_SIZE]);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API u8 F_API INPKBAnyKeyPressed();

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API u8 F_API INPKBAnyKeyReleased();

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API u8 F_API INPKBIsPressed(u8 kCode);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API u8 F_API INPKBIsReleased(u8 kCode);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API void F_API INPKBClear();

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API tCHAR F_API INPKBGetAscii(u8 kCode);

/////////////////////////////////////
// Name:	INPKBGetStrReleased
// Purpose:	grab all keys and
//			put them to ascii
// Output:	
// Return:	
/////////////////////////////////////
DLL_API void F_API INPKBGetStrReleased(tCHAR *outbuffer, u32 outbuffersize);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API void F_API INPKBGetKeyName(u8 kCode, tCHAR *outbuffer, u32 size);

DLL_API const tCHAR * F_API INPKBToStr(u8 kCode);
DLL_API u8 F_API INPKBToCode(const tCHAR *str);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API u8 F_API INPKBExist();

//
// Mouse
//

/////////////////////////////////////
// Name:	INPMouseInit
// Purpose:	initialize mouse
//			if bExclusive, this means
//			we don't get actual windows
//			mouse location.  Use it on
//			fullscreen only.
//			pBound is used for 
//			exclusive mode.
// Output:	mouse initialized
// Return:	success if so
/////////////////////////////////////
DLL_API s32 F_API INPMouseInit(void *hMain, u8 bExclusive, iPoint *pBound);

/////////////////////////////////////
// Name:	INPMouseSetBound
// Purpose:	set the boundary of mouse
// Output:	
// Return:	
/////////////////////////////////////
DLL_API void F_API INPMouseSetBound(iPoint *pBound);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API void F_API INPMouseDestroy();

/////////////////////////////////////
// Name:	INPMouseUpdate
// Purpose:	update mouse device, we
//			require hMain to get the
//			actual location of the mouse
//			(this is not used in
//			 exclusive mode)
// Output:	mouse updated
// Return:	
/////////////////////////////////////
DLL_API s32 F_API INPMouseUpdate(void *hMain);

/////////////////////////////////////
// Name:	INPMouseGetMove
// Purpose:	get the relative motion of
//			the mouse.
// Output:	pX,pY,pZ is set
// Return:	none
/////////////////////////////////////
DLL_API void F_API INPMouseGetMove(s32 *pX, s32 *pY, s32 *pZ);

/////////////////////////////////////
// Name:	INPMouseGetLoc
// Purpose:	get location of the mouse,
//			in exclusive mode, this is
//			the accumulation of relative
//			motion.  O.w., the actual
//			mouse location.
// Output:	pLoc set
// Return:	none
/////////////////////////////////////
DLL_API void F_API INPMouseGetLoc(iPoint *pLoc);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API u8 F_API INPMouseAnyBtnPressed();

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API u8 F_API INPMouseAnyBtnReleased();

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API u8 F_API INPMouseIsPressed(u8 btn);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API u8 F_API INPMouseIsReleased(u8 btn);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API void F_API INPMouseClear();

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API u8 F_API INPMouseExist();

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API u8 F_API INPMouseIsExclusive();

//
// Joystick
//

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API s32 F_API INPJoystickEnum();

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API u32 F_API INPJoystickGetNumEnum();

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API void F_API INPJoystickGetNameEnum(u32 ind, tCHAR *strOut);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API s32 F_API INPJoystickGetIndEnumStr(const tCHAR *name);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API s32 F_API INPJoystickGetIndEnum(hJOYSTICK jStick);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API hJOYSTICK F_API INPJoystickInit(void *hMain, u32 enumInd, s32 min, s32 max);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API void F_API INPJoystickDestroy(hJOYSTICK jStick);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API void F_API INPJoystickAddRef(hJOYSTICK jStick);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API s32 F_API INPJoystickUpdate(hJOYSTICK jStick);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API u8 F_API INPJoystickAnyBtnPressed(hJOYSTICK jStick);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API u8 F_API INPJoystickAnyBtnReleased(hJOYSTICK jStick);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API u8 F_API INPJoystickIsBtnPressed(hJOYSTICK jStick, u8 btn);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API u8 F_API INPJoystickIsBtnReleased(hJOYSTICK jStick, u8 btn);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API u8 F_API INPJoystickAnyArrowPressed(hJOYSTICK jStick);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API u8 F_API INPJoystickAnyArrowReleased(hJOYSTICK jStick);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API u8 F_API INPJoystickIsArrowPressed(hJOYSTICK jStick, eGamePadArrows arrow);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API u8 F_API INPJoystickIsArrowReleased(hJOYSTICK jStick, eGamePadArrows arrow);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API void F_API INPJoystickClear(hJOYSTICK jStick);

#endif