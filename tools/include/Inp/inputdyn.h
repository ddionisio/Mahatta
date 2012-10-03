#ifndef _inputdyn_h
#define _inputdyn_h

#ifdef WIN32
  #include <windows.h>
#else
  #include <dlfcn.h>
  #include <string.h>
#endif

#include "INP\input.h"

typedef struct
{
    void           *module;

	s32		(F_API *INPInit) (void *hMain, void *hinst, u32 coopFlag);
	s32     (F_API *INPReInit) (void *hMain, void *hinst);
	void		(F_API *INPDestroy) ();

	s32		(F_API *INPKBInit) (void *hMain);
	void		(F_API *INPKBDestroy) ();
	s32 	(F_API *INPKBUpdate) ();
	u8		(F_API *INPKBGetBufferStates) (inpState buff[SAMPLE_BUFFER_SIZE]);
	u8			(F_API *INPKBAnyKeyPressed) ();
	u8			(F_API *INPKBAnyKeyReleased) ();
	u8			(F_API *INPKBIsPressed) (u8 kCode);
	u8			(F_API *INPKBIsReleased) (u8 kCode);
	void		(F_API *INPKBClear) ();
	tCHAR		(F_API *INPKBGetAscii) (u8 kCode);
	void		(F_API *INPKBGetStrReleased) (tCHAR *outbuffer, u32 outbuffersize);
	void		(F_API *INPKBGetKeyName) (u8 kCode, tCHAR *outbuffer, u32 size);
	const tCHAR * (F_API *INPKBToStr) (u8 kCode);
	u8			(F_API *INPKBToCode) (const tCHAR *str);
	u8			(F_API *INPKBExist) ();

	s32		(F_API *INPMouseInit) (void *hMain, u8 bExclusive, iPoint *pBound);
	void	(F_API *INPMouseSetBound) (iPoint *pBound);
	void		(F_API *INPMouseDestroy) ();
	s32		(F_API *INPMouseUpdate) (void *hMain);
	void		(F_API *INPMouseGetMove) (s32 *pX, s32 *pY, s32 *pZ);
	void		(F_API *INPMouseGetLoc) (iPoint *pLoc);
	u8			(F_API *INPMouseAnyBtnPressed) ();
	u8			(F_API *INPMouseAnyBtnReleased) ();
	u8			(F_API *INPMouseIsPressed) (u8 btn);
	u8			(F_API *INPMouseIsReleased) (u8 btn);
	void		(F_API *INPMouseClear) ();
	u8			(F_API *INPMouseExist) ();
	u8			(F_API *INPMouseIsExclusive) ();

	s32 	(F_API *INPJoystickEnum) ();
	u32			(F_API *INPJoystickGetNumEnum) ();
	void		(F_API *INPJoystickGetNameEnum) (u32 ind, tCHAR *strOut);
	s32			(F_API *INPJoystickGetIndEnumStr) (const tCHAR *name);
	s32			(F_API *INPJoystickGetIndEnum) (hJOYSTICK jStick);
	hJOYSTICK	(F_API *INPJoystickInit) (void *hMain, u32 enumInd, s32 min, s32 max);
	void		(F_API *INPJoystickDestroy) (hJOYSTICK jStick);
	void		(F_API *INPJoystickAddRef) (hJOYSTICK jStick);
	s32		(F_API *INPJoystickUpdate) (hJOYSTICK jStick);
	u8			(F_API *INPJoystickAnyBtnPressed) (hJOYSTICK jStick);
	u8			(F_API *INPJoystickAnyBtnReleased) (hJOYSTICK jStick);
	u8			(F_API *INPJoystickIsBtnPressed) (hJOYSTICK jStick, u8 btn);
	u8			(F_API *INPJoystickIsBtnReleased) (hJOYSTICK jStick, u8 btn);
	u8			(F_API *INPJoystickAnyArrowPressed) (hJOYSTICK jStick);
	u8			(F_API *INPJoystickAnyArrowReleased) (hJOYSTICK jStick);
	u8			(F_API *INPJoystickIsArrowPressed) (hJOYSTICK jStick, eGamePadArrows arrow);
	u8			(F_API *INPJoystickIsArrowReleased) (hJOYSTICK jStick, eGamePadArrows arrow);
	void		(F_API *INPJoystickClear) (hJOYSTICK jStick);
} INPUT_INSTANCE;

