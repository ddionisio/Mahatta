#define _WIN32_DCOM 

#include <dshow.h>
//#include <stdio.h>
#include <atlbase.h>
//#include <objbase.h>

#include "gfx_i.h"

#pragma comment(lib, "Quartz.lib")
#pragma comment(lib, "Strmiids.lib")

void _VidLogError(HRESULT hr, const tCHAR *header)
{
	switch(hr)
	{
	case VFW_S_AUDIO_NOT_RENDERED:
		LogMsg(LOG_FILE, header, L"Partial success; the audio was not rendered.");
		break;
		//case VFW_S_DUPLICATE_NAME Success; the Filter Graph Manager modified the filter name to avoid duplication. 
	case VFW_S_PARTIAL_RENDER:
		LogMsg(LOG_FILE, header, L"Some of the streams in this movie are in an unsupported format.");
		break;
	case VFW_S_VIDEO_NOT_RENDERED:
		LogMsg(LOG_FILE, header, L"Partial success; some of the streams in this movie are in an unsupported format.");
		break;
	case E_ABORT:
		LogMsg(LOG_FILE, header, L"Operation aborted.");
		break;
	case E_INVALIDARG:
		LogMsg(LOG_FILE, header, L"Argument is invalid.");
		break;
	case E_OUTOFMEMORY:
		LogMsg(LOG_FILE, header, L"Insufficient memory.");
		break;
	case E_POINTER: 
		LogMsg(LOG_FILE, header, L"NULL pointer argument.");
		break;
	case VFW_E_CANNOT_CONNECT:
		LogMsg(LOG_FILE, header, L"No combination of intermediate filters could be found to make the connection.");
		break;
	case VFW_E_CANNOT_LOAD_SOURCE_FILTER:
		LogMsg(LOG_FILE, header, L"The source filter for this file could not be loaded.");
		break;
	case VFW_E_CANNOT_RENDER:
		LogMsg(LOG_FILE, header, L"No combination of filters could be found to render the stream.");
		break;
	case VFW_E_INVALID_FILE_FORMAT:
		LogMsg(LOG_FILE, header, L"The file format is invalid.");
		break;
	case VFW_E_NOT_FOUND:
		LogMsg(LOG_FILE, header, L"An object or name was not found.");
		break;
	case VFW_E_UNKNOWN_FILE_TYPE:
		LogMsg(LOG_FILE, header, L"The media type of this file is not recognized.");
		break;
	case VFW_E_UNSUPPORTED_STREAM:
		LogMsg(LOG_FILE, header, L"Cannot play back the file: the format is not supported.");
		break;
	default:
		LogMsg(LOG_FILE, header, L"Failure...of some sorts.");
		break;
	}
}

typedef struct _gfxVideo {
	IGraphBuilder			*pGraph;
	IMediaControl			*pMediaControl;
	IVideoWindow			*pVidWin;
	IMediaSeeking			*pMediaSeeking;

	IMediaEventEx			*pEvent;

	u8						 bDataLoaded;
	u8						 bRunning;
	u8						 bActive;

	HWND					 hdirectshowWnd;
	WNDCLASSEX				 wc;
} gfxVideo;

static gfxVideo			 g_video={0};

static HRESULT _VideoUnload()
{
	HRESULT hr = S_OK;

	if(g_video.bDataLoaded) 
	{
		hr = g_video.pMediaControl->Stop();
		g_video.bRunning=FALSE;

		hr = g_video.pEvent->SetNotifyWindow(NULL, 0, 0);
		hr = g_video.pVidWin->put_Visible(FALSE);
		hr = g_video.pVidWin->put_Owner(NULL);

		SAFE_RELEASE(g_video.pGraph);
		SAFE_RELEASE(g_video.pMediaControl);
		SAFE_RELEASE(g_video.pVidWin);
		SAFE_RELEASE(g_video.pEvent);
		SAFE_RELEASE(g_video.pMediaSeeking);
		g_video.bDataLoaded=FALSE;
	}

	return hr;
}

