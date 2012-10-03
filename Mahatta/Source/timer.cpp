#include "common.h"
#include <time.h>
#include <mmsystem.h>

#pragma comment( lib, "winmm.lib" )

// Timer variables
u8 g_bUseHFTimer;
s64 g_currentTime;
f32 g_fTimeScale;

// Initialise a timer for use in our application.
// Wherever possible use a Performance timer as this
// gives the best accuracy.
// If this isn't available, fall back to the multimedia timer.
// In each case, note the number of "ticks" per second so that
// we can divide the elapsed time by this value to determine
// how much real time has actually passed.
// For more information on using these timers, see the articles at
// www.mvps.org/directx
u8 TimerInit()
{
	s64 TimerFrequency;
	g_currentTime = 0; // (rodo)

	// use the Windows multimedia timer
	g_fTimeScale = 0.001f;
	return TRUE;

	// No timer available at all (should never happen)
	return FALSE;
}

// Use the timer we previously initialised to determine
// how much time (in seconds, or a fraction thereof) has
// passed since this function was last called.
f32 TimerGetElapsed()
{
	s64 newTime;
	f32 fElapsed;

	/* truquito guarris (rodo) */
	if (g_currentTime == 0){
		g_currentTime=timeGetTime();
	}

	newTime=timeGetTime();
	
	// Scale accordingly
	fElapsed = (f32)((newTime - g_currentTime) * g_fTimeScale);

	// Save the new time value for the next time we're called
	g_currentTime = newTime;

	return fElapsed;
}

u32 g_timer = 0;

u32 g_tickerFPS = 0;

u32   g_fps = 0, g_oldfps = 0;

void TimerFPSBegin()
{
	g_tickerFPS = GetTickCount();
}

u32 TimerFPSEnd()
{
	if(g_tickerFPS - 1000 > g_timer)
	{
		g_timer = g_tickerFPS;
	
		g_oldfps = g_fps;
		g_fps = 0;
	}
	else
		g_fps++;

	return g_oldfps;
}