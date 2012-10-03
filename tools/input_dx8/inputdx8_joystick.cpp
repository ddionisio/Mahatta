#include "input_i.h"

JoystickEnumArray	g_joystickEnums;		//all the joystick enumeration
JoystickList			g_joysticks;			//all the joysticks

 BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance,
                                     VOID* pContext )
{
	DIDEVICEINSTANCE newEnum;

	memcpy(&newEnum, pdidInstance, sizeof(DIDEVICEINSTANCE));

	//just add it to the enumeration list
	g_joystickEnums.push_back(newEnum);

	return DIENUM_CONTINUE;
}

s32 F_API INPJoystickEnum()
{
	g_joystickEnums.clear();

	//get all the attached joysticks
	HRESULT hr = g_pDInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback,
		NULL, DIEDFL_ATTACHEDONLY );
	if(FAILED(hr))
	{ DInputError(hr, L"INPJoystickEnum"); return RETCODE_FAILURE; }

	return RETCODE_SUCCESS;
}

u32 F_API INPJoystickGetNumEnum()
{
	return (u32)g_joystickEnums.size();
}

void F_API INPJoystickGetNameEnum(u32 ind, tCHAR *strOut)
{
	if(INPJoystickGetNumEnum() > 0)
	{
		u32 count = strlen(g_joystickEnums[ind].tszProductName)+1;
		mbstowcs(strOut, g_joystickEnums[ind].tszProductName, count);
	}
	else
		strOut[0] = 0;
}

s32 F_API INPJoystickGetIndEnumStr(const tCHAR *name)
{
	tCHAR prodName[MAXCHARBUFF];

	for(u32 i = 0; i < g_joystickEnums.size(); i++)
	{
		INPJoystickGetNameEnum(i, prodName);

		if(wcscmp(prodName, name) == 0)
			return i;
	}

	return -1; //not found
}

s32 F_API INPJoystickGetIndEnum(hJOYSTICK jStick)
{
	if(jStick)
		return jStick->enumInd;
	return -1;
}

 hJOYSTICK _INPJoystickSearch(u32 enumInd)
{
	tCHAR prodName[MAXCHARBUFF], prodName2[MAXCHARBUFF];

	hJOYSTICK theJoy = 0;

	for(JoystickList::iterator i = g_joysticks.begin(); i != g_joysticks.end(); i++)
	{
		theJoy = *i;

		if(theJoy)
		{
			INPJoystickGetNameEnum(enumInd, prodName);
			INPJoystickGetNameEnum(theJoy->enumInd, prodName2);

			if(wcscmp(prodName, prodName2) == 0)
				break;
		}
	}

	return theJoy;
}

static void _JoystickDestroyDat(hJOYSTICK jStick)
{
	if(jStick->device)
	{
		jStick->device->Unacquire();
		jStick->device->Release();
	}
}

//
static void _JoystickDestroy(hJOYSTICK jStick)
{
	if(jStick)
	{
		_JoystickDestroyDat(jStick);
		MemFree(jStick);
	}
}

 BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi,
                                   VOID* pContext )
{
    hJOYSTICK jStick = (hJOYSTICK)pContext;

    static s32 nSliderCount = 0;  // Number of returned slider controls
    static s32 nPOVCount = 0;     // Number of returned POV controls

    // For axes that are returned, set the DIPROP_RANGE property for the
    // enumerated axis in order to scale min/max values.
    if( pdidoi->dwType & DIDFT_AXIS )
	{
		//set range of axis
		DIPROPRANGE diprg; 
        diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
        diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
        diprg.diph.dwHow        = DIPH_BYID; 
        diprg.diph.dwObj        = pdidoi->dwType; // Specify the enumerated axis
        diprg.lMin              = jStick->min;
        diprg.lMax              = jStick->max;
    
        // Set the range for the axis
        if(FAILED(jStick->device->SetProperty(DIPROP_RANGE, &diprg.diph)))
			return DIENUM_STOP;
	}

	return DIENUM_CONTINUE;
}