static HRESULT _VideoLoad(HMAIN hwnd, s32 winMessage, const tCHAR *filename)
{
	HRESULT hr;

	if(g_video.bDataLoaded)
		_VideoUnload();

	tCHAR path[MAXCHARBUFF];
	FSGetPath(filename, path, MAXCHARBUFF);
	
	if(FAILED(hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&g_video.pGraph)))
		return hr;

	if(FAILED(hr = g_video.pGraph->QueryInterface(IID_IMediaControl, (void **)&g_video.pMediaControl)))
		return hr;

	if(FAILED(hr = g_video.pGraph->QueryInterface(IID_IVideoWindow, (void **)&g_video.pVidWin)))
		return hr;

	if(FAILED(hr = g_video.pGraph->QueryInterface(IID_IMediaEventEx, (void **)&g_video.pEvent)))
		return hr;

	if(FAILED(hr = g_video.pGraph->QueryInterface(IID_IMediaSeeking, (void **)&g_video.pMediaSeeking)))
		return hr;
	
	if(FAILED(hr = g_video.pEvent->SetNotifyWindow((OAHWND)hwnd, winMessage, 0)))
		return hr;
	
	if(FAILED(hr = g_video.pGraph->RenderFile(path, NULL)))
		return hr;

	if(FAILED(hr = g_video.pVidWin->put_Owner((OAHWND)hwnd)))
		return hr;

	//g_video.pVidWin->put_AutoShow(OAFALSE);

	RECT r;
	GetWindowRect(hwnd, &r);

	if(FAILED(hr = g_video.pVidWin->put_WindowStyle( WS_POPUP )))
		return hr;

	if(FAILED(hr = g_video.pVidWin->SetWindowPosition(r.left, r.top, r.right, r.bottom)))
		return hr;
	
	g_video.bRunning=FALSE;
	g_video.bDataLoaded=TRUE;

	return S_OK;
}

static HRESULT _VideoInit()
{
	HRESULT hr;

	if(!g_video.bActive)
	{
		if(FAILED(hr = CoInitialize(NULL))) 	// Initializes COM
			return hr;

		g_video.bActive = TRUE;
	}

	return S_OK;
}

static void _VideoDeInit()
{
	g_video.bActive = FALSE;

	CoUninitialize();      // Uninitializes COM
	if(g_video.bDataLoaded) _VideoUnload();
}

HWND g_hwnd;

#define WIN_VID_TITLE	(L"RenderVideo")
#define GFXWM_GRAPHNOTIFY  WM_APP + 1

LRESULT CALLBACK videowndproc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == GFXWM_GRAPHNOTIFY ) {
		long evCode, param1, param2;
		while (SUCCEEDED(g_video.pEvent->GetEvent(&evCode, &param1, &param2, 0))) {
			g_video.pEvent->FreeEventParams(evCode, param1, param2);
			switch (evCode)	{
				case EC_STREAM_CONTROL_STOPPED:
				case EC_COMPLETE:  // Fall through.
				case EC_USERABORT: // Fall through.
				case EC_ERRORABORT:	{
					g_video.bRunning=FALSE;
					return(1);
				}
			}
		} 
	}
	else if (message==WM_KEYUP) {
		if(wParam==VK_ESCAPE) {
			g_video.bRunning=FALSE;
		}
	}
	else if(message==WM_CLOSE || message==WM_QUIT)
	{
		g_video.bRunning=FALSE;
		PostQuitMessage(0);
	}

	return (DefWindowProc(hWnd, message, wParam, lParam));
}

void _VideoCreateWindow() {
	memset(&g_video.wc, 0, sizeof(g_video.wc));
	g_video.wc.cbSize			= sizeof(WNDCLASSEX);
	g_video.wc.style			= CS_HREDRAW | CS_VREDRAW;
	g_video.wc.lpfnWndProc		= videowndproc;
	g_video.wc.hInstance		= GetModuleHandle(NULL);
	g_video.wc.lpszClassName	= WIN_VID_TITLE;

	RegisterClassEx( &g_video.wc );

    // Create the application's window
    g_video.hdirectshowWnd = CreateWindow( WIN_VID_TITLE, WIN_VID_TITLE, 
                          WS_POPUP, 0, 
						  0,  g_mode.width, 
						  g_mode.height, GetDesktopWindow(), NULL, g_video.wc.hInstance, NULL );
}

void _VideoDestroyWindow() {
	DestroyWindow(g_video.hdirectshowWnd);
	UnregisterClass(WIN_VID_TITLE,g_video.wc.hInstance);
}

static HRESULT _VideoPlay()
{
	HRESULT hr;

	HWND lastwnd;

	lastwnd=GetForegroundWindow();

	if(FAILED(hr = g_video.pVidWin->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS)))
		return hr;

	if(g_mode.bFullscrn)
	{
		//ShowWindow(g_hwnd,SW_HIDE);//
		
		SetForegroundWindow(g_hwnd);
		ShowWindow(g_hwnd,SW_RESTORE);//SW_SHOW
		
		if(FAILED(hr = g_video.pVidWin->put_Top(OATRUE)))
			return hr;
	}

	g_video.bRunning=TRUE;

	if(FAILED(hr = g_video.pMediaControl->Pause()))
		return hr;

	if(FAILED(hr = g_video.pMediaControl->Run()))
		return hr;
}

