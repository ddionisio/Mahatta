#include "engine_common.h"
#include <bitset>
#include <set>
#include <AL/al.h>
#include <cassert>

// temp - REMOVE ME!!!
#pragma comment(lib, "OpenAL32")
// temp - REMOVE ME!!!

/// @brief Structure used to maintain a template for sound loading
struct _STmp {
// Enumerations
	enum eStr {
		eStrFile,	///< File string parameter
		eStrCount	///< Count of string parameters
	};
	enum eInt {
		eIntVolume,	///< Volume int parameter
		eIntCount	///< Count of int parameters
	};
	enum eFloat {
		eFloatRefDist,	///< Reference distance float parameter
		eFloatRollOff,	///< Rolloff factor float parameter
		eFloatMaxDist,	///< Maximum distance float parameter
		eFloatCount	///< Count of float parameters
	};
	enum eFlag {
		eFlagLocal,	///< Local flag parameter
		eFlagLooping,	///< Looping flag parameter
		eFlagNoCull,///< Culling flag parameter
		eFlagPreCache,	///< Precache parameter
		eFlagCount	///< Count of flag parameters
	};
// Members					
	std::bitset<8> mStrBits;///< Bits indicating loaded string parameters
	std::bitset<8> mIntBits;///< Bits indicating loaded int parameters
	std::bitset<8> mFloatBits;	///< Bits indicating loaded vfloat parameters
	std::bitset<8> mFlagBits;	///< Bits indicating loaded flag parameters
	std::wstring mName;	///< Name associated with sound
	std::wstring mStrs[eStrCount];	///< String parameters
	u32 mInts[eIntCount];	///< Int parameters
	f32 mFloats[eFloatCount];	///< Float parameters
	s32 mID;///< ID bound to name
// Methods
	/// @brief Copy constructs a _STmp object
	/// @param T Reference to a _STmp object
	_STmp (const _STmp & T)
	{
		assert(!mName.empty() || mID != -1);
	
		mName = T.mName;
		mID = T.mID;
		mStrBits = T.mStrBits;
		mIntBits = T.mIntBits;
		mFloatBits = T.mFloatBits;
		mFlagBits = T.mFlagBits;

		for (Uint32 index = 0; index < eStrCount; ++index)
		{
			mStrs[index] = mStrBits[index] ? T.mStrs[index] : L"";
		}

		for (Uint32 index = 0; index < eIntCount; ++index)
		{
			mInts[index] = mIntBits[index] ? T.mInts[index] : 0;
		}

		for (Uint32 index = 0; index < eFloatCount; ++index)
		{
			mFloats[index] = mFloatBits[index] ? T.mFloats[index] : 0.0f;
		}
	}

	/// @brief Constructs a _STmp object, assigning to it an ID
	/// @param ID ID to assign to sound template
	_STmp (s32 ID) : mID(ID)
	{
	}

	/// @brief Constructs a _STmp object, assigning to it a name
	/// @param name Name to assign to sound template
	_STmp (std::wstring const & name) : mName(name), mID(-1)
	{
	}
	
	/// @brief Assigns a string parameter
	/// @param which Index of parameter
	/// @param Str String to assign
	/// @return A boolean indicating whether the parameter was already set
	bool SetString (eStr which, std::wstring const & Str)
	{
		if (mStrBits[which])
		{
			LogMsg(LOG_FILE, L"SetString", L"Field reassignment");

			return true;
		}

		mStrs[which] = Str;
		mStrBits[which] = true;

		return false;
	}

	/// @brief Assigns an int parameter
	/// @param which Index of parameter
	/// @param Int Int to assign
	/// @return A boolean indicating whether the parameter was already set
	bool SetInt (eInt which, std::wstring const & Int)
	{
		if (mIntBits[which])
		{
			LogMsg(LOG_FILE, L"SetInt", L"Field reassignment");

			return true;
		}

		mInts[which] = _wtoi(Int.c_str());
		mIntBits[which] = true;

		return false;
	}

	/// @brief Assigns a float parameter
	/// @param which Index of parameter
	/// @param Float Float to assign
	/// @return A boolean indicating whether the parameter was already set
	bool SetFloat (eFloat which, std::wstring const & Float)
	{
		if (mFloatBits[which])
		{
			LogMsg(LOG_FILE, L"SetFloat", L"Field reassignment");

			return true;
		}

		mFloats[which] = static_cast<f32>(_wtof(Float.c_str()));
		mFloatBits[which] = true;

		return false;
	}

