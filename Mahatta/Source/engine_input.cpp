#include "engine_local.h"

//#ifdef _DEBUG
#define INPUTCOOPLVL	(INP_NONEXCLUSIVE | INP_FOREGROUND)
//#else
//#define INPUTCOOPLVL	(INP_EXCLUSIVE | INP_FOREGROUND)
//#endif

#define JOYSTICK_MIN	-1000
#define JOYSTICK_MAX	1000

#define INPUT_CFG_SECTION (L"input")
#define INPUT_CFG_JOYSTICK (L"joystick")

s32		  g_joystickEnum = 0;
hJOYSTICK g_joystick = 0;

/////////////////////////////////////
// Name:	EngineInputInit
// Purpose:	initialize input system
//			load joystick from given
//			cfg.
// Output:	input initialized
// Return:	TRUE if success
/////////////////////////////////////
s32 EngineInputInit(hCFG cfg, HMAININST hInst)
{
	EngineInputDestroy();

	g_INP->INPInit(WinGetHandle(), hInst, INPUTCOOPLVL);
	
	g_INP->INPKBInit(WinGetHandle());

	gfxMode mode; g_GFX->GFXGetMode(&mode);
	iPoint mBound={mode.width, mode.height};
	g_INP->INPMouseInit(WinGetHandle(), mode.bFullscrn, &mBound);

	//////////////////////////////////////////
	//initialize joystick
	g_INP->INPJoystickEnum();

	tCHAR joystickName[MAXCHARBUFF];

	if(cfg->GetItemStr(INPUT_CFG_SECTION, INPUT_CFG_JOYSTICK, joystickName, MAXCHARBUFF))
	{
		g_joystickEnum = g_INP->INPJoystickGetIndEnumStr(joystickName);

		if(g_joystickEnum == -1)
			g_joystickEnum = 0;
	}

	if(g_INP->INPJoystickGetNumEnum() > 0)
		g_joystick = g_INP->INPJoystickInit(WinGetHandle(), g_joystickEnum, JOYSTICK_MIN, JOYSTICK_MAX);
	//////////////////////////////////////////

	return TRUE;
}

/////////////////////////////////////
// Name:	EngineInputDestroy
// Purpose:	destroy input system
// Output:	input destroyed
// Return:	none
/////////////////////////////////////
void EngineInputDestroy()
{
	g_INP->INPJoystickDestroy(g_joystick);

	g_INP->INPDestroy();
}

/////////////////////////////////////
// Name:	EngineInputIsPressed
// Purpose:	check to see if a button
//			is pressed from given device
// Output:	none
// Return:	TRUE if given key is pressed
/////////////////////////////////////
u8 EngineInputIsPressed(eInputType device, u8 key)
{
	switch(device)
	{
	case INP_TYPE_KEYBOARD:
		return g_INP->INPKBIsPressed(key);

	case INP_TYPE_MOUSE:
		return g_INP->INPMouseIsPressed(key);

	case INP_TYPE_JOYSTICK:
		if(g_joystick)
			return g_INP->INPJoystickIsBtnPressed(g_joystick, key);
		break;
	}

	return FALSE;
}

/////////////////////////////////////
// Name:	EngineInputIsReleased
// Purpose:	check to see if a button
//			is released from given device
// Output:	none
// Return:	TRUE if given key is released
/////////////////////////////////////
u8 EngineInputIsReleased(eInputType device, u8 key)
{
	switch(device)
	{
	case INP_TYPE_KEYBOARD:
		return g_INP->INPKBIsReleased(key);

	case INP_TYPE_MOUSE:
		return g_INP->INPMouseIsReleased(key);

	case INP_TYPE_JOYSTICK:
		if(g_joystick)
			return g_INP->INPJoystickIsBtnReleased(g_joystick, key);
		break;
	}

	return FALSE;
}

/////////////////////////////////////
// Name:	EngineInputAnyKeyPressed
// Purpose:	check to see if a button
//			is pressed from given device
// Output:	none
// Return:	TRUE if given key is pressed
/////////////////////////////////////
u8 EngineInputAnyKeyPressed(eInputType device)
{
	switch(device)
	{
	case INP_TYPE_KEYBOARD:
		return g_INP->INPKBAnyKeyPressed();

	case INP_TYPE_MOUSE:
		return g_INP->INPMouseAnyBtnPressed();

	case INP_TYPE_JOYSTICK:
		if(g_joystick)
			return g_INP->INPJoystickAnyBtnPressed(g_joystick);
		break;
	}

	return FALSE;
}

