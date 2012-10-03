#ifndef AUDIO_DYN_H
#define AUDIO_DYN_H

typedef struct
{
    void           *module;

	void (F_API * SFXSetFS) (hFMAIN fs);

	int (F_API * AudioInitSystem)(void);
	int (F_API * AudioPlaySource)(ALuint source);

	ALuint (F_API * AudioCreateSourceFromFile)(tCHAR const * name, SDL_bool bStreaming);
	ALuint (F_API * AudioCreateSourceFromMemory)(ALbyte * memory, SDL_bool bStreaming);

	void (F_API * AudioQuitSystem)(void);
	void (F_API * AudioDestroySource)(ALuint source);

	SDL_bool (F_API * AudioWasInit)(void);
	SDL_bool (F_API * AudioIsSourcePlaying)(ALuint source);
	SDL_bool (F_API * AudioIsSourceValid)(ALuint source);
} AUDIO_INSTANCE;

static AUDIO_INSTANCE * AudioCreateInstance (LPCSTR dllName)
{
    AUDIO_INSTANCE * instance;

    instance = (AUDIO_INSTANCE *)calloc(sizeof(AUDIO_INSTANCE), 1);
    if (!instance)
    {
        return NULL;
    }

#ifdef WIN32
    instance->module = LoadLibrary(dllName);
#else
    instance->module = dlopen(dllName, RTLD_GLOBAL | RTLD_LAZY);
#endif
    if (!instance->module)
    {
        free(instance);
        return NULL;
    }

	MOD_GETPROC(SFXSetFS, "SFXSetFS");

	MOD_GETPROC(AudioInitSystem, "AudioInitSystem");
	MOD_GETPROC(AudioPlaySource, "AudioPlaySource");
	MOD_GETPROC(AudioCreateSourceFromFile, "AudioCreateSourceFromFile");
	MOD_GETPROC(AudioCreateSourceFromMemory, "AudioCreateSourceFromMemory");
	MOD_GETPROC(AudioQuitSystem, "AudioQuitSystem");
	MOD_GETPROC(AudioDestroySource, "AudioDestroySource");
	MOD_GETPROC(AudioWasInit, "AudioWasInit");
	MOD_GETPROC(AudioIsSourcePlaying, "AudioIsSourcePlaying");
	MOD_GETPROC(AudioIsSourceValid, "AudioIsSourceValid");

	return instance;
}

static void AudioFreeInstance(AUDIO_INSTANCE * instance)
{
    if (instance)
    {
        if (instance->module)
        {
#ifdef WIN32
            FreeLibrary((HMODULE)instance->module);
#else
            dlclose(instance->module);
#endif
        }
        free(instance);
    }
}

#endif // AUDIO_DYN_H