	/// @brief Assigns a flag parameter
	/// @param which Index of parameter
	/// @return A boolean indicating whether the parameter was already set
	bool SetFlag (eFlag which)
	{
		if (mFlagBits[which])
		{
			LogMsg(LOG_FILE, L"SetFlag", L"Field reassignment");

			return true;
		}

		mFlagBits[which] = true;

		return false;
	}

	s32 GetID (void);
};

/// @brief Functor used to order a sound template container
struct STmp_Less {
// Methods
	bool operator () (_STmp const & _L, _STmp const & _R) const
	{
		/////////////////////////////////////////////////////////////////////////////////
		// We may iterate through the sound templates in one of two ways. At first, when
		// we are adding them to their container, they are sorted lexicographically by
		// name. After each has been assigned a name, they will also have valid IDs. The
		// numerical order of the IDs corresponds to the lexicographic name ordering, so
		// we favor an ID iteration if valid IDs are present in each sound template.
		/////////////////////////////////////////////////////////////////////////////////
		bool bTestIDs = _L.mID != -1 && _R.mID != -1;

		if (bTestIDs) return _L.mID < _R.mID;

		assert(!_L.mName.empty() && !_R.mName.empty());

		return _L.mName < _R.mName;
	}
};

typedef std::set<_STmp, STmp_Less> NameMap;
typedef std::map<s32, ALuint> SoundCtr;

/// @brief Mapping of sound names to templates
NameMap g_NameMap;
/// @brief Container of sound channels to state structures
SoundCtr g_Sounds;

/// @brief Music source
ALuint g_Music;
/// @brief Old music source
ALuint g_OldMusic;

/// @brief Current sound volume
u8 g_SoundVol;
/// @brief Current music volume
u8 g_MusicVol;

/// @brief Gets a new ID for a sound
/// @return New ID to assign
s32 _STmp::GetID (void)
{
	assert(mID != -1);

	//////////////////////////////////////////////////////////////////////////////////////
	// We build sound IDs out of the template's ID and an index, striving for as small an
	// index as possible. We first get the range of sounds whose IDs contain the template
	// ID in their upper bits. If the range is empty, or the range's first item is not at
	// the beginning, we use the first slot. Otherwise, we iterate through the range, and
	// look for gaps between the previous and current entries. If we find a gap, we use
	// the first slot in that gap. If all else fails, we use the first slot after the
	// range. Note that we assume we will not wander into the next template's slots.
	//////////////////////////////////////////////////////////////////////////////////////
	s32 cur = mID << 16;

	SoundCtr::iterator siL = g_Sounds.lower_bound(cur);
	SoundCtr::iterator siU = g_Sounds.upper_bound(cur + (1 << 16));

	if (siL == siU || cur < siL->first) return cur;

	while (siL != siU)
	{
		s32 diff = (siL++)->first - cur;

		if (diff > 1) break;

		cur += diff;
	}

	return cur + 1;
}

/////////////////////////////////////
// Name:	EngineSoundSetVolume
// Purpose:	set the sound volume
// Output:	master sound volume set
// Return:	none
/////////////////////////////////////
void EngineSoundSetVolume(u8 vol)
{
	g_SoundVol = vol;
}

/////////////////////////////////////
// Name:	EngineSoundGetVolume
// Purpose:	get the sound volume
// Output:	none
// Return:	the volume
/////////////////////////////////////
u8 EngineSoundGetVolume()
{
	return g_SoundVol;
}

