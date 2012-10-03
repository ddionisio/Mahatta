#ifndef _engineTimer_h
#define _engineTimer_h

// Initialise a timer for use in our application.
// Wherever possible use a Performance timer as this
// gives the best accuracy.
// If this isn't available, fall back to the multimedia timer.
// In each case, note the number of "ticks" per second so that
// we can divide the elapsed time by this value to determine
// how much real time has actually passed.
// For more information on using these timers, see the articles at
// www.mvps.org/directx
u8 TimerInit();

// Use the timer we previously initialised to determine
// how much time (in seconds, or a fraction thereof) has
// passed since this function was last called.
f32 TimerGetElapsed();

void TimerFPSBegin();
u32 TimerFPSEnd();

#endif