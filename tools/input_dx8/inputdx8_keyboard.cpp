#include "input_i.h"

LPDIRECTINPUTDEVICE8 g_pDKeyboard=0;			//The keyboard device interface

DIDEVICEOBJECTDATA	g_pKbBuff[SAMPLE_BUFFER_SIZE];					//Keyboard buffer

u32 g_prevKNum=0;

u8 g_keyState[MAXKBKEY]={0};

inpState g_buffState[SAMPLE_BUFFER_SIZE]={0};
u8		 g_buffStateSize=0;

#define KEYDOWN(data)		(g_keyState[data]==INPUT_DOWN)
#define KEYRELEASED(data)	(g_keyState[data]==INPUT_RELEASED)

DLL_API u8 F_API INPKBExist()
{
	return g_pDKeyboard == 0 ? FALSE : TRUE;
}

s32 F_API INPKBInit(void *hMain)
{
	INPKBDestroy();

	HRESULT hr;

	//create the keyboard device
	hr = g_pDInput->CreateDevice(GUID_SysKeyboard, &g_pDKeyboard, NULL);
	if(FAILED(hr))
	{ DInputError(hr, L"_INPKBInit"); return RETCODE_FAILURE; }

	//set keyboard device as a REAL keyboard
	hr = g_pDKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(hr))
	{ DInputError(hr, L"_INPKBInit"); return RETCODE_FAILURE; }

	//set up keyboard coop level
	hr = g_pDKeyboard->SetCooperativeLevel((HWND)hMain, g_coopFlag);
	if(FAILED(hr))
	{ DInputError(hr, L"_INPKBInit"); return RETCODE_FAILURE; }

	//set up buffering
	DIPROPDWORD dipdw;

    dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj        = 0;
    dipdw.diph.dwHow        = DIPH_DEVICE;
    dipdw.dwData            = SAMPLE_BUFFER_SIZE; // Arbitary buffer size

	//set up keyboard property to tell that we are using buffer
	hr = g_pDKeyboard->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
    if(FAILED(hr))
	{ DInputError(hr, L"_INPKBInit"); return RETCODE_FAILURE; }

	//acquire keyboard device
	g_pDKeyboard->Acquire();

	return RETCODE_SUCCESS;
}

void F_API INPKBDestroy()
{
	if(g_pDKeyboard)
	{
		g_pDKeyboard->Unacquire();
		g_pDKeyboard->Release();
		g_pDKeyboard=0;
	}
}

s32 F_API INPKBUpdate()
{
	if(!g_pDKeyboard)
		return RETCODE_SUCCESS;

	u32 i;

	//////////////////////
	//reset buffer
	for(i=0; i < g_prevKNum; i++)
	{
		if(KEYRELEASED(g_buffState[i].ofs))
		{
			g_buffState[i].state = INPUT_UP;
			g_keyState[g_buffState[i].ofs] = INPUT_UP;
		}
	}

	g_buffStateSize = 0;
	//////////////////////

	HRESULT hr;

	g_prevKNum = SAMPLE_BUFFER_SIZE;
	
	//First, check to see if the keyboard is still working/functioning
    hr= g_pDKeyboard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),g_pKbBuff,(LPDWORD)&g_prevKNum,0);
    if(hr != DI_OK)
	{
		g_prevKNum = 0;

		//did we lose the keyboard?
        if(hr==DIERR_INPUTLOST || hr==DIERR_NOTACQUIRED) 
		{
			//we gotta have it back!
			g_pDKeyboard->Acquire();
			/*hr = g_pDKeyboard->Acquire();
			if(FAILED(hr))
			{ DInputError(hr, "_INPKBUpdate"); return RETCODE_FAILURE; }
*/
			return RETCODE_SUCCESS;
		}
		else
			return RETCODE_FAILURE;
	}

	//update the keyboard states
	for(i=0; i < g_prevKNum; i++)
	{
		g_buffState[i].ofs = g_pKbBuff[i].dwOfs;
		g_buffState[i].state = (g_pKbBuff[i].dwData & INPUT_DOWN) ? INPUT_DOWN : INPUT_RELEASED;

		g_keyState[g_buffState[i].ofs] = g_buffState[i].state;
	}

	g_buffStateSize=g_prevKNum;

	return RETCODE_SUCCESS;
}