static INPUT_INSTANCE *InputCreateInstance(LPCSTR dllName)
{
    INPUT_INSTANCE *instance;

    instance = (INPUT_INSTANCE *)calloc(sizeof(INPUT_INSTANCE), 1);
    if (!instance)
    {
        return NULL;
    }

#ifdef WIN32
    instance->module = LoadLibrary(dllName);
#else
    instance->module = dlopen(dllName, RTLD_GLOBAL | RTLD_LAZY);
#endif
    if (!instance->module)
    {
        free(instance);
        return NULL;
    }

	MOD_GETPROC(INPInit, "INPInit");
	MOD_GETPROC(INPReInit, "INPReInit");
	MOD_GETPROC(INPDestroy, "INPDestroy");

	MOD_GETPROC(INPKBInit, "INPKBInit");
	MOD_GETPROC(INPKBDestroy, "INPKBDestroy");
	MOD_GETPROC(INPKBUpdate, "INPKBUpdate");
	MOD_GETPROC(INPKBAnyKeyPressed, "INPKBAnyKeyPressed");
	MOD_GETPROC(INPKBAnyKeyReleased, "INPKBAnyKeyReleased");
	MOD_GETPROC(INPKBIsPressed, "INPKBIsPressed");
	MOD_GETPROC(INPKBIsReleased, "INPKBIsReleased");
	MOD_GETPROC(INPKBClear, "INPKBClear");
	MOD_GETPROC(INPKBGetAscii, "INPKBGetAscii");
	MOD_GETPROC(INPKBGetBufferStates, "INPKBGetBufferStates");
	MOD_GETPROC(INPKBGetStrReleased, "INPKBGetStrReleased");
	MOD_GETPROC(INPKBGetKeyName, "INPKBGetKeyName");
	MOD_GETPROC(INPKBToStr,		"INPKBToStr");
	MOD_GETPROC(INPKBToCode,	"INPKBToCode");
	MOD_GETPROC(INPKBExist, "INPKBExist");

	MOD_GETPROC(INPMouseInit, "INPMouseInit");
	MOD_GETPROC(INPMouseSetBound, "INPMouseSetBound");
	MOD_GETPROC(INPMouseDestroy, "INPMouseDestroy");
	MOD_GETPROC(INPMouseUpdate, "INPMouseUpdate");
	MOD_GETPROC(INPMouseGetMove, "INPMouseGetMove");
	MOD_GETPROC(INPMouseGetLoc, "INPMouseGetLoc");
	MOD_GETPROC(INPMouseAnyBtnPressed, "INPMouseAnyBtnPressed");
	MOD_GETPROC(INPMouseAnyBtnReleased, "INPMouseAnyBtnReleased");
	MOD_GETPROC(INPMouseIsPressed, "INPMouseIsPressed");
	MOD_GETPROC(INPMouseIsReleased, "INPMouseIsReleased");
	MOD_GETPROC(INPMouseClear, "INPMouseClear");
	MOD_GETPROC(INPMouseExist, "INPMouseExist");
	MOD_GETPROC(INPMouseIsExclusive, "INPMouseIsExclusive");

	MOD_GETPROC(INPJoystickEnum, "INPJoystickEnum");
	MOD_GETPROC(INPJoystickGetNumEnum, "INPJoystickGetNumEnum");
	MOD_GETPROC(INPJoystickGetNameEnum, "INPJoystickGetNameEnum");
	MOD_GETPROC(INPJoystickGetIndEnumStr, "INPJoystickGetIndEnumStr");
	MOD_GETPROC(INPJoystickGetIndEnum, "INPJoystickGetIndEnum");
	MOD_GETPROC(INPJoystickInit, "INPJoystickInit");
	MOD_GETPROC(INPJoystickDestroy, "INPJoystickDestroy");
	MOD_GETPROC(INPJoystickAddRef, "INPJoystickAddRef");
	MOD_GETPROC(INPJoystickUpdate, "INPJoystickUpdate");
	MOD_GETPROC(INPJoystickAnyBtnPressed, "INPJoystickAnyBtnPressed");
	MOD_GETPROC(INPJoystickAnyBtnReleased, "INPJoystickAnyBtnReleased");
	MOD_GETPROC(INPJoystickIsBtnPressed, "INPJoystickIsBtnPressed");
	MOD_GETPROC(INPJoystickIsBtnReleased, "INPJoystickIsBtnReleased");
	MOD_GETPROC(INPJoystickAnyArrowPressed, "INPJoystickAnyArrowPressed");
	MOD_GETPROC(INPJoystickAnyArrowReleased, "INPJoystickAnyArrowReleased");
	MOD_GETPROC(INPJoystickIsArrowPressed, "INPJoystickIsArrowPressed");
	MOD_GETPROC(INPJoystickIsArrowReleased, "INPJoystickIsArrowReleased");
	MOD_GETPROC(INPJoystickClear, "INPJoystickClear");

	return instance;
}

static void InputFreeInstance(INPUT_INSTANCE *instance)
{
    if (instance)
    {
        if (instance->module)
        {
#ifdef WIN32
            FreeLibrary((HMODULE)instance->module);
#else
            dlclose(instance->module);
#endif
        }
        free(instance);
    }
}

#endif