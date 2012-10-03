#ifndef AUDIO_H
#define AUDIO_H

#include "Audio/Audio_types.h"

DLL_API void F_API SFXSetFS(hFMAIN fs);

DLL_API int F_API AudioInitSystem (void);
DLL_API int F_API AudioPlaySource (ALuint source);

DLL_API ALuint F_API AudioCreateSourceFromFile (tCHAR const * name, SDL_bool bStreaming);
DLL_API ALuint F_API AudioCreateSourceFromMemory (ALbyte * memory, SDL_bool bStreaming);

DLL_API void F_API AudioQuitSystem (void);
DLL_API void F_API AudioDestroySource (ALuint source);

DLL_API SDL_bool F_API AudioWasInit (void);
DLL_API SDL_bool F_API AudioIsSourcePlaying (ALuint source);
DLL_API SDL_bool F_API AudioIsSourceValid (ALuint source);

#endif // AUDIO_H