/////////////////////////////////////
// Name:	EngineSoundSysLoad
// Purpose:	load sound library
//			the file may contain several
//			sounds. This loads all the 
//			resources of sounds in the 
//			library.
// Output:	system loaded
// Return:	TRUE if success
/////////////////////////////////////
s32 EngineSoundSysLoad(const tCHAR *filename)
{
	///////////////////////////////////////////////////////////////////
	// We prime the audio manager and set some default control values.
	///////////////////////////////////////////////////////////////////
	if (g_SFX->AudioInitSystem() == 0) return 0;

	g_Music = AL_NONE;
	g_OldMusic = AL_NONE;

	g_SoundVol = 255;
	g_MusicVol = 255;

	///////////////////////////////////////////////////////////////////
	// We read our sound configuration file and load all its sections.
	///////////////////////////////////////////////////////////////////
	hSHADERF shdF = ShaderFileLoad(filename);

	if(shdF)
	{
		u32 count = shdF->SectionGetNum();

		for (u32 index = 0; index < count; ++index)
		{
			bool bError = false;

			////////////////////////////////////////////////////////////////////////////
			// We generate a sound template for the current section and proceed through
			// any and all of its variables, loading each according to its type.
			////////////////////////////////////////////////////////////////////////////
			std::wstring name = shdF->SectionGetName(index);

			_STmp sTmp(name);

			u32 varCount = shdF->SectionGetVarNum(index);

			for (u32 var = 0; var < varCount; ++var)
			{
				std::wstring varName = shdF->VarGetName(index, var);
				std::wstring paramName = shdF->VarGetParam(index, var);

				if (varName == L"file") bError |= sTmp.SetString(_STmp::eStrFile, paramName);

				else if (varName == L"volume") bError |= sTmp.SetInt(_STmp::eIntVolume, paramName);

				else if (varName == L"local") bError |= sTmp.SetFlag(_STmp::eFlagLocal);
				else if (varName == L"looping") bError |= sTmp.SetFlag(_STmp::eFlagLooping);
				else if (varName == L"no_cull") bError |= sTmp.SetFlag(_STmp::eFlagNoCull);
				else if (varName == L"precache") bError |= sTmp.SetFlag(_STmp::eFlagPreCache);

				else if (varName == L"refDist") bError |= sTmp.SetFloat(_STmp::eFloatRefDist, paramName);
				else if (varName == L"rollOff") bError |= sTmp.SetFloat(_STmp::eFloatRollOff, paramName);
				else if (varName == L"maxDist") bError |= sTmp.SetFloat(_STmp::eFloatMaxDist, paramName);

				else bError = true;
			}

			////////////////////////////////////////////////////////////////////////////
			// We log any errors for this section. If none occur, we load the template.
			////////////////////////////////////////////////////////////////////////////
			if (bError || !sTmp.mStrBits[_STmp::eStrFile])
			{
				std::wstring error(L"Error loading sound: ");
		
				LogMsg(LOG_FILE, L"EngineSoundSysLoad", (error + name).c_str());
			}

			else
			{
				g_NameMap.insert(sTmp);

				////////////////////////////////////////////////////////////////////////
				// If precaching is desired, we create a source from the provided file.
				// We do not need to keep a handle to this source, as we will not be
				// directly using it, and it will be cleaned up when we close down.
				////////////////////////////////////////////////////////////////////////
				if (sTmp.mFlagBits[_STmp::eFlagPreCache])
				{
					g_SFX->AudioCreateSourceFromFile(sTmp.mStrs[_STmp::eStrFile].c_str(), SDL_FALSE);
				}
			}
		}

		shdF->Release();

		///////////////////////////////////////////////////////////////////////
		// Once all valid templates are loaded, we alias their names with IDs.
		///////////////////////////////////////////////////////////////////////
		NameMap::iterator ni = g_NameMap.begin();

		for (s32 ID = 0; ni != g_NameMap.end(); ++ni) ni->mID = ID++;
	}
	
	return TRUE;
}

/////////////////////////////////////
// Name:	EngineSoundSysDestroy
// Purpose:	destroy the sound library,
//			this will also destroy all
//			instances of sounds
// Output:	everything destroyed!
// Return:	none
/////////////////////////////////////
void EngineSoundSysDestroy()
{
	/////////////////////////////////////////////////////////
	// We need only clear out the name and sound containers.
	/////////////////////////////////////////////////////////
	g_NameMap.clear();
	g_Sounds.clear();

	////////////////////////////////////////////////////////////////////
	// We shut down the audio manager, clearing out all active sources.
	////////////////////////////////////////////////////////////////////
	g_SFX->AudioQuitSystem();
}

/////////////////////////////////////
// Name:	EngineSoundSysGetID
// Purpose:	get the index of a sound in
//			the library with given name
//			<name> { ... }
// Output:	sndSysIDOut is set
// Return:	FALSE if given name does not
//			exist.
/////////////////////////////////////
u8 EngineSoundSysGetID(const tCHAR *name, s32 *sndSysIDOut)
{
	NameMap::iterator iter = g_NameMap.find(_STmp(name));

	if (iter == g_NameMap.end()) return FALSE;

	*sndSysIDOut = iter->mID;
	
	return TRUE;
}

/////////////////////////////////////
// Name:	EngineSoundIsValid
// Purpose:	check to see if given sndID
//			is still valid, o.w. it must've
//			expired already.
// Output:	none
// Return:	TRUE if sndID is valid and
//			still kickin'
/////////////////////////////////////
u8 EngineSoundIsValid(s32 sndID)
{
	return g_Sounds.find(sndID) != g_Sounds.end() ? TRUE : FALSE;
}

