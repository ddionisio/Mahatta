#include "common.h"

#include "wintool.h"

/*LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam,
        LPARAM lparam);*/

//
// globals
//
u8			g_bFullScrn=FALSE;
u8			g_bOpen=TRUE; //is window open?
DEVMODE		g_defaultDevMode={0};
HMAIN		g_hmain = 0;
HMAININST	g_hInst = 0;
//WINCALLBACK g_wCback=0;

char		g_winName[MAXCHARBUFF];

/////////////////////////////////////
// Name:	WinGetBPP
// Purpose:	get the bits per pixel
//			of the current mode we
//			are in (desktop setting)
// Output:	none
// Return:	bits per pixel: 32/24/16...
/////////////////////////////////////
u32 WinGetBPP()
{
	DEVMODE lpDevMode;

	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &lpDevMode);

	return lpDevMode.dmBitsPerPel;
}

/////////////////////////////////////
// Name:	WinInit
// Purpose:	initializes windows
// Output:	window initialized
// Return:	RETCODE_SUCCESS if successful
/////////////////////////////////////
RETCODE WinInit(tWinInfo *pWinInfo)
{
	//destroy previous window
	WinDestroy();

	char menuStr[MAXCHARBUFF]={0};

	if(pWinInfo->menuStr)
		wcstombs(menuStr, pWinInfo->menuStr, MAXCHARBUFF);

	if(pWinInfo->winNameStr)
		wcstombs(g_winName, pWinInfo->winNameStr, MAXCHARBUFF);

	if(!pWinInfo) { return RETCODE_FAILURE; }

	g_hInst = pWinInfo->hinstance;

//	g_wCback = pWinInfo->callback;

	WNDCLASS wndclass={0};
    
    wndclass.style = 0;//CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = pWinInfo->callback;
	g_hInst = wndclass.hInstance = g_hInst ? g_hInst : GetModuleHandle(NULL);
    wndclass.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(pWinInfo->icon));
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);//WHITE_BRUSH);
	wndclass.lpszMenuName  = menuStr;
	wndclass.lpszClassName = g_winName;

    RegisterClass(&wndclass);

	u32 winFlag;

	//set the proper flags for creating the window
	if(TESTFLAGS(pWinInfo->style, WIN_OVERLAPPED))
	{
		winFlag = WS_OVERLAPPEDWINDOW;

		if(TESTFLAGS(pWinInfo->style, WIN_FIXED))
			winFlag ^= (WS_THICKFRAME | WS_MAXIMIZEBOX);
	}
	else if(TESTFLAGS(pWinInfo->style, WIN_POPUP))
		winFlag = WS_POPUP;

	//check for fullscreen
	if(TESTFLAGS(pWinInfo->style, WIN_FULLSCREEN))
	{
		DEVMODE lpDevMode;

		EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &g_defaultDevMode);

		lpDevMode.dmSize				=	sizeof(DEVMODE);
		lpDevMode.dmBitsPerPel			=	pWinInfo->bpp;
		lpDevMode.dmPelsWidth			=	pWinInfo->width;
		lpDevMode.dmPelsHeight			=	pWinInfo->height;
		lpDevMode.dmDisplayFrequency	=	pWinInfo->refresh;
		lpDevMode.dmDriverExtra			=	0;
		lpDevMode.dmFields				=	DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT|DM_DISPLAYFREQUENCY;

		ChangeDisplaySettings(&lpDevMode, 0);

		g_bFullScrn = TRUE;
	}
	else
		g_bFullScrn = FALSE;
	
	g_hmain = CreateWindow(g_winName, g_winName, winFlag,
            pWinInfo->x, // X position.
            pWinInfo->y, // Y position.
            pWinInfo->width,
            pWinInfo->height,
            NULL, //it's not a child, so it's NULL
            0,
            g_hInst,
            pWinInfo->data);

	//set the client rect to be at the right size
	/*if(!g_bFullScrn)
	{
		iRect wR, cR;
		WinGetRect(&wR); WinGetClientRect(&cR);
		u32 w = pWinInfo->width + ((wR.right-wR.left)-(cR.right-cR.left)),
			h = pWinInfo->height + ((wR.bottom-wR.top)-(cR.bottom-cR.top));
		MoveWindow(g_hmain, wR.left, wR.top, w, h, FALSE);
	}*/

	ShowWindow(g_hmain, SW_SHOWNORMAL);
    UpdateWindow(g_hmain);

	g_bOpen = TRUE;

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	WinDestroy
// Purpose:	unregisters window, no
//			need to call this on
//			app. term. Only use this
//			for things like switching
//			from fullscreen to window
//			and vice versa.
// Output:	windows destroyed
// Return:	RETCODE_SUCCESS if successful
/////////////////////////////////////
RETCODE WinDestroy()
{
	if(g_hmain)
	{ DestroyWindow(g_hmain); g_hmain = 0; }

	if(g_hInst)
	{ UnregisterClass(g_winName, g_hInst); g_hInst = 0; }
	
	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	WinQuit
// Purpose:	quit out of application
// Output:	post quit msg. to windows
// Return:	none
/////////////////////////////////////
void WinQuit()
{
	if(g_bFullScrn)
		ChangeDisplaySettings(&g_defaultDevMode, 0);

	PostQuitMessage (0);
}

/////////////////////////////////////
// Name:	WinGetHandle
// Purpose:	grab the main window 
//			handle
// Output:	none
// Return:	the window handle
/////////////////////////////////////
HMAIN WinGetHandle()
{
	return g_hmain;
}

/////////////////////////////////////
// Name:	WinGetHInstance
// Purpose:	grab the main hInstance
// Output:	none
// Return:	the window hInstance
/////////////////////////////////////
HMAININST WinGetHInstance()
{
	return g_hInst;
}

/////////////////////////////////////
// Name:	WinGetRect
// Purpose:	get the window area
// Output:	pRect filled
// Return:	none
/////////////////////////////////////
void WinGetRect(iRect *pRect)
{
	WINDOWINFO wi;

	GetWindowInfo(WinGetHandle(), &wi);

	memcpy(pRect, &wi.rcWindow, sizeof(iRect));
}

/////////////////////////////////////
// Name:	WinGetClientRect
// Purpose:	get the client area
// Output:	pRect filled
// Return:	none
/////////////////////////////////////
void WinGetClientRect(iRect *pRect)
{
	GetClientRect(WinGetHandle(), (LPRECT)pRect);
}

/////////////////////////////////////
// Name:	WinIsOpen
// Purpose:	checks if window is open
// Output:	none
// Return:	TRUE if open, FALSE o.w.
/////////////////////////////////////
u8 WinIsOpen()
{
	return g_bOpen;
}

/////////////////////////////////////
// Name:	WinOpen
// Purpose:	open/close the window
//			doesn't do much, but
//			helps for your app to know
//			if you want to do something
// Output:	flag open set
// Return:	none
/////////////////////////////////////
void WinOpen(u8 bYes)
{
	g_bOpen = bYes;
}

/////////////////////////////////////
// Name:	WinGetMessage
// Purpose:	process window messages
// Output:	none
// Return:	RETCODE_BREAK if we get
//			a quit message
/////////////////////////////////////
RETCODE WinGetMessage()
{
	MSG msg;

	while(PeekMessage(&msg,NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return RETCODE_BREAK;
		}
		else
		{
			TranslateMessage (&msg);
			DispatchMessage (&msg);
		}
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	WinSetTitle
// Purpose:	set window title
// Output:	window title set
// Return:	none
/////////////////////////////////////
void WinSetTitle(const tCHAR *str)
{
	char winNameStr[MAXCHARBUFF];
	wcstombs(winNameStr, str, MAXCHARBUFF);

	SetWindowText(WinGetHandle(), winNameStr);
}

/////////////////////////////////////
// Name:	WinShowCursor
// Purpose:	enable/disable cursor display
// Output:	mouse cursor shown/hidden
// Return:	none
/////////////////////////////////////
void WinShowCursor(u8 bCursor)
{
	ShowCursor(bCursor);
}

/////////////////////////////////////
// Name:	WinAdjustClientRect
// Purpose:	this will set the client's
//			width and height correctly
//			with given w and h
// Output:	hMain client size set
// Return:	none
/////////////////////////////////////
void WinAdjustClientRect(HMAIN hMain, u32 w, u32 h)
{
	iRect  rcWork;
    iRect  rc;
    //u32 dwStyle;

	// Aet window size
	SetRect( (LPRECT)&rc, 0, 0, w, h);

    AdjustWindowRectEx( (LPRECT)&rc, GetWindowStyle(hMain), GetMenu(hMain) != NULL,
                        GetWindowExStyle(hMain) );

    SetWindowPos( hMain, NULL, 0, 0, rc.right-rc.left, rc.bottom-rc.top,
                  SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE );

    SetWindowPos( hMain, HWND_NOTOPMOST, 0, 0, 0, 0,
                  SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE );

    //  Make sure our window does not hang outside of the work area
    SystemParametersInfo( SPI_GETWORKAREA, 0, &rcWork, 0 );
    GetWindowRect( hMain, (LPRECT)&rc );
    if( rc.left < rcWork.left ) rc.left = rcWork.left;
    if( rc.top  < rcWork.top )  rc.top  = rcWork.top;
    SetWindowPos( hMain, NULL, rc.left, rc.top, 0, 0,
                  SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );
}