u8 F_API INPKBGetBufferStates(inpState buff[SAMPLE_BUFFER_SIZE])
{
	memcpy(buff, g_buffState, sizeof(g_buffState));
	return g_buffStateSize;
}

u8 F_API INPKBAnyKeyPressed()
{
	for (u32 count = 0; count < MAXKBKEY; count++)
		if(g_keyState[count] == INPUT_DOWN)
			return TRUE;

	return FALSE;
}

u8 F_API INPKBAnyKeyReleased()
{
	for (s32 count = 0; count < MAXKBKEY; count++)
		if(g_keyState[count] == INPUT_RELEASED)
			return TRUE;

	return FALSE;
}

u8 F_API INPKBIsPressed(u8 kCode)
{
	return (g_keyState[kCode] == INPUT_DOWN);
}

u8 F_API INPKBIsReleased(u8 kCode)
{
	return (g_keyState[kCode] == INPUT_RELEASED);
}

void F_API INPKBClear()
{
	memset(g_keyState, 0, sizeof(g_keyState));
}

tCHAR F_API INPKBGetAscii(u8 kCode)
{ 
	
	tCHAR retvalue = L'';
	char asciichar[256];

	memset(asciichar,0, sizeof(char)*256);

	HKL layout = GetKeyboardLayout(0);
	u8 State[256];
	u32  scancode;
	memset(State, 0, sizeof (u8) * 256);
	u32 vk = MapVirtualKeyEx(kCode,1,layout);
	bool returnvaluegetkeyb = GetKeyboardState(State);  //This function returns crap if we are in exclusive mode

	//CODE TO CHECK OS VERSION,
	//ToUnicodeEx doesn't work in win98,me
   

	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx((OSVERSIONINFO*)&osvi); //Should move this function to initialization

   //VER_PLATFORM_WIN32_NT //WinNT4 to Win2003
   //VER_PLATFORM_WIN32_WINDOWS //Windows 95, Windows 98, or Windows Me.
   if ( osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) //Crappy Versions of Windows Without Unicode
   {
	   int returnvalascii = ToAsciiEx(vk, kCode, State, (unsigned short *)asciichar, 0, layout);
		if(returnvalascii > 0)
		{
			int retrunvaluembtowcs = mbstowcs(&retvalue, asciichar, 1);
			if(retrunvaluembtowcs == -1) //some error
				retvalue = L'';
		}
		else
			retvalue = L'';
   }
   else //Good versions of widnows with Unicode
   {
	   s32 retvalunicode = ToUnicodeEx(vk, kCode, State, &retvalue, 1, 0, layout);
	   if(retvalunicode <= 0) // -1 or 0 mean error or no character was translated
		   retvalue = L'';
   }


/*
   HKL layout = GetKeyboardLayout(0);
   unsigned char State[256];
   if (GetKeyboardState(State)==FALSE)
      return 0;
   unsigned int vk = MapVirtualKeyEx(scancode,1,layout);
   return ToAsciiEx(vk,scancode,State,(unsigned short *)result,0,layout);
*/
/*
BOOL bIsWindowsVersionOK(DWORD dwMajor, DWORD dwMinor, DWORD dwSPMajor )
{
OSVERSIONINFO osvi;
 
// Initialize the OSVERSIONINFO structure.
//
ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
GetVersionEx((OSVERSIONINFO*)&osvi);
 
// First the major version
if ( osvi.dwMajorVersion > dwMajor )
   return TRUE;
else if ( osvi.dwMajorVersion == dwMajor )
   {
   // Then the minor
   if (osvi.dwMinorVersion > dwMinor )
      return TRUE;
   else if (osvi.dwMinorVersion == dwMinor )
      {
      // OK, better check the Service Pack
      if ( dwSPMajor && 
        osvi.dwPlatformId == VER_PLATFORM_WIN32_NT )
         {
         HKEY   hKey;
         DWORD dwCSDVersion;
          DWORD dwSize;
         BOOL   fMeetsSPRequirement = FALSE;
 
         if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
            System\\CurrentControlSet\\Control\\Windows",
0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
            {
            dwSize = sizeof(dwCSDVersion);
            if (RegQueryValueEx(hKey, "CSDVersion",
               NULL, NULL,
(unsigned char*)&dwCSDVersion, 
&dwSize) == ERROR_SUCCESS)
               {
               fMeetsSPRequirement = 
(LOWORD(dwCSDVersion) >= dwSPMajor);
               }
           RegCloseKey(hKey);
           }
         return fMeetsSPRequirement;
         }
      return TRUE;
      }
   }
return FALSE;
}

*/

   return retvalue;
}

