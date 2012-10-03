/// @file
/// 

#include "common.h"
#include "Std/filesys.h"

#include "Audio_Imp.h"
#include "OGG_Imp.h"
#include "Audio/Audio.h"

/// @brief Installs the audio library's file system
/// @param fs File system to install
/// @note Tested
void F_API SFXSetFS (hFMAIN fs)
{
	FSMainSet(fs);
}

/// @brief Prepares the audio manager for use
/// @return 0 on failure, non-0 for success
/// @note Tested
int F_API AudioInitSystem (void)
{
	Audio::Main & audio = Audio::Main::Get();

	////////////////////////////////////////////////
	// Flush any information present in the system.
	////////////////////////////////////////////////
	if (audio.mInitialized) AudioQuitSystem();

	/////////////////////////////////////////////////////////////////////
	// We install an OpenAL context, using the best available 3D device. 
	/////////////////////////////////////////////////////////////////////
	if ((audio.mDevice = alcOpenDevice((ALCubyte*)"DSound3D")) == 0) return 0;
	if ((audio.mContext = alcCreateContext(audio.mDevice, 0)) == 0) return 0;

	alcMakeContextCurrent(audio.mContext);

	//////////////////////////////////////////////////////
	// We create some dummy keys used to perform lookups.
	//////////////////////////////////////////////////////
	audio.mSourceKey = new Audio::Source(AL_NONE);
	audio.mBufferFileKey = new Audio::BufferFile(L"");

	////////////////////////////////////////////////////
	// Indicate that the audio system is ready for use.
	////////////////////////////////////////////////////
	audio.mInitialized = true;	

	return 1;
}

/// @brief Updates and plays a source
/// @param source OpenAL source to play
/// @return 0 on failure, non-0 for success
/// @note Tested
int F_API AudioPlaySource (ALuint source)
{
	if (!AudioIsSourceValid(source)) return 0;

	////////////////////////////////////////////////////////
	// The source may be a stream, so we perform an update.
	////////////////////////////////////////////////////////
	try {
		(*Audio::Main::Get().FindSource(source))->Update();
	} catch (ALuint) {
		return 0;
	}

	/////////////////////////////////////////////////////////
	// If the source is not already in mid-play, we play it.
	/////////////////////////////////////////////////////////
	if (!AudioIsSourcePlaying(source)) alSourcePlay(source);

	return 1;
}

/// @brief Creates a source, given an audio file
/// @param name Name of audio file
/// @param bStreaming Indicates whether the source should be streamed
/// @return OpenAL source used to refer to source and control its parameters
/// @note Tested, but incomplete
ALuint F_API AudioCreateSourceFromFile (tCHAR const * name, SDL_bool bStreaming)
{
	////////////////////////////////////////
	// We first generate the OpenAL source.
	////////////////////////////////////////
	ALuint source;

	alGetError();
	alGenSources(1, &source);

	if (!Audio::alCheck()) return AL_NONE;

	////////////////////////////////////////////////////////////////////////////
	// We determine what type of file we have, and call the appropriate loader.
	////////////////////////////////////////////////////////////////////////////
	try {
	//	switch (Audio::FileType(name))
	//	{
	//	case Audio::eFile_Ogg:
			Audio::Main::Get().Ogg_LoadFromFile(source, name, bStreaming != SDL_FALSE);
	//	break;
	//	default:
	//	break;
	//	}
	} catch (std::bad_alloc &) {
		alDeleteSources(1, &source);
		source = AL_NONE;
	}

	return source;
}

/// @brief
/// @param memory
/// @param bStreaming
/// @return
/// @note
ALuint F_API AudioCreateSourceFromMemory (ALbyte * memory, SDL_bool bStreaming)
{
	return AL_NONE;
}

/// @brief Shuts down the audio manager, restoring it to its original state; also,
///		   performs garbage collection on all sources and buffers
/// @note Tested
void F_API AudioQuitSystem (void)
{
	Audio::Main & audio = Audio::Main::Get();

	///////////////////////////////////////////////
	// We clear all internal data from the system.
	///////////////////////////////////////////////
	audio.Empty();

	//////////////////////////////////
	// We close down our OpenAL data.
	//////////////////////////////////
	if (audio.mContext != 0)
	{
		alcMakeContextCurrent(0);
		alcDestroyContext(audio.mContext);

		audio.mContext = 0;
	}

	if (audio.mDevice != 0)
	{
		alcCloseDevice(audio.mDevice);

		audio.mDevice = 0;
	}

	/////////////////////////////////////////////////////
	// Prevent use of the system without initialization.
	/////////////////////////////////////////////////////
	audio.mInitialized = false;
}

/// @brief Destroys a source
/// @param sound OpenAL source to destroy
/// @note Tested
void F_API AudioDestroySource (ALuint source)
{
	if (!AudioIsSourceValid(source)) return;

	////////////////////////////////////////////////////////////////////////////////////
	// OpenAL can be safely used outside the library, in which case the source may have
	// been deleted manually; thus, we need to check whether it still exists. If so, we
	// detach any buffers queued into it, stop it, and delete it properly.
	////////////////////////////////////////////////////////////////////////////////////
	if (alIsSource(source))
	{
		ALint nQueued;	alGetSourcei(source, AL_BUFFERS_QUEUED, &nQueued);

		ALuint buffer;	while (nQueued-- != 0) alSourceUnqueueBuffers(source, 1, &buffer);

		alSourceStop(source);
		alDeleteSources(1, &source);
	}

	/////////////////////////////////////////////////////////////
	// We now remove the associated data from the audio manager.
	/////////////////////////////////////////////////////////////
	Audio::Main::Get().UnmapSource(source);
}

/// @brief Indicates whether the audio manager was initialized
/// @return A boolean indicating whether initialization occurred
/// @note Tested
SDL_bool F_API AudioWasInit (void)
{
	return Audio::Main::Get().mInitialized ? SDL_TRUE : SDL_FALSE;
}

/// @brief Indicates whether a source is being played
/// @param source OpenAL source to test
/// @return A boolean indicating whether the source is playing
/// @note Tested
SDL_bool F_API AudioIsSourcePlaying (ALuint source)
{
	if (!AudioIsSourceValid(source)) return SDL_FALSE;
	if (!alIsSource(source)) return SDL_FALSE;

	ALint state;	alGetSourcei(source, AL_SOURCE_STATE, &state);
    
	return AL_PLAYING == state ? SDL_TRUE : SDL_FALSE;
}

/// @brief Indicates whether an OpenAL source corresponds to a valid source
/// @param source OpenAL source to test
/// @return A boolean indicating whether the source is valid
/// @note Tested
SDL_bool F_API AudioIsSourceValid (ALuint source)
{
	if (!AudioWasInit()) return SDL_FALSE;

	Audio::Main & audio = Audio::Main::Get();

	return audio.FindSource(source) != audio.mSources.end() ? SDL_TRUE : SDL_FALSE;
}