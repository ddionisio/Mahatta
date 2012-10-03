#include "input_i.h"

LPDIRECTINPUTDEVICE8 g_pDMouse=0;			//The mouse device interface

u8					 g_bExclusive;

u8					 g_mouseBtn[MAXMOUSEBTN];	//btn states (up/released/down)

iPoint				 g_mouseLoc, g_bound;
s32					 g_mouseRelX, g_mouseRelY, g_mouseRelZ;

/////////////////////////////////////
// Name:	INPMouseInit
// Purpose:	initialize mouse
//			if bExclusive, this means
//			we don't get actual windows
//			mouse location.  Use it on
//			fullscreen only.
// Output:	mouse initialized
// Return:	success if so
/////////////////////////////////////
s32 F_API INPMouseInit(void *hMain, u8 bExclusive, iPoint *pBound)
{
	INPMouseDestroy();

	HRESULT hr;

	//create the mouse device
	hr = g_pDInput->CreateDevice(GUID_SysMouse, &g_pDMouse, NULL);
	if(FAILED(hr))
	{ DInputError(hr, L"INPMouseInit"); return RETCODE_FAILURE; }

	//set mouse device as a REAL mouse
	hr = g_pDMouse->SetDataFormat(&c_dfDIMouse2);
	if(FAILED(hr))
	{ DInputError(hr, L"INPMouseInit"); return RETCODE_FAILURE; }

	//set up mouse coop level
	u32 coop = g_coopFlag & ~(INP_EXCLUSIVE | INP_NONEXCLUSIVE);
	coop |= bExclusive ? INP_EXCLUSIVE : INP_NONEXCLUSIVE;

	hr = g_pDMouse->SetCooperativeLevel((HWND)hMain, coop);
	if(FAILED(hr))
	{ DInputError(hr, L"INPMouseInit"); return RETCODE_FAILURE; }

	//acquire mouse device
	g_pDMouse->Acquire();

	g_bExclusive = bExclusive;
	memset(&g_mouseLoc, 0, sizeof(g_mouseLoc));

	INPMouseSetBound(pBound);

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	INPMouseSetBound
// Purpose:	set the boundary of mouse
// Output:	
// Return:	
/////////////////////////////////////
void F_API INPMouseSetBound(iPoint *pBound)
{
	if(pBound)
		g_bound = *pBound;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
void F_API INPMouseDestroy()
{
	if(g_pDMouse)
	{
		g_pDMouse->Unacquire();
		g_pDMouse->Release();
		g_pDMouse=0;
	}
}

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
s32 F_API INPMouseUpdate(void *hMain)
{
	if(!g_pDMouse)
		return RETCODE_SUCCESS;

	u8 i;
	DIMOUSESTATE2 mouseDat={0};

	//update released buttons
	for(i = 0; i < MAXMOUSEBTN; i++)
	{
		if(g_mouseBtn[i] == INPUT_RELEASED)
			g_mouseBtn[i] = INPUT_UP;
	}

	HRESULT hr;

	hr = g_pDMouse->Poll();

	if(FAILED(hr))
	{
		// DInput is telling us that the input stream has been
        // interrupted. We aren't tracking any state between polls, so
        // we don't have any special reset that needs to be done. We
        // just re-acquire and try again.
        hr = g_pDMouse->Acquire();
        while( hr == DIERR_INPUTLOST ) 
            hr = g_pDMouse->Acquire();

        // hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
        // may occur when the app is minimized or in the process of 
        // switching, so just try again later 
        return RETCODE_SUCCESS; 
	}

	// Get the input's device state
    if(FAILED(hr = g_pDMouse->GetDeviceState(sizeof(DIMOUSESTATE2), &mouseDat)))
	{ DInputError(hr, L"INPMouseUpdate"); return RETCODE_FAILURE; }

	//check the buttons
	for(i = 0; i < MAXMOUSEBTN; i++)
    {
		if(mouseDat.rgbButtons[i] & INPUT_DOWN)
			g_mouseBtn[i] = INPUT_DOWN;
		else if(g_mouseBtn[i] == INPUT_DOWN)
			g_mouseBtn[i] = INPUT_RELEASED;
	}

	//check location
	if(!g_bExclusive)
	{
		GetCursorPos((LPPOINT)&g_mouseLoc);
		ScreenToClient((HWND)hMain, (LPPOINT)&g_mouseLoc);
	}
	else
	{
		g_mouseLoc.x += mouseDat.lX;

		if(g_mouseLoc.x < 0) g_mouseLoc.x = 0;
		else if(g_mouseLoc.x > g_bound.x) g_mouseLoc.x = g_bound.x;

		g_mouseLoc.y += mouseDat.lY;

		if(g_mouseLoc.y < 0) g_mouseLoc.y = 0;
		else if(g_mouseLoc.y > g_bound.y) g_mouseLoc.y = g_bound.y;
	}

	g_mouseRelX = mouseDat.lX;
	g_mouseRelY = mouseDat.lY;
	g_mouseRelZ = mouseDat.lZ;

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	INPMouseGetMove
// Purpose:	get the relative motion of
//			the mouse.
// Output:	pX,pY,pZ is set
// Return:	none
/////////////////////////////////////
void F_API INPMouseGetMove(s32 *pX, s32 *pY, s32 *pZ)
{
	*pX = g_mouseRelX;
	*pY = g_mouseRelY;
	*pZ = g_mouseRelZ;
}

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
void F_API INPMouseGetLoc(iPoint *pLoc)
{
	memcpy(pLoc, &g_mouseLoc, sizeof(iPoint));
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
u8 F_API INPMouseAnyBtnPressed()
{
	for( u32 i = 0; i < MAXMOUSEBTN; i++ )
	{
		if(g_mouseBtn[i] == INPUT_DOWN)
			return TRUE;
	}

	return FALSE;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
u8 F_API INPMouseAnyBtnReleased()
{
	for( u32 i = 0; i < MAXMOUSEBTN; i++ )
	{
		if(g_mouseBtn[i] == INPUT_RELEASED)
			return TRUE;
	}

	return FALSE;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
u8 F_API INPMouseIsPressed(u8 btn)
{
	return (g_mouseBtn[btn] == INPUT_DOWN);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
u8 F_API INPMouseIsReleased(u8 btn)
{
	return (g_mouseBtn[btn] == INPUT_RELEASED);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
void F_API INPMouseClear()
{
	memset(g_mouseBtn, 0, sizeof(g_mouseBtn));
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
u8 F_API INPMouseExist()
{
	return g_pDMouse == 0 ? FALSE : TRUE;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
u8 F_API INPMouseIsExclusive()
{
	return g_bExclusive;
}