/////////////////////////////////////
// Name:	INPKBGetStrReleased
// Purpose:	grab all keys and
//			put them to ascii
// Output:	
// Return:	
/////////////////////////////////////
void F_API INPKBGetStrReleased(tCHAR *outbuffer, u32 outbuffersize)
{
	memset(outbuffer, 0, sizeof(tCHAR)*outbuffersize);

	for (u32 i = 0, count = 0; count < MAXKBKEY && i < (outbuffersize-1); count++)
	{
		if(g_keyState[count] == INPUT_RELEASED)
		{
			outbuffer[i] = INPKBGetAscii(count);

			if(outbuffer[i] != 0)
				i++;
		}
	}
}

static const tCHAR *g_kcodeStr[] = {
0,
L"ESCAPE",
L"1",
L"2",
L"3",
L"4",
L"5",
L"6",
L"7",
L"8",
L"9",
L"0",
L"MINUS",    /* - on main keyboard */
L"EQUALS",
L"BACK",    /* backspace */
L"TAB",
L"Q",
L"W",
L"E",
L"R",
L"T",
L"Y",
L"U",
L"I",
L"O",
L"P",
L"LBRACKET",
L"RBRACKET",
L"RETURN",    /* Enter on main keyboard */
L"LCONTROL",
L"A",
L"S",
L"D",
L"F",
L"G",
L"H",
L"J",
L"K",
L"L",
L"SEMICOLON",
L"APOSTROPHE",
L"GRAVE",    /* accent grave */
L"LSHIFT",
L"BACKSLASH",
L"Z",
L"X",
L"C",
L"V",
L"B",
L"N",
L"M",
L"COMMA",
L"PERIOD",    /* . on main keyboard */
L"SLASH",    /* / on main keyboard */
L"RSHIFT",
L"MULTIPLY",    /* * on numeric keypad */
L"LMENU",    /* left Alt */
L"SPACE",
L"CAPSLOCK",
L"F1",
L"F2",
L"F3",
L"F4",
L"F5",
L"F6",
L"F7",
L"F8",
L"F9",
L"F10",
L"NUMLOCK",
L"SCROLL",    /* Scroll Lock */
L"NUMPAD7",
L"NUMPAD8",
L"NUMPAD9",
L"SUBTRACT",    /* - on numeric keypad */
L"NUMPAD4",
L"NUMPAD5",
L"NUMPAD6",
L"ADD",    /* + on numeric keypad */
L"NUMPAD1",
L"NUMPAD2",
L"NUMPAD3",
L"NUMPAD0",
L"DECIMAL",    /* . on numeric keypad */
L"OEM_102",    /* <> or \| on RT 102-key keyboard (Non-U.S.) */
L"F11",
L"F12",
0,0,0,0,0,0,0,0,0,0,0,
L"F13",    /*                     (NEC PC98) */
L"F14",    /*                     (NEC PC98) */
L"F15",    /*                     (NEC PC98) */
0,0,0,0,0,0,0,0,0,
L"KANA",    /* (Japanese keyboard)            */
0,0,
L"ABNT_C1",    /* /? on Brazilian keyboard */
0,0,0,0,0,
L"CONVERT",    /* (Japanese keyboard)            */
0,
L"NOCONVERT",    /* (Japanese keyboard)            */
0,
L"YEN",    /* (Japanese keyboard)            */
L"ABNT_C2",    /* Numpad . on Brazilian keyboard */
0,0,0,0,0,0,0,0,0,0,0,0,0,0,
L"NUMPADEQUALS",    /* = on numeric keypad (NEC PC98) */
0,0,
L"PREVTRACK",    /* Previous Track (INP_CIRCUMFLEX on Japanese keyboard) */
L"AT",    /*                     (NEC PC98) */
L"COLON",    /*                     (NEC PC98) */
L"UNDERLINE",    /*                     (NEC PC98) */
L"KANJI",    /* (Japanese keyboard)            */
L"STOP",    /*                     (NEC PC98) */
L"AX",    /*                     (Japan AX) */
L"UNLABELED",    /*                        (J3100) */
0,
L"NEXTTRACK",    /* Next Track */
0,0,
L"NUMPADENTER",    /* Enter on numeric keypad */
L"RCONTROL",
0,0,
L"MUTE",    /* Mute */
L"CALCULATOR",    /* Calculator */
L"PLAYPAUSE",    /* Play / Pause */
0,
L"MEDIASTOP",    /* Media Stop */
0,0,0,0,0,0,0,0,0,
L"VOLUMEDOWN",    /* Volume - */
0,
L"VOLUMEUP",    /* Volume + */
0,
L"WEBHOME",    /* Web home */
L"NUMPADCOMMA",    /* , on numeric keypad (NEC PC98) */
0,
L"DIVIDE",    /* / on numeric keypad */
0,
L"SYSRQ",
L"RMENU",    /* right Alt */
0,0,0,0,0,0,0,0,0,0,0,0,0,0,
L"PAUSE",    /* Pause */
0,
L"HOME",    /* Home on arrow keypad */
L"UP",    /* UpArrow on arrow keypad */
L"PRIOR",    /* PgUp on arrow keypad */
0,
L"LEFT",    /* LeftArrow on arrow keypad */
0,
L"RIGHT",    /* RightArrow on arrow keypad */
0,
L"END",    /* End on arrow keypad */
L"DOWN",    /* DownArrow on arrow keypad */
L"NEXT",    /* PgDn on arrow keypad */
L"INSERT",    /* Insert on arrow keypad */
L"DELETE",    /* Delete on arrow keypad */
0,0,0,0,0,0,0,
L"LWIN",    /* Left Windows key */
L"RWIN",    /* Right Windows key */
L"APPS",    /* AppMenu key */
L"POWER",    /* System Power */
L"SLEEP",    /* System Sleep */
0,0,0,
L"WAKE",    /* System Wake */
0,
L"WEBSEARCH",    /* Web Search */
L"WEBFAVORITES",    /* Web Favorites */
L"WEBREFRESH",    /* Web Refresh */
L"WEBSTOP",    /* Web Stop */
L"WEBFORWARD",    /* Web Forward */
L"WEBBACK",    /* Web Back */
L"MYCOMPUTER",    /* My Computer */
L"MAIL",    /* Mail */
L"MEDIASELECT"    /* Media Select */
};