/////////////////////////////////////
// Name:	GFXVideoPlay
// Purpose:	play a movie from given
//			file.
// Output:	movie played
// Return:	something
/////////////////////////////////////
s32 F_API GFXVideoPlay(HMAIN hwnd, const tCHAR *filename)
{
	HRESULT hr;
	MSG msg;

	DEVMODE lpDevMode;

	//check to see if the file exists
	hFILE fp = FSOpen(filename, L"rb"); if(fp) FSClose(fp); else { LogMsg(LOG_FILE, L"GFXVideoPlay: File does not exists", filename); return FALSE; }

	g_hwnd = hwnd;

	if(g_mode.bFullscrn)
	{
		ShowWindow(g_hwnd,SW_HIDE);

		EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &lpDevMode);
	}

	if(FAILED(hr = _VideoInit()))
	{ _VidLogError(hr, filename); LogMsg(LOG_FILE, L"GFXVideo", L"_VideoInit() failed"); goto DONE_VID; }

	_VideoCreateWindow();

	if(FAILED(hr = _VideoLoad(g_video.hdirectshowWnd, GFXWM_GRAPHNOTIFY, filename)))
	{ _VidLogError(hr, filename); LogMsg(LOG_FILE, L"GFXVideo", L"_VideoLoad() failed"); goto DONE_VID; }

	//HWND lastwnd;
	//lastwnd=GetForegroundWindow();
	if(FAILED(hr = g_video.pVidWin->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS)))
	{ _VidLogError(hr, filename); goto DONE_VID; }

	if(g_mode.bFullscrn)
	{
		MoveWindow(g_video.hdirectshowWnd, 0, 0, lpDevMode.dmPelsWidth, lpDevMode.dmPelsHeight, FALSE);

		if(FAILED(hr = g_video.pVidWin->SetWindowPosition(0, 0, lpDevMode.dmPelsWidth, lpDevMode.dmPelsHeight)))
		{ _VidLogError(hr, filename); goto DONE_VID; }

		if(FAILED(hr = g_video.pVidWin->put_FullScreenMode(OATRUE)))
		{ _VidLogError(hr, filename); goto DONE_VID; }
	}
	else
	{
		WINDOWINFO wi; GetWindowInfo(hwnd, &wi);

		MoveWindow(g_video.hdirectshowWnd, wi.rcClient.left, wi.rcClient.top, 
			wi.rcClient.right, wi.rcClient.bottom, FALSE);
	}

	ShowWindow(g_video.hdirectshowWnd,SW_SHOW);
	SetForegroundWindow(g_video.hdirectshowWnd);

	//ShowCursor(false);
	g_video.bRunning=TRUE;
	
	if(FAILED(hr = g_video.pMediaControl->Run()))
	{ _VidLogError(hr, filename); goto DONE_VID; }
	
	while(g_video.bRunning) {
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
		{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
		}
	}
	//ShowWindow(g_video.hdirectshowWnd,SW_HIDE);
	//SetForegroundWindow(lastwnd);
	//ShowCursor(true);
	hr = g_video.pMediaControl->Stop();
	LONGLONG pos=0;
	hr = g_video.pMediaSeeking->SetPositions(&pos,AM_SEEKING_AbsolutePositioning , NULL, AM_SEEKING_NoPositioning);

DONE_VID:
	_VideoUnload();
	_VideoDeInit();
	_VideoDestroyWindow();

	SetForegroundWindow(g_hwnd);
	ShowWindow(g_hwnd,SW_RESTORE);

	return TRUE;
}

#if 0
//
// Constants
//
#define KEYBOARD_SAMPLE_FREQ  100  // Sample user input on an interval
#define CUTSCENE_NAME   (L"GFX Video Player\0")

//
// Globals
//
static IGraphBuilder  *pGB = NULL;
static IMediaControl  *pMC = NULL;
static IVideoWindow   *pVW = NULL;
static IMediaEvent    *pME = NULL;

static HWND g_hwndMain=0;
static u8   g_bContinue=TRUE, g_bUserInterruptedPlayback=FALSE;


//
// Function prototypes
//
static HRESULT PlayMedia(const tCHAR *lpszMovie, HMAININST hInstance, u8 bFullscreen);
static HRESULT GetInterfaces(void);
static HRESULT SetFullscreen(void);
static void CloseApp();
static void CleanupInterfaces(void);
static u8 CreateHiddenWindow( HMAININST hInstance, const tCHAR *szFile );
static LONG WINAPI GFX_WindowProc(HWND, UINT, WPARAM, LPARAM);

