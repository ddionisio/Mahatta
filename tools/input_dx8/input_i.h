#ifndef _input_i_h
#define _input_i_h

#pragma warning ( disable : 4311 )
#pragma warning ( disable : 4312 )

#define DIRECTINPUT_VERSION 0x0800

#include "common.h"

#include "INP\input.h"

#include "STD\memTool.h"
#include "STD\pager.h"
#include "STD\log.h"

#include <dinput.h>

/********************************************************************
*																	*
*						Internal Globals							*
*																	*
********************************************************************/
extern LPDIRECTINPUT8		  g_pDInput;

extern u32			  g_coopFlag;

/********************************************************************
*																	*
*					Types/Structures/Interfaces						*
*																	*
********************************************************************/
typedef vector< DIDEVICEINSTANCE > JoystickEnumArray;
typedef list< hJOYSTICK >		   JoystickList;

typedef struct _inputJoystick {
	u32						enumInd;	//index in enumeration array
	LPDIRECTINPUTDEVICE8	device;		//the device
	DIJOYSTATE2				state;		//state when updated

	s32						min,max;	//min/max range

	u8						arrowStates[eGamePadNum];	//used by gamepads (up/released/down)
	u8						btnStates[MAXJOYSTICKBTN];	//btn states (up/released/down)

	u32						ref;		//number of reference
} inputJoystick;

/********************************************************************
*																	*
*						 Internal Functions							*
*																	*
********************************************************************/

 void DInputError(HRESULT hr, const tCHAR *header);

 s32 INPJoystickReloadAll(void *hMain);
 void INPJoystickDestroyAll();

#endif