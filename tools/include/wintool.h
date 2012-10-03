#ifndef _wintool_h
#define _wintool_h

/********************************************************************
*																	*
*						DEFINES/MACROS/ENUMS						*
*																	*
********************************************************************/

//Window styles
#define WIN_OVERLAPPED		1		//you cannot mix overlapped and pop-up
#define WIN_POPUP			2
#define WIN_FIXED			4		//window is not sizeable or maximizable
#define WIN_FULLSCREEN		8		//use fullscreen

//
// THE Main, use this for your main function
//
#define TheMain() WINAPI WinMain(HMAININST hInstance,		\
							HMAININST hPreviousInstance,	\
							PSTR szCmdLine,					\
							s32 iCmdShow)

/********************************************************************
*																	*
*							Types									*
*																	*
********************************************************************/

/********************************************************************
*																	*
*							Structs									*
*																	*
********************************************************************/
typedef struct _tWinInfo
{
	u32			style;		//window style
	HMAININST	hinstance;	//hinstance
	u32			icon;		//icon ID (in win32, it's one of those resource stuff)
	tCHAR	   *winNameStr; //the window title
	tCHAR	   *menuStr;	//the menu, if you want menu

	s32			x,y;			//window coordinate
	s32			width, height;	//window size

	u32			bpp;		//fullscreen only, bits per pixel
	u32			refresh;	//fullscreen only, refresh rate

	WNDPROC		callback;	//callback for retrieving messages

	void	   *data;		//misc. data
} tWinInfo;

/********************************************************************
*																	*
*							Functions								*
*																	*
********************************************************************/

/////////////////////////////////////
// Name:	WinGetBPP
// Purpose:	get the bits per pixel
//			of the current mode we
//			are in (desktop setting)
// Output:	none
// Return:	bits per pixel: 32/24/16...
/////////////////////////////////////
u32 WinGetBPP();

/////////////////////////////////////
// Name:	WinInit
// Purpose:	initializes windows
// Output:	window initialized
// Return:	RETCODE_SUCCESS if successful
/////////////////////////////////////
RETCODE WinInit(tWinInfo *pWinInfo);

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
RETCODE WinDestroy();

/////////////////////////////////////
// Name:	WinQuit
// Purpose:	quit out of application
// Output:	post quit msg. to windows
// Return:	none
/////////////////////////////////////
void WinQuit();

/////////////////////////////////////
// Name:	WinGetHandle
// Purpose:	grab the main window 
//			handle
// Output:	none
// Return:	the window handle
/////////////////////////////////////
HMAIN WinGetHandle();

/////////////////////////////////////
// Name:	WinGetHInstance
// Purpose:	grab the main hInstance
// Output:	none
// Return:	the window hInstance
/////////////////////////////////////
HMAININST WinGetHInstance();

/////////////////////////////////////
// Name:	WinGetRect
// Purpose:	get the window area
// Output:	pRect filled
// Return:	none
/////////////////////////////////////
void WinGetRect(iRect *pRect);

/////////////////////////////////////
// Name:	WinGetClientRect
// Purpose:	get the client area
// Output:	pRect filled
// Return:	none
/////////////////////////////////////
void WinGetClientRect(iRect *pRect);

/////////////////////////////////////
// Name:	WinIsOpen
// Purpose:	checks if window is open
// Output:	none
// Return:	TRUE if open, FALSE o.w.
/////////////////////////////////////
u8 WinIsOpen();

/////////////////////////////////////
// Name:	WinOpen
// Purpose:	open/close the window
//			doesn't do much, but
//			helps for your app to know
//			if you want to do something
// Output:	flag open set
// Return:	none
/////////////////////////////////////
void WinOpen(u8 bYes);

/////////////////////////////////////
// Name:	WinGetMessage
// Purpose:	process window messages
// Output:	none
// Return:	RETCODE_BREAK if we get
//			a quit message
/////////////////////////////////////
RETCODE WinGetMessage();

/////////////////////////////////////
// Name:	WinSetTitle
// Purpose:	set window title
// Output:	window title set
// Return:	none
/////////////////////////////////////
void WinSetTitle(const tCHAR *str);

/////////////////////////////////////
// Name:	WinShowCursor
// Purpose:	enable/disable cursor display
// Output:	mouse cursor shown/hidden
// Return:	none
/////////////////////////////////////
void WinShowCursor(u8 bCursor);

/////////////////////////////////////
// Name:	WinAdjustClientRect
// Purpose:	this will set the client's
//			width and height correctly
//			with given w and h
// Output:	hMain client size set
// Return:	none
/////////////////////////////////////
void WinAdjustClientRect(HMAIN hMain, u32 w, u32 h);

#endif