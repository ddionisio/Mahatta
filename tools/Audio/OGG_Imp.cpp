/// @file
///

#include "common.h"
#include "Std/filesys.h"

#include "Audio_Imp.h"
#include "OGG_Imp.h"

namespace Audio
{
	/// @brief Constructs an Ogg_ClipFile object
	/// @param source OpenAL source to bind
	/// @param name Name of Ogg file
	/// @note Tested
	Ogg_ClipFile::Ogg_ClipFile (ALuint source, std::wstring const & name) : Clip(source)
	{
		mBufferData = Main::Get().GetBufferFile(name, Ogg_New);

		if (mBufferData != 0) alSourcei(mSource, AL_BUFFER, mBufferData->mBuffer);
	}

	/// @brief Destructs an Ogg_ClipFile object
	/// @note Tested
	Ogg_ClipFile::~Ogg_ClipFile (void)
	{
	}

	/// @brief Constructs an Ogg_StreamFile object
	/// @param source OpenAL source to bind
	/// @param name Name of Ogg file
	/// @param count Count of OpenAL buffers to use for Ogg streaming
	/// @param size Size, in bytes, of each Ogg buffer
	/// @note Tested
	Ogg_StreamFile::Ogg_StreamFile (ALuint source, std::wstring const & name, Uint32 count, Sint32 size) : Stream(source, count, size)
	{
		///////////////////////////////////////////////////////////////////////////////
		// Derived types may not need to open a file, in which case we can exit early.
		///////////////////////////////////////////////////////////////////////////////
		if (name.empty()) return;

		////////////////////////////////////////////////////////////////////////////////
		// We open the file. If it is an archive, we clear the data we have loaded thus
		// far and throw the file pointer up. Otherwise, we open the Vorbis file in the
		// standard stdio-based way, and fill up our buffers.
		////////////////////////////////////////////////////////////////////////////////
		hFILE fp = FSOpen(name.c_str(), L"rb");

		if (fp != 0)
		{
			if (FSIsArchive(fp))
			{
				Clear();

				throw fp;
			}

			if (0 == ov_open(FSFileGet(fp), &mFile, 0, 0))
			{
				mStatus[eVorbis] = true;

				PrepareBuffers();
			}

			else FSClose(fp);
		}

		////////////////////////////////////////////////////////////////////////////
		// If we did not fully construct, we clear out our data and throw an error.
		////////////////////////////////////////////////////////////////////////////
		if (!mStatus[eBuffersLoaded])
		{
			Clear();

			throw std::bad_alloc("Ogg_StreamFile");
		}
	}

	/// @brief Destructs an Ogg_StreamFile object
	/// @note Tested
	Ogg_StreamFile::~Ogg_StreamFile (void)
	{
		ov_clear(&mFile);
	}

	/// @brief Reads data from the Ogg file stream into an OpenAL buffer
	/// @param buffer OpenAL buffer to load
	/// @param bFill Indicates whether to load until the buffer is full
	/// @note Tested
	void Ogg_StreamFile::Read (ALuint buffer, bool bFill)
	{
		//////////////////////////////////////////////////////////////////////////////////
		// We read data from the Ogg stream into a data cache, filling it if possible--on 
		// each read, we accumulate the count of bytes read, and compare the accumulator
		// against the size of the cache. If we encounter an error, we exit out. If we
		// hit the end of the file, we have two options: if we want to fill the buffer,
		// we go back to the start of the file and continue reading; otherwise, we reduce
		// the size of the data cache instead.
		//////////////////////////////////////////////////////////////////////////////////
		int size = 0, section;

		do {
			int result = ov_read(&mFile, mData + size, mSize - size, 0, 2, 1, &section);

			if (result < 0) throw buffer;

			else if (0 == result)
			{
				if (bFill) ov_raw_seek_lap(&mFile, 0);

				else mSize = size;
			}

			else size += result;
		} while (size < mSize);

		///////////////////////////////////////////////////////////////
		// We now buffer the data in our cache into the OpenAL buffer.
		///////////////////////////////////////////////////////////////
		vorbis_info * vInfo = ov_info(&mFile, -1);

		alBufferData(buffer, 1 == vInfo->channels ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, mData, size, vInfo->rate);
	}