/////////////////////////////////////
// Name:	GFXVideoPlay
// Purpose:	play a movie from given
//			file
// Output:	movie played
// Return:	something
/////////////////////////////////////
u32 _GFXVideoPlay(const tCHAR *szMovie, HMAININST hInstance, u8 bFullscreen)
{
    HRESULT hr;

    if (!szMovie)
        return FALSE;

    // Create the main hidden window to field keyboard input
    if (!CreateHiddenWindow(hInstance, szMovie))
        return FALSE;

    // Initialize COM
    if (FAILED(hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)))
        return FALSE;

    // Get DirectShow interfaces
    if (FAILED(hr = GetInterfaces()))
    {
        CoUninitialize();
        return hr;
    }

    // Play the movie / cutscene
    hr = PlayMedia(szMovie, hInstance, bFullscreen);

    // If the user interrupted playback and there was no other error,
    // return S_FALSE.
    if ((hr == S_OK) && g_bUserInterruptedPlayback)
        hr = S_FALSE;

    // Release DirectShow interfaces
    CleanupInterfaces();
    CoUninitialize();

    return TRUE;
}


u8 CreateHiddenWindow( HMAININST hInstance, const tCHAR *szFile )
{
    tCHAR szTitle[MAXCHARBUFF + sizeof(CUTSCENE_NAME) + 5];

    // Set up and register window class
    WNDCLASS wc      = {0};
    wc.lpfnWndProc   = (WNDPROC) GFX_WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CUTSCENE_NAME;
    if (!RegisterClass(&wc))
        return FALSE;

    // Prevent buffer overrun by restricting size of title to MAX_PATH
    wsprintf(szTitle, TEXT("%s: \0"), CUTSCENE_NAME);
    _tcsncat(szTitle, szFile, MAX_PATH);

    // Create a window of zero size that will serve as the sink for
    // keyboard input.  If this media file has a video component, then
    // a second ActiveMovie window will be displayed in which the video
    // will be rendered.  Setting keyboard focus on this application window
    // will allow the user to move the video window around the screen, make
    // it full screen, resize, center, etc. independent of the application
    // window.  If the media file has only an audio component, then this will
    // be the only window created.
    g_hwndMain = CreateWindowEx(
        0, CUTSCENE_NAME, szTitle,
        0,            // not visible
        0, 0, 0, 0,
        NULL, NULL, hInstance, NULL );

    return (g_hwndMain != NULL);
}


LONG WINAPI GFX_WindowProc( HWND hWnd, UINT message,
                        WPARAM wParam, LPARAM lParam )
{
    switch( message )
    {
        // Monitor keystrokes for manipulating video window
        // and program options
        case WM_KEYDOWN:
            switch( wParam )
            {
                case VK_ESCAPE:
                case VK_SPACE:
                case VK_RETURN:
                    g_bUserInterruptedPlayback = TRUE;
                    CloseApp();
                    break;
            }
            break;

        case WM_DESTROY:
			PostMessage(g_hwndMain, WM_CLOSE, 0, 0);
            //PostQuitMessage(0);
            return 0;
    }

    // Pass this message to the video window for notification of system changes
    if (pVW)
        pVW->NotifyOwnerMessage((LONG_PTR) hWnd, message, wParam, lParam);

    return (LONG) DefWindowProc(hWnd, message, wParam, lParam);
}


HRESULT GetInterfaces(void)
{
    HRESULT hr = S_OK;

    // Instantiate filter graph interface
    if(FAILED(CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, 
                         IID_IGraphBuilder, (void **)&pGB)))
	{ goto CLEANUP; }

    // Get interfaces to control playback & screensize
	if(FAILED(pGB->QueryInterface(IID_IMediaControl,  (void **)&pMC))) { goto CLEANUP; }
    if(FAILED(pGB->QueryInterface(IID_IVideoWindow,   (void **)&pVW))) { goto CLEANUP; }

    // Get interface to allow the app to wait for completion of playback
    if(FAILED(pGB->QueryInterface(IID_IMediaEventEx,  (void **)&pME))) { goto CLEANUP; }

    return S_OK;

    // In case of failure, the helper macro jumps here
CLEANUP:
    CleanupInterfaces();
    return(hr);
}