static s32 _INPJoystickInitDat(hJOYSTICK newStick, void *hMain, u32 enumInd, s32 min, s32 max)
{
	HRESULT hr;

	newStick->enumInd = enumInd;
		
	//create the device
	hr = g_pDInput->CreateDevice(g_joystickEnums[enumInd].guidInstance, &newStick->device, NULL);
	if(FAILED(hr))
	{ DInputError(hr, L"INPJoystickInit"); return RETCODE_FAILURE; }

	// A data format specifies which controls on a device we are interested in,
	// and how they should be reported. This tells DInput that we will be
	// passing a DIJOYSTATE2 structure to IDirectInputDevice::GetDeviceState().
	hr = newStick->device->SetDataFormat(&c_dfDIJoystick2);
	if(FAILED(hr))
	{ DInputError(hr, L"INPJoystickInit"); return RETCODE_FAILURE; }

	// Set the cooperative level to let DInput know how this device should
	// interact with the system and with other DInput applications.
	hr = newStick->device->SetCooperativeLevel((HWND)hMain, g_coopFlag);
	if(FAILED(hr))
	{ DInputError(hr, L"INPJoystickInit"); return RETCODE_FAILURE; }

	//set the min/max of all axis
	newStick->min = min;
	newStick->max = max;

	newStick->device->EnumObjects(EnumObjectsCallback, (VOID*)newStick, DIDFT_ALL);

	return RETCODE_SUCCESS;
}

hJOYSTICK F_API INPJoystickInit(void *hMain, u32 enumInd, s32 min, s32 max)
{
	//make sure there are joysticks...
	if(g_joystickEnums.size() == 0)
		return 0;

	//check to see if this joystick alread exists
	hJOYSTICK newStick = _INPJoystickSearch(enumInd);

	if(!newStick)
	{
		newStick = (hJOYSTICK)MemAlloc(sizeof(inputJoystick));

		if(!newStick) { LogMsg(LOG_FILE, L"INPJoystickInit", L"Unable to allocate new joystick!"); return 0; }

		if(_INPJoystickInitDat(newStick, hMain, enumInd, min, max) == RETCODE_FAILURE)
		{ _JoystickDestroy(newStick); return 0; }

		//insert to global list
		g_joysticks.push_back(newStick);
	}
	else
		INPJoystickAddRef(newStick);

	return newStick;
}

 void INPJoystickDestroyAll()
{
	for(JoystickList::iterator i = g_joysticks.begin(); i != g_joysticks.end(); i++)
	{
		_JoystickDestroy(*i);
	}

	g_joysticks.clear();
}

void F_API INPJoystickDestroy(hJOYSTICK jStick)
{
	if(jStick)
	{
		if(jStick->ref == 0)
		{
			//just remove the pointer of object from list
			g_joysticks.remove(jStick);

			_JoystickDestroy(jStick);
		}
		else
			jStick->ref--;
	}
}

void F_API INPJoystickAddRef(hJOYSTICK jStick)
{
	jStick->ref++;
}

s32 F_API INPJoystickUpdate(hJOYSTICK jStick)
{
	if(!jStick)
		return RETCODE_FAILURE;

	u32 i;

	//reset released btns.
	for(i = 0; i < eGamePadNum; i++)
	{
		if(jStick->arrowStates[i] == INPUT_RELEASED)
			jStick->arrowStates[i] = INPUT_UP;
	}

	for(i = 0; i < MAXJOYSTICKBTN; i++)
	{
		if(jStick->btnStates[i] == INPUT_RELEASED)
			jStick->btnStates[i] = INPUT_UP;
	}

	memset(&jStick->state, 0 , sizeof(jStick->state));
	
	HRESULT hr;

	hr = jStick->device->Poll();

	if(FAILED(hr))
	{
		// DInput is telling us that the input stream has been
        // interrupted. We aren't tracking any state between polls, so
        // we don't have any special reset that needs to be done. We
        // just re-acquire and try again.
        hr = jStick->device->Acquire();
        while( hr == DIERR_INPUTLOST ) 
            hr = jStick->device->Acquire();

        // hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
        // may occur when the app is minimized or in the process of 
        // switching, so just try again later 
        return RETCODE_SUCCESS; 
	}

	// Get the input's device state
    if(FAILED(hr = jStick->device->GetDeviceState(sizeof(DIJOYSTATE2), &jStick->state)))
	{ DInputError(hr, L"INPJoystickUpdate"); return RETCODE_FAILURE; }

	//check the arrow status

	//left/right
	if(jStick->state.lX == jStick->min)
		jStick->arrowStates[eGamePadLeft] = INPUT_DOWN;
	else if(jStick->state.lX == jStick->max)
		jStick->arrowStates[eGamePadRight] = INPUT_DOWN;
	else
	{
		if(jStick->arrowStates[eGamePadLeft] == INPUT_DOWN)
			jStick->arrowStates[eGamePadLeft] = INPUT_RELEASED;
		else if(jStick->arrowStates[eGamePadRight] == INPUT_DOWN)
			jStick->arrowStates[eGamePadRight] = INPUT_RELEASED;
	}

	//up/down
	if(jStick->state.lY == jStick->min)
		jStick->arrowStates[eGamePadUp] = INPUT_DOWN;
	else if(jStick->state.lY == jStick->max)
		jStick->arrowStates[eGamePadDown] = INPUT_DOWN;
	else
	{
		if(jStick->arrowStates[eGamePadUp] == INPUT_DOWN)
			jStick->arrowStates[eGamePadUp] = INPUT_RELEASED;
		else if(jStick->arrowStates[eGamePadDown] == INPUT_DOWN)
			jStick->arrowStates[eGamePadDown] = INPUT_RELEASED;
	}

	//check the buttons
	for(i = 0; i < MAXJOYSTICKBTN; i++ )
    {
        if(jStick->state.rgbButtons[i] & INPUT_DOWN)
			jStick->btnStates[i] = INPUT_DOWN;
		else if(jStick->btnStates[i] == INPUT_DOWN)
			jStick->btnStates[i] = INPUT_RELEASED;
	}

	return RETCODE_SUCCESS;
}