/////////////////////////////////////
// Name:	EngineInputAnyKeyReleased
// Purpose:	check to see if a button
//			is pressed from given device
// Output:	none
// Return:	TRUE if given key is pressed
/////////////////////////////////////
u8 EngineInputAnyKeyReleased(eInputType device)
{
	switch(device)
	{
	case INP_TYPE_KEYBOARD:
		return g_INP->INPKBAnyKeyReleased();

	case INP_TYPE_MOUSE:
		return g_INP->INPMouseAnyBtnReleased();

	case INP_TYPE_JOYSTICK:
		if(g_joystick)
			return g_INP->INPJoystickAnyBtnReleased(g_joystick);
		break;
	}

	return FALSE;
}

/////////////////////////////////////
// Name:	EngineInputJStickArrowReleased
// Purpose:	is given DPad dir released?
// Output:	none
// Return:	TRUE if so
/////////////////////////////////////
u8 EngineInputJStickArrowReleased(eGamePadArrows arrow)
{
	if(!g_joystick)
		return FALSE;

	return g_INP->INPJoystickIsArrowReleased(g_joystick, arrow);
}

/////////////////////////////////////
// Name:	EngineInputJStickArrowPressed
// Purpose:	is given DPad dir pressed?
// Output:	none
// Return:	TRUE if so
/////////////////////////////////////
u8 EngineInputJStickArrowPressed(eGamePadArrows arrow)
{
	if(!g_joystick)
		return FALSE;

	return g_INP->INPJoystickIsArrowPressed(g_joystick, arrow);
}

/////////////////////////////////////
// Name:	EngineInputJStickAnyArrowReleased
// Purpose:	check to see if any of
//			the joystick's dpad is released
// Output:	none
// Return:	TRUE if so
/////////////////////////////////////
u8 EngineInputJStickAnyArrowReleased()
{
	if(!g_joystick)
		return FALSE;

	return g_INP->INPJoystickAnyArrowReleased(g_joystick);
}

/////////////////////////////////////
// Name:	EngineInputJStickAnyArrowPressed
// Purpose:	check to see if any of
//			the joystick's dpad is pressed
// Output:	none
// Return:	TRUE if so
/////////////////////////////////////
u8 EngineInputJStickAnyArrowPressed()
{
	if(!g_joystick)
		return FALSE;

	return g_INP->INPJoystickAnyArrowPressed(g_joystick);
}

/////////////////////////////////////
// Name:	InputChangeJstick
// Purpose:	change the joystick device
// Output:	joystick changed
// Return:	success if success
/////////////////////////////////////
s32 EngineInputChangeJstick(s32 enumInd)
{
	if(enumInd != g_joystickEnum  && g_INP->INPJoystickGetNumEnum() > 0)
	{
		g_INP->INPJoystickDestroy(g_joystick);

		g_joystick = g_INP->INPJoystickInit(WinGetHandle(), enumInd, JOYSTICK_MIN, JOYSTICK_MAX);

		g_joystickEnum = enumInd;
	}

	return TRUE;
}

/////////////////////////////////////
// Name:	EngineInputGetJstickInd
// Purpose:	grab the enum. index of
//			joystick
// Output:	none
// Return:	the joystick enum. index,
//			-1 if no joystick.
/////////////////////////////////////
s32 EngineInputGetJstickInd()
{
	if(g_joystick)
		return g_joystickEnum;//INPXJoystickGetIndEnum(g_joystick);

	return -1;
}

/////////////////////////////////////
// Name:	EngineInputSave
// Purpose:	save the input config.
// Output:	cfg set
// Return:	success
/////////////////////////////////////
s32 EngineInputSave(hCFG cfg)
{
	//////////////////////////////////////////
	//save the joystick name
	if(g_joystickEnum != -1)
	{
		tCHAR joystickName[MAXCHARBUFF];

		g_INP->INPJoystickGetNameEnum(g_joystickEnum, joystickName);
		
		cfg->AddItemStr(INPUT_CFG_SECTION, INPUT_CFG_JOYSTICK, joystickName);
	}
	//////////////////////////////////////////

	return TRUE;
}