void CleanupInterfaces(void)
{
    // Release the DirectShow interfaces
    SAFE_RELEASE(pGB);
    SAFE_RELEASE(pMC);
    SAFE_RELEASE(pVW);
    SAFE_RELEASE(pME);

    DestroyWindow(g_hwndMain);
}


void CloseApp()
{
    // Stop playback and exit
    if (pMC)
        pMC->Stop();

    g_bContinue = FALSE;
    PostMessage(g_hwndMain, WM_CLOSE, 0, 0);
}


HRESULT PlayMedia(const tCHAR *lpszMovie, HMAININST hInstance, u8 bFullscreen)
{
    USES_CONVERSION;

    HRESULT hr = S_OK;
    tCHAR wFileName[MAXCHARBUFF];
    u8 bSleep=TRUE;

    if (!lpszMovie)
        return E_POINTER;

    // get the absolute path
	FSGetPath(lpszMovie, wFileName, MAXCHARBUFF);
    wFileName[MAXCHARBUFF - 1] = 0;

    // Allow DirectShow to create the FilterGraph for this media file
    hr = pGB->RenderFile(wFileName, NULL);
    if (FAILED(hr)) {
        LogMsg(LOG_FILE, L"PlayMedia Failed", wFileName);
        return hr;
    }

    // Set the message drain of the video window to point to our hidden
    // application window.  This allows keyboard input to be transferred
    // to our main window for processing.
    //
    // If this is an audio-only or MIDI file, then put_MessageDrain will fail.
    //
    hr = pVW->put_MessageDrain((OAHWND) g_hwndMain);
    if (FAILED(hr))
    {
		LogMsg(LOG_FILE, L"PlayMedia Failed: no video component", wFileName);
        return hr;
    }

    // Set fullscreen
	if(bFullscreen)
	{
		hr = SetFullscreen();
		if (FAILED(hr)) {
			LogMsg(LOG_FILE, L"PlayMedia Failed: unable to set fullscreen", wFileName);
			return hr;
		}
	}

    // Display first frame of the movie
    hr = pMC->Pause();
    if (FAILED(hr)) {
		LogMsg(LOG_FILE, L"PlayMedia Failed: Pause()", wFileName);
        return hr;
    }

    // Start playback
    hr = pMC->Run();
    if (FAILED(hr)) {
		LogMsg(LOG_FILE, L"PlayMedia Failed: Run()", wFileName);
        return hr;
    }

    // Update state variables
    g_bContinue = TRUE;

    // Enter a loop of checking for events and sampling keyboard input
    while (g_bContinue)
    {
        MSG msg;
        long lEventCode;
        LONG_PTR lpParam1, lpParam2;

        // Reset sleep flag
        bSleep = TRUE;

        // Has there been a media event?  Look for end of stream condition.
        if(E_ABORT != pME->GetEvent(&lEventCode, &lpParam1, 
                                    &lpParam2, 0))
        {
            // Is this the end of the movie?
            if (lEventCode == EC_COMPLETE)
            {
                g_bContinue = FALSE;
                bSleep = FALSE;
            }

            // Free the media event resources
            hr = pME->FreeEventParams(lEventCode, lpParam1, lpParam2);
            if (FAILED(hr))
            {
				LogMsg(LOG_FILE, L"PlayMedia Failed: free event params", wFileName);
            }
        }

        // Give system threads time to run (and don't sample user input madly)
        if (bSleep)
            Sleep(KEYBOARD_SAMPLE_FREQ);

        // Check and process window messages (like our keystrokes)
        while (PeekMessage (&msg, g_hwndMain, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return hr;
}


HRESULT SetFullscreen(void)
{
    HRESULT hr=S_OK;
    LONG lMode;
    static HWND hDrain=0;

    if (!pVW)
        return S_FALSE;

	DEVMODE lpDevMode;
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &lpDevMode);
	pVW->SetWindowPosition(0,0,lpDevMode.dmPelsWidth,lpDevMode.dmPelsHeight);
	pVW->put_WindowStyle(WS_POPUP);

    // Read current state
    if(FAILED(pVW->get_FullScreenMode(&lMode))) return E_FAIL;

    if (lMode == 0)  /* OAFALSE */
    {
        // Save current message drain
        if(FAILED(pVW->get_MessageDrain((OAHWND *) &hDrain))) return E_FAIL;

        // Set message drain to application main window
        if(FAILED(pVW->put_MessageDrain((OAHWND) g_hwndMain))) return E_FAIL;

        // Switch to full-screen mode
        lMode = -1;  /* OATRUE */
        if(FAILED(pVW->put_FullScreenMode(lMode))) return E_FAIL;
    }

    return hr;
}
#endif