u8 F_API INPJoystickAnyBtnPressed(hJOYSTICK jStick)
{
	if(!jStick)
		return FALSE;

	for( u32 i = 0; i < MAXJOYSTICKBTN; i++ )
	{
		if(jStick->btnStates[i] == INPUT_DOWN)
			return TRUE;
	}

	return FALSE;
}

u8 F_API INPJoystickAnyBtnReleased(hJOYSTICK jStick)
{
	if(!jStick)
		return FALSE;

	for( u32 i = 0; i < MAXJOYSTICKBTN; i++ )
	{
		if(jStick->btnStates[i] == INPUT_RELEASED)
			return TRUE;
	}

	return FALSE;
}

u8 F_API INPJoystickIsBtnPressed(hJOYSTICK jStick, u8 btn)
{
	if(!jStick)
		return FALSE;

	return (jStick->btnStates[btn] == INPUT_DOWN);
}

u8 F_API INPJoystickIsBtnReleased(hJOYSTICK jStick, u8 btn)
{
	if(!jStick)
		return FALSE;

	return (jStick->btnStates[btn] == INPUT_RELEASED);
}

u8 F_API INPJoystickAnyArrowPressed(hJOYSTICK jStick)
{
	if(!jStick)
		return FALSE;

	for(u32 i = 0; i < eGamePadNum; i++)
	{
		if(jStick->arrowStates[i] == INPUT_DOWN)
			return TRUE;
	}

	return FALSE;
}

u8 F_API INPJoystickAnyArrowReleased(hJOYSTICK jStick)
{
	if(!jStick)
		return FALSE;

	for(u32 i = 0; i < eGamePadNum; i++)
	{
		if(jStick->arrowStates[i] == INPUT_RELEASED)
			return TRUE;
	}

	return FALSE;
}

u8 F_API INPJoystickIsArrowPressed(hJOYSTICK jStick, eGamePadArrows arrow)
{
	if(!jStick)
		return FALSE;

	return (jStick->arrowStates[arrow] == INPUT_DOWN);
}

u8 F_API INPJoystickIsArrowReleased(hJOYSTICK jStick, eGamePadArrows arrow)
{
	if(!jStick)
		return FALSE;

	return (jStick->arrowStates[arrow] == INPUT_RELEASED);
}

void F_API INPJoystickClear(hJOYSTICK jStick)
{
	if(!jStick)
		return;

	memset(jStick->arrowStates, 0, sizeof(jStick->arrowStates));
	memset(jStick->btnStates, 0, sizeof(jStick->btnStates));
}


 s32 INPJoystickReload(hJOYSTICK jStick, void *hMain)
{
	_JoystickDestroyDat(jStick);

	INPJoystickClear(jStick);

	return _INPJoystickInitDat(jStick, hMain, jStick->enumInd, jStick->min, jStick->max);
}

 s32 INPJoystickReloadAll(void *hMain)
{
	for(JoystickList::iterator i = g_joysticks.begin(); i != g_joysticks.end(); i++)
		INPJoystickReload(*i, hMain);

	return RETCODE_SUCCESS;
}