	/// @brief Clears a Ogg_StreamFile object's resources
	/// @note Tested
	void Ogg_StreamFile::Clear (void)
	{
		Stream::Clear();

		if (mStatus[eVorbis])
		{
			ov_clear(&mFile);

			mStatus[eVorbis] = false;
		}
	}

	/// @brief Constructs an Ogg_Memory object
	/// @param buffer Memory buffer to assign to object
	/// @param range Memory range to assign to object
	/// @note Tested
	Ogg_Memory::Ogg_Memory (Uint8 * buffer, Uint32 range) : mBuffer(buffer), mRange(range), mWhere(0)
	{
	}

	/// @brief Destructs an Ogg_Memory object
	Ogg_Memory::~Ogg_Memory (void)
	{
	}

	/// @brief Constructs an Ogg_StreamArchive object
	/// @param source OpenAL source to bind
	/// @param name Name of Ogg archive
	/// @param fp Handle of archive file
	/// @param count Count of OpenAL buffers to use for Ogg streaming
	/// @param size Size, in bytes, of each Ogg buffer
	/// @note Tested
	Ogg_StreamArchive::Ogg_StreamArchive (ALuint source, std::wstring const & name, hFILE fp, Uint32 count, Sint32 size) : Ogg_StreamFile(source, L"", count), mMemory(FSBufferGetPtr(fp), FSBufferGetSize(fp)), mFP(fp)
	{
		/////////////////////////////////////////////////////////////////////////////////
		// We open the Vorbis file using memory-reading routines, and the mMemory member 
		// of this object as the data field. We then fill up our buffers.
		/////////////////////////////////////////////////////////////////////////////////
		ov_callbacks callbacks;

		callbacks.read_func = Ogg_ReadMemory;
		callbacks.seek_func = Ogg_SeekMemory;
		callbacks.close_func = Ogg_CloseMemory;
		callbacks.tell_func = Ogg_TellMemory;

		if (0 == ov_open_callbacks(&mMemory, &mFile, 0, 0, callbacks))
		{
			mStatus[eVorbis] = true;

			PrepareBuffers();
		}

		////////////////////////////////////////////////////////////////////////////
		// If we did not fully construct, we clear out our data and throw an error.
		////////////////////////////////////////////////////////////////////////////
		if (!mStatus[eBuffersLoaded])
		{
			Clear();

			throw std::bad_alloc("Ogg_StreamArchive");
		}
	}

	/// @brief Destructs an Ogg_StreamArchive object
	/// @note Tested
	Ogg_StreamArchive::~Ogg_StreamArchive (void)
	{
		if (mFP != 0) FSClose(mFP);
	}

	/// @brief Clears an Ogg_StreamArchive object's resources
	/// @note Tested
	void Ogg_StreamArchive::Clear (void)
	{
		Ogg_StreamFile::Clear();

		if (mFP != 0)
		{
			FSClose(mFP);

			mFP = 0;
		}
	}

	/// @brief Loads an Ogg source from a file
	/// @param source OpenAL source to bind
	/// @param name Name of Ogg file
	/// @param bStreaming Indicates whether the source is streaming
	/// @note Tested
	void Main::Ogg_LoadFromFile (ALuint source, std::wstring const & name, bool bStreaming)
	{
		Source * pSource = 0;

		////////////////////////////////////////////////////////////////////////////////
		// If we want to load a streaming Ogg file, we first try to constructs a normal
		// Ogg_StreamFile object. If this throws a file handle, we catch it and make an
		// Ogg_StreamArchive object instead. If we are not streaming, we simply make an
		// Ogg_ClipFile object. We then insert the new object into the source set.
		////////////////////////////////////////////////////////////////////////////////
		try {
			if (bStreaming) pSource = new Ogg_StreamFile(source, name);

			else pSource = new Ogg_ClipFile(source, name);
		} catch (hFILE fp) {
			pSource = new Ogg_StreamArchive(source, name, fp);
		}

		mSources.insert(pSource);
	}

	/// @brief Used to create a new Ogg file-based buffer
	/// @param name Name of Ogg file
	/// @return The new Ogg file-based buffer
	/// @note Tested
	BufferFile * Ogg_New (std::wstring const & name)
	{
		BufferFile * pBuffer = 0;
		Stream * pStream = 0;

		////////////////////////////////////////////////////////////////////////////////
		// We create our BufferFile object. We first try to construct an Ogg_StreamFile
		// object, given our file name. If this throws a file handle, we catch it and 
		// make an Ogg_StreamArchive object instead. We read audio data from whichever
		// stream we constructed, in its entirety, into the buffer.
		////////////////////////////////////////////////////////////////////////////////
		try {
			pBuffer = new BufferFile(name);
			
			try {
				pStream = new Ogg_StreamFile(AL_NONE, name, 0, 4096 * 128);
			} catch (hFILE fp) {
				pStream = new Ogg_StreamArchive(AL_NONE, name, fp, 0, 4096 * 128);
			}
			
			pStream->Read(pBuffer->mBuffer, false);
		} catch (...) {
			delete pBuffer;
			pBuffer = 0;
		}

		///////////////////////////////////////////////////////////////
		// We destroy our temporary stream, and return the new buffer.
		///////////////////////////////////////////////////////////////
		delete pStream;

		return pBuffer;
	}

	/// @brief Read callback for an Ogg archive
	/// @param buffer [out] Buffer into which callback reads data
	/// @param size Size of a member to be read
	/// @param nmemb Number of members to read
	/// @param pMemory Memory object pointer
	/// @return Count of members fully read
	/// @note Tested
	size_t Ogg_ReadMemory (void * buffer, size_t size, size_t nmemb, void * pMemory)
	{
		if (0 == size || 0 == nmemb) return 0;

		Ogg_Memory * pM = static_cast<Ogg_Memory*>(pMemory);

		////////////////////////////////////////////////////////////////////////////////
		// We get the extent of the requested read, and clamp it to the possible range.
		// From here, we find out how many bytes to read. If this amount is non-0, we
		// copy data from memory into our buffer and increment the memory offset. We
		// then return the count of items succesfully read.
		////////////////////////////////////////////////////////////////////////////////
		size_t end = pM->mWhere + size * nmemb;

		if (end > pM->mRange) end = pM->mRange;

		size_t count = end - pM->mWhere;

		if (count != 0)
		{
			memcpy(buffer, pM->mBuffer + pM->mWhere, count);

			pM->mWhere += count;
		}

		return count / size;
	}

	/// @brief Seek callback for an Ogg archive
	/// @param pMemory Memory object pointer
	/// @param offset Offset indicating how to seek, given whence
	/// @param whence Indicates how to treat the offset
	/// @return 0 on success, -1 for failure
	/// @note Tested
	int Ogg_SeekMemory (void * pMemory, ogg_int64_t offset, int whence)
	{
		Ogg_Memory * pM = static_cast<Ogg_Memory*>(pMemory);

		///////////////////////////////////////////////////////////////////////////////
		// We simply assign the supplied offset to the memory offset, so we can ignore
		// the SEEK_SET case. In the SEEK_CUR case, we extend the offset by the memory
		// offset. In the SEEK_END case, we extend it by the memory range.
		///////////////////////////////////////////////////////////////////////////////
		switch (whence)
		{
		case SEEK_CUR:
			offset += pM->mWhere;
			break;
		case SEEK_END:
			offset += pM->mRange;
			break;
		}

		pM->mWhere = static_cast<size_t>(offset);

		return 0;
	}

	/// @brief Close callback for an Ogg archive
	/// @param pMemory Memory object pointer
	/// @return 0 on success, -1 for failure
	/// @note Tested
	int Ogg_CloseMemory (void * pMemory)
	{
		return 0;
	}

	/// @brief Tell callback for an Ogg archive
	/// @param pMemory Memory object pointer
	/// @return Current location in the stream
	/// @note Tested
	long Ogg_TellMemory (void * pMemory)
	{
		Ogg_Memory * pM = static_cast<Ogg_Memory*>(pMemory);

		return static_cast<long>(pM->mWhere);
	}
}