#include "input_i.h"

LPDIRECTINPUT8       g_pDInput=0;

u32		 g_coopFlag;

s32 F_API INPInit(void *hMain, void *hinst, u32 coopFlag)
{
	g_coopFlag = coopFlag;
	//create the main input interface
	HRESULT hr = DirectInput8Create((HINSTANCE)hinst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_pDInput, NULL);
	
	if(FAILED(hr))
	{ DInputError(hr, L"_INPInit"); return RETCODE_FAILURE; }

	return RETCODE_SUCCESS;
}

void F_API INPDestroy()
{
	INPKBDestroy();
	INPMouseDestroy();
	INPJoystickDestroyAll();

	if(g_pDInput)
	{
		g_pDInput->Release();
		g_pDInput=0;
	}

	//g_joystickEnums.clear();
	//g_joystickEnums.resize(0);
}

s32 F_API INPReInit(void *hMain, void *hinst)
{
	//reinitialize input
	if(g_pDInput)
	{
		g_pDInput->Release();
		g_pDInput=0;
	}

	INPInit(hMain, hinst, g_coopFlag);

	//reinitialize keyboard
	if(INPKBExist())
	{
		INPKBDestroy();
		INPKBInit(hMain);
		INPKBClear();
	}

	//reinitialize mouse
	if(INPMouseExist())
	{
		INPMouseDestroy();
		INPMouseInit(hMain, INPMouseIsExclusive(), 0);
		INPMouseClear();
	}

	//reinitialize joysticks
	INPJoystickReloadAll(hMain);

	return RETCODE_SUCCESS;
}