/////////////////////////////////////
// Name:	EngineSoundPlay
// Purpose:	emits the given sndSysID (from
//			EngineSoundSysGetID).
// Output:	a channel is chosen for the sysID
// Return:	the sndID (instance of sndSysID)
/////////////////////////////////////
s32 EngineSoundPlay(s32 sndSysID, const Vec3D & pos)
{
	if (-1 == sndSysID) return -1;

	NameMap::iterator iter = g_NameMap.find(_STmp(sndSysID));

	if (iter == g_NameMap.end()) return -1;

	//////////////////////////////////////////////////////////////////////
	// We create a new source, get a new ID, and plug it into the sounds.
	//////////////////////////////////////////////////////////////////////
	ALuint source = g_SFX->AudioCreateSourceFromFile(iter->mStrs[_STmp::eStrFile].c_str(), SDL_FALSE);

	if (AL_NONE == source) return -1;

	s32 sID = iter->GetID();

	g_Sounds[sID] = source;

	/////////////////////////////////////////////////////////////////////////////
	// We configure the source properties from the template and play the source.
	/////////////////////////////////////////////////////////////////////////////
	if (iter->mIntBits[_STmp::eIntVolume]) alSourcef(source, AL_GAIN, f32(iter->mInts[_STmp::eIntVolume]) / 255);
	if (iter->mFloatBits[_STmp::eFloatRefDist]) alSourcef(source, AL_REFERENCE_DISTANCE, iter->mFloats[_STmp::eFloatRefDist]);
	if (iter->mFloatBits[_STmp::eFloatRollOff]) alSourcef(source, AL_ROLLOFF_FACTOR, iter->mFloats[_STmp::eFloatRollOff]);
	if (iter->mFloatBits[_STmp::eFloatMaxDist]) alSourcef(source, AL_MAX_DISTANCE, iter->mFloats[_STmp::eFloatMaxDist]);
	if (iter->mFlagBits[_STmp::eFlagLocal]) alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);
	if (iter->mFlagBits[_STmp::eFlagLooping]) alSourcei(source, AL_LOOPING, AL_TRUE);

	alSource3f(source, AL_POSITION, pos.x, pos.y, pos.z);

	g_SFX->AudioPlaySource(source);

	return sID;
}

/////////////////////////////////////
// Name:	EngineSoundSet
// Purpose:	update given sndID (from
//			EngineSoundPlay)
// Output:	given sndID updated
// Return:	TRUE if success.
/////////////////////////////////////
u8 EngineSoundSet(s32 sndID, const Vec3D & pos, const Vec3D & vel)
{
	if (!EngineSoundIsValid(sndID)) return FALSE;

	ALuint source = g_Sounds[sndID];

	alSource3f(source, AL_POSITION, pos.x, pos.y, pos.z);
	alSource3f(source, AL_VELOCITY, vel.x, vel.y, vel.z);

	return TRUE;
}

/////////////////////////////////////
// Name:	EngineSoundPause
// Purpose:	(un)pause given sndID
// Output:	snd paused
// Return:	TRUE if success
/////////////////////////////////////
u8 EngineSoundPause(s32 sndID, u8 bPause)
{
	if (!EngineSoundIsValid(sndID)) return FALSE;

	ALuint source = g_Sounds[sndID];

	SDL_bool bPlaying = g_SFX->AudioIsSourcePlaying(source);

	if (bPause)
	{
		if (bPlaying) alSourcePause(source);
	}

	else
	{
		if (!bPlaying) alSourcePlay(source);
	}

	return TRUE;
}

/////////////////////////////////////
// Name:	EngineSoundIsPause
// Purpose:	is given sound paused?
// Output:	none
// Return:	TRUE if snd is paused.
/////////////////////////////////////
u8 EngineSoundIsPause(s32 sndID)
{
	if (!EngineSoundIsValid(sndID)) return FALSE;

	return !g_SFX->AudioIsSourcePlaying(g_Sounds[sndID]) ? TRUE : FALSE;
}

/////////////////////////////////////
// Name:	EngineSoundStop
// Purpose:	stop given sndID, this will
//			invalidate sndID as well.
// Output:	sndID invalidated
// Return:	TRUE if success
/////////////////////////////////////
u8 EngineSoundStop(s32 sndID)
{
	if (!EngineSoundIsValid(sndID)) return FALSE;

	g_SFX->AudioDestroySource(g_Sounds[sndID]);

	g_Sounds.erase(sndID);

	return TRUE;
}

/////////////////////////////////////
// Name:	EngineSoundStopAll
// Purpose:	invalidate all sounds playing
// Output:	silence.
// Return:	TRUE if success
/////////////////////////////////////
u8 EngineSoundStopAll()
{
	for (SoundCtr::iterator iter = g_Sounds.begin(); iter != g_Sounds.end(); ++iter)
	{
		g_SFX->AudioDestroySource(iter->second);
	}

	g_Sounds.clear();

	return TRUE;
}

/////////////////////////////////////
// Name:	EngineSoundSetListener
// Purpose:	update listener's
//			3d sound position
// Output:	none
// Return:	TRUE if success
/////////////////////////////////////
u8 EngineSoundSetListener(const Vec3D &pos, const Vec3D &vel, const Vec3D &fwd, const Vec3D &top)
{
	alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
	alListener3f(AL_VELOCITY, vel.x, vel.y, vel.z);

	Vec3D rgt;	Vec3DCross(&fwd, &top, &rgt);
	Vec3D upV;	Vec3DCross(&rgt, &fwd, &upV);

	ALfloat fOri[] = { fwd.x, fwd.y, fwd.z, upV.x, upV.y, upV.z };

	alListenerfv(AL_ORIENTATION, fOri);

	return TRUE;
}

/////////////////////////////////////
// Name:	EngineSoundUpdate
// Purpose:	update the 3d sound
//			system
// Output:	none
// Return:	TRUE if success
/////////////////////////////////////
u8 EngineSoundUpdate()
{
	////////////////////////////////////////////////////////////////////////////////////
	// If there is old music, we update it. We first check whether its fadeout time has
	// elapsed. If so, we remove and nullify it. Otherwise, we attenuate the volume of
	// the music according to the fadeout time, and play the old music.
	////////////////////////////////////////////////////////////////////////////////////
	if (g_OldMusic != AL_NONE)
	{
		// if time up
			// g_SFX->AudioDestroySource(g_OldMusic);
			// g_OldMusic = AL_NONE;

		// else
			// set attenuated gain (g_MusicVol)

			g_SFX->AudioPlaySource(g_OldMusic);
	}

	//////////////////////////////////////////////////////////////////////////
	// If there is normal music, we set the current music volume and play it.
	//////////////////////////////////////////////////////////////////////////
	if (g_Music != AL_NONE)
	{
		alSourcef(g_Music, AL_GAIN, f32(g_MusicVol) / 255);

		g_SFX->AudioPlaySource(g_Music);
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// We iterate through the sounds that are playing, and flush any stopped ones. If one
	// is still playing, we update its playback volume.
	//////////////////////////////////////////////////////////////////////////////////////
	s32 ID = -1;

	for (SoundCtr::iterator iter = g_Sounds.begin(); iter != g_Sounds.end(); )
	{
		ALint state;	alGetSourcei(iter->second, AL_SOURCE_STATE, &state);

		if (AL_STOPPED == state)
		{
			g_SFX->AudioDestroySource(iter->second);

			iter = g_Sounds.erase(iter);
		}

		else
		{
			f32 fVol = f32(g_SoundVol) / 255;

			s32 cur = iter->first >> 16;

			if (cur != ID)
			{
				NameMap::iterator ni = g_NameMap.find(cur);

				if (ni->mIntBits[_STmp::eIntVolume])
				{
					fVol *= ni->mInts[_STmp::eIntVolume];
				}

				ID = cur;
			}

			alSourcef(iter->second, AL_GAIN, fVol);

			++iter;
		}
	}

	return TRUE;
}

//
// Music
//

/////////////////////////////////////
// Name:	EngineMusicGetVolume
// Purpose:	get the music volume
// Output:	none
// Return:	the volume
/////////////////////////////////////
u8 EngineMusicGetVolume()
{
	return g_MusicVol;
}

/////////////////////////////////////
// Name:	EngineMusicSetVolume
// Purpose:	set the music volume
// Output:	none
// Return:	none
/////////////////////////////////////
void EngineMusicSetVolume(u8 vol)
{
	g_MusicVol = vol;
}

/////////////////////////////////////
// Name:	EngineMusicPlay
// Purpose:	play a music from file
// Output:	music played
// Return:	TRUE if success
/////////////////////////////////////
u8 EngineMusicPlay(const tCHAR *filename)
{
	g_Music = g_SFX->AudioCreateSourceFromFile(filename, SDL_TRUE);

	alSourcei(g_Music, AL_SOURCE_RELATIVE, AL_TRUE);

	return g_Music != AL_NONE;
}

/////////////////////////////////////
// Name:	EngineMusicStop
// Purpose:	stop current music played
// Output:	music is stopped & destroyed
// Return:	none
/////////////////////////////////////
void EngineMusicStop(f32 fadeOutTime)
{
	g_OldMusic = g_Music;

	// Setup timer

	g_Music = AL_NONE;
}