/////////////////////////////////////
// Name:	EngineInputGetKey
// Purpose:	get the keycode based on
//			given string.
// Output:	device and kCode set
// Return:	TRUE if kCode is found
//			and valid
/////////////////////////////////////
u8 EngineInputGetKey(const tCHAR *str, eInputType *deviceOut, u8 *kCodeOut)
{
	u8 i;
	tCHAR buff[MAXCHARBUFF];
	
	//try keyboard
	*kCodeOut = g_INP->INPKBToCode(str);
	if(*kCodeOut)
	{
		*deviceOut = INP_TYPE_KEYBOARD;
		return TRUE;
	}

	//try joystick
	for(i = 0; i < MAXJOYSTICKBTN; i++)
	{
		swprintf(buff, L"joy%d", (u32)i);

		if(wcsicmp(buff, str) == 0)
		{
			*kCodeOut = i;
			*deviceOut = INP_TYPE_JOYSTICK;
			return TRUE;
		}
	}

	//try mouse
	for(i = 0; i < MAXMOUSEBTN; i++)
	{
		swprintf(buff, L"mouse%d", (u32)i);

		if(wcsicmp(buff, str) == 0)
		{
			*kCodeOut = i;
			*deviceOut = INP_TYPE_MOUSE;
			return TRUE;
		}
	}

	//Invalid keycode string!

	return FALSE;
}

/////////////////////////////////////
// Name:	EngineInputGetStr
// Purpose:	get the string based on
//			given device and keycode.
// Output:	strOut set
// Return:	TRUE if kCode is valid
/////////////////////////////////////
u8 EngineInputGetStr(eInputType device, u8 key, tCHAR *strOut, u32 size)
{
	static tCHAR buff[MAXCHARBUFF];
	const tCHAR *keyStr;

	u8 ret = TRUE;

	switch(device)
	{
	case INP_TYPE_KEYBOARD:
		{
			keyStr = g_INP->INPKBToStr(key);

			//wcscpy(strOut, keyStr ? keyStr : L"/0");
		}
		break;

	case INP_TYPE_JOYSTICK:
		if(key < MAXJOYSTICKBTN)
		{ swprintf(buff, L"joy%d", (u32)key); keyStr = buff; }
		else
			ret = FALSE;
		break;

	case INP_TYPE_MOUSE:
		if(key < MAXMOUSEBTN)
		{ swprintf(buff, L"mouse%d", (u32)key); keyStr = buff; }
		else
			ret = FALSE;
		break;
	}

	u32 keyS = wcslen(keyStr);
		
	if(size >= keyS)
	{
		memcpy(strOut, keyStr, sizeof(tCHAR)*keyS);
		strOut[keyS] = 0;
	}
	else
	{
		memcpy(strOut, keyStr, sizeof(tCHAR)*size);
		strOut[size-1] = 0;
	}

	return ret;
}

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
u8 EngineInputGetName(eInputType device, u8 key, tCHAR *strOut, u32 size)
{
	static tCHAR buff[MAXCHARBUFF];

	u8 ret = TRUE;

	switch(device)
	{
	case INP_TYPE_KEYBOARD:
		g_INP->INPKBGetKeyName(key, strOut, size);
		return TRUE;

	case INP_TYPE_JOYSTICK:
		if(key < MAXJOYSTICKBTN)
			swprintf(buff, L"Joystick Button %d", (u32)(key)+1);
		else
			ret = FALSE;
		break;

	case INP_TYPE_MOUSE:
		if(key < MAXMOUSEBTN)
			swprintf(buff, L"Mouse Button %d", (u32)(key)+1);
		else
			ret = FALSE;
		break;
	}

	u32 keyS = wcslen(buff);
		
	if(size >= keyS)
	{
		memcpy(strOut, buff, sizeof(tCHAR)*keyS);
		strOut[keyS] = 0;
	}
	else
	{
		memcpy(strOut, buff, sizeof(tCHAR)*size);
		strOut[size-1] = 0;
	}

	return ret;
}

/////////////////////////////////////
// Name:	EngineInputUpdate
// Purpose:	update the input stuff
// Output:	input device updated
// Return:	FALSE if something went
//			wrong
/////////////////////////////////////
s32 EngineInputUpdate()
{
	g_INP->INPKBUpdate();
	g_INP->INPMouseUpdate(WinGetHandle());

	if(g_joystick)
		g_INP->INPJoystickUpdate(g_joystick);

	return TRUE;
}

/////////////////////////////////////
// Name:	EngineInputClear
// Purpose:	clear the input stuff
// Output:	input device updated
// Return:	none
/////////////////////////////////////
void EngineInputClear()
{
	g_INP->INPKBClear();
	g_INP->INPMouseClear();

	if(g_joystick)
		g_INP->INPJoystickClear(g_joystick);

}