void F_API INPKBGetKeyName(u8 kCode, tCHAR *outbuffer, u32 size)
{
	DIPROPSTRING string={0};

	string.diph.dwSize = sizeof(DIPROPSTRING);
	string.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	string.diph.dwObj = kCode;
	string.diph.dwHow = DIPH_BYOFFSET;

	HRESULT hr = g_pDKeyboard->GetProperty(DIPROP_KEYNAME,
	&string.diph);

	if(FAILED(hr))
	{ *outbuffer=0; }
	else
	{
		if(size > 260)
		{
			memcpy(outbuffer, string.wsz, sizeof(string.wsz));
		}
		else
		{
			memcpy(outbuffer, string.wsz, sizeof(tCHAR)*size);
			outbuffer[size-1] = 0;
		}
		//wcscpy(outbuffer, string.wsz);
	}

	/*WideCharToMultiByte(CP_ACP, 0, string.wsz, -1,
	outbuffer, outbuffersize, 
	NULL, NULL);*/
}

const tCHAR * F_API INPKBToStr(u8 kCode)
{
	return g_kcodeStr[kCode];
}

u8 F_API INPKBToCode(const tCHAR *str)
{
	for(u8 i = 1; i < INP_MAXCODE; i++)
	{
		if(g_kcodeStr[i] && wcsicmp(g_kcodeStr[i], str) == 0)
			return i;
	}

	return 0;
}