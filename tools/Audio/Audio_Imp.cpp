/// @file
///

#include "common.h"
#include "Std/filesys.h"

#include "Audio_Imp.h"
#include "Audio/Audio.h"
#include <iostream>

namespace Audio 
{
	/// @var G_Main
	/// @brief Audio manager singleton
	Main G_Main;

	/// @brief Constructs a Source object
	/// @param source OpenAL source to bind
	/// @note Tested
	Source::Source (ALuint source) : mSource(source)
	{
	}

	/// @brief Destructs a Source object
	/// @note Tested
	Source::~Source (void)
	{
		if (alIsSource(mSource)) alSourcei(mSource, AL_BUFFER, AL_NONE);
	}

	/// @brief Clears a Source object's resources
	/// @note Tested
	void Source::Clear (void)
	{
		if (alIsSource(mSource))
		{
			alSourcei(mSource, AL_BUFFER, AL_NONE);

			mSource = AL_NONE;
		}
	}

	/// @brief Constructs a Buffer object
	/// @note Tested
	Buffer::Buffer (void) : mBuffer(AL_NONE), mRefCount(0)
	{
		alGenBuffers(1, &mBuffer);
	}

	/// @brief Destructs a Buffer object
	/// @note Tested
	Buffer::~Buffer (void)
	{
		alDeleteBuffers(1, &mBuffer);
	}

	/// @brief Unreferences a Buffer object, removing it if its reference count goes to 0
	/// @note Tested
	void Buffer::Release (void)
	{
		if (0 == --mRefCount) Remove();
	}

	/// @brief Constructs a Clip object
	/// @param source OpenAL source to bind
	/// @note Tested
	Clip::Clip (ALuint source) : Source(source), mBufferData(0)
	{
	}

	/// @brief Destructs a Clip object
	/// @note Tested
	Clip::~Clip (void)
	{
		if (mBufferData != 0) mBufferData->Release();
	}

	/// @brief Clears a Clip object's resources
	/// @note Tested
	void Clip::Clear (void)
	{
		if (mBufferData != 0)
		{
			mBufferData->Release();

			mBufferData = 0;
		}
	}

	/// @brief Constructs a Stream object
	/// @param source OpenAL source to bind
	/// @param count Count of OpenAL buffers to use for streaming
	/// @param size Size, in bytes, of each buffer
	/// @note Tested
	Stream::Stream (ALuint source, Uint32 count, Sint32 size) : Source(source), mBuffers(0), mData(0), mCount(count), mSize(size)
	{
		try {
			/////////////////////////////////////////////////////////////////////////////
			// We create an OpenAL buffer array. To prepare for possible error-handling,
			// we initialize each member to a null buffer value.
			/////////////////////////////////////////////////////////////////////////////
			mBuffers = new ALuint[mCount];

			for (Uint32 index = 0; index < mCount; ++index) mBuffers[index] = AL_NONE;

			///////////////////////////////////////////////////////////////
			// We create some scratch memory used to store streamed audio.
			///////////////////////////////////////////////////////////////
			mData = new char[mSize];

			///////////////////////////////////////////////////////
			// We generate our OpenAL buffers in the buffer array.
			///////////////////////////////////////////////////////
			alGetError();
			alGenBuffers(mCount, mBuffers);

			if (!alCheck()) throw AL_NONE;
		} catch (...) {
			Clear();

			throw std::bad_alloc("Stream");
		}
	}

	/// @brief Destructs a Stream object
	/// @note Tested
	Stream::~Stream (void)
	{
		if (mBuffers != 0) alDeleteBuffers(mCount, mBuffers);

		delete mBuffers;
		delete mData;
	}
	
	/// @brief Prepares the buffers of a Stream object after construction
	/// @note Tested
	void Stream::PrepareBuffers (void)
	{
		//////////////////////////////////////////////////////////////////////////////
		// We fill each of the buffers in order. If we bound a source, we queue these 
		// buffers into the source. If no reads fail, we indicate success.
		//////////////////////////////////////////////////////////////////////////////
		try {
			for (Uint32 index = 0; index < mCount; ++index) Read(mBuffers[index], true);

			if (mSource != AL_NONE) alSourceQueueBuffers(mSource, mCount, mBuffers);

			mStatus[eBuffersLoaded] = true;
		} catch (ALuint) {}
	}

	/// @brief Clears a Stream object's resources
	/// @note Tested
	void Stream::Clear (void)
	{
		Source::Clear();

		if (mBuffers != 0) alDeleteBuffers(mCount, mBuffers);

		delete mBuffers;
		delete mData;

		mBuffers = 0;
		mData = 0;
	}

	/// @brief Performs a periodic update on a Stream object
	/// @note Tested
	void Stream::Update (void)
	{
		////////////////////////////////////////////////////////////////////////////////
		// We find out how many of the source's buffers have already been processed. We
		// then pull each of these off the source's queue, fill it up with audio data, 
		// and put it back onto the front of the queue.
		////////////////////////////////////////////////////////////////////////////////
		ALint nProcessed;	alGetSourcei(mSource, AL_BUFFERS_PROCESSED, &nProcessed);
	
		while (nProcessed-- != 0)
		{
			ALuint buffer;	alSourceUnqueueBuffers(mSource, 1, &buffer);

			Read(buffer, true);

			alSourceQueueBuffers(mSource, 1, &buffer);
		}
	}

	/// @brief Constructs a BufferFile object
	/// @param name Name of file associated with buffer
	/// @note Tested
	BufferFile::BufferFile (std::wstring const & name) : Buffer(), mName(name)
	{
	}

	/// @brief Destructs a BufferFile object
	/// @note Tested
	BufferFile::~BufferFile (void)
	{
	}

	/// @brief Used to remove a BufferFile object when its reference count goes to 0
	/// @note Tested
	void BufferFile::Remove (void)
	{
		Main::Get().mBufferFiles.erase(this);

		delete this;
	}

	/// @brief Constructs the audio manager
	/// @note Tested
	Main::Main (void) : mSourceKey(0), mBufferFileKey(0), mContext(0), mDevice(0), mInitialized(false)
	{
	}

	/// @brief Destructs the audio manager
	/// @note Tested
	Main::~Main (void)
	{
	}

	/// @brief Accesses the audio manager singleton
	/// @return Reference to the audio manager singleton
	/// @note Tested
	Main & Main::Get (void)
	{
		return G_Main;
	}

	/// @brief Removes all internal data from the audio manager
	/// @note Tested
	void Main::Empty (void)
	{
		/////////////////////////////////////////////////////////////////////////////////
		// We iteratively remove all sources. Doing so will remove all buffers, as well.
		/////////////////////////////////////////////////////////////////////////////////
		while (!mSources.empty())
		{
			AudioDestroySource((*mSources.begin())->mSource);
		}

		//////////////////////////////////////////
		// We remove and nullify our lookup keys.
		//////////////////////////////////////////
		delete mSourceKey;
		delete mBufferFileKey;

		mSourceKey = 0;
		mBufferFileKey = 0;
	}

	/// @brief Removes a source from the audio manager
	/// @param source OpenAL source to remove
	/// @note Tested
	void Main::UnmapSource (ALuint source)
	{
		//////////////////////////////////////////////////////////////////
		// If the source exists, we delete its data and remove the entry.
		//////////////////////////////////////////////////////////////////
		SourceSet::iterator entry = FindSource(source);

		if (entry != mSources.end())
		{
			delete *entry;

			mSources.erase(entry);
		}
	}

	/// @brief Acquires a Source object from the audio manager
	/// @param source OpenAL source used as the search key
	/// @return Iterator associated with the Source object
	/// @note Tested
	SourceSet::iterator Main::FindSource (ALuint source)
	{
		///////////////////////////////////////////////////////////////////////////////
		// We set the source field of the dummy key and use it to lookup the Source in
		// the audio manager's Source set. We return the iterator into this set, after
		// first nullifying the dummy key's source field(so it destructs properly).
		///////////////////////////////////////////////////////////////////////////////
		mSourceKey->mSource = source;

		SourceSet::iterator entry = mSources.find(mSourceKey);

		mSourceKey->mSource = AL_NONE;

		return entry;
	}

	/// @brief Acquires a BufferFile object from the audio manager
	/// @param name Name of file bound to the buffer
	/// @return Iterator associated with the BufferFile object
	/// @note Tested
	BufferFileSet::iterator Main::FindBufferFile (std::wstring const & name)
	{
		/////////////////////////////////////////////////////////////////////////////////
		// We set the name field of the dummy key and use it to lookup the BufferFile in
		// the audio manager's BufferFile set. We return the iterator into this set, 
		// after first nullifying the dummy key's name field(so it destructs properly).
		/////////////////////////////////////////////////////////////////////////////////
		mBufferFileKey->mName = name;

		BufferFileSet::iterator entry = mBufferFiles.find(mBufferFileKey);

		mBufferFileKey->mName.clear();

		return entry;
	}

	/// @brief Acquires a file-based buffer, creating a new one if necessary
	/// @param name Name of file associated with buffer
	/// @param New Function used, if called for, to create a new buffer
	/// @return Pointer to the proper BufferFile object
	/// @note Tested
	BufferFile * Main::GetBufferFile (std::wstring const & name, BufferFile * New (std::wstring const &))
	{
		/////////////////////////////////////////////////////////////////////////////////
		// We lookup the file-based buffer. If not found, we create a new buffer and add 
		// it to the manager's file-based buffer set. Either way, we then increment the
		// buffer's reference count and return it.
		/////////////////////////////////////////////////////////////////////////////////
		BufferFileSet::iterator entry = FindBufferFile(name);

		try {
			if (entry == mBufferFiles.end())
			{
				BufferFile * pBuffer = New(name);

				if (0 == pBuffer) throw std::bad_alloc("GetBufferFile");

				entry = mBufferFiles.insert(pBuffer).first;
			}

			++(*entry)->mRefCount;
		} catch (std::bad_alloc &) {
			return 0;
		}

		return *entry;
	}

	/// @brief Checks for an OpenAL error, reporting one if present
	/// @return A boolean indicating whether there is no error
	/// @note Tested
	bool alCheck (void)
	{
		switch (alGetError())
		{
		case AL_NO_ERROR: 
			return true;
		case AL_INVALID_NAME:
			std::wcerr << L"AL: Invalid name." << std::endl;
			break;
		case AL_INVALID_ENUM:
			std::wcerr << L"AL: Invalid enum." << std::endl;
			break;
		case AL_INVALID_VALUE:
			std::wcerr << L"AL: Invalid value." << std::endl;
			break;
		case AL_INVALID_OPERATION:
			std::wcerr << L"AL: Invalid operation." << std::endl;
			break;
		case AL_OUT_OF_MEMORY:
			std::wcerr << L"AL: Out of memory." << std::endl;
			break;
		}

		return false;
	}
}