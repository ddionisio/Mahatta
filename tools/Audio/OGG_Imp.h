#ifndef OGG_IMP_H
#define OGG_IMP_H

#include <VORBIS/vorbisfile.h>

namespace Audio 
{
	/// @brief Structure used to represent an Ogg audio memory datum
	struct Ogg_Memory {
	// Members
		Uint8 * mBuffer;///< Address of memory buffer
		size_t mWhere;	///< Current memory position
		Uint32 mRange;	///< Count of bytes in memory range
	// Methods
		Ogg_Memory (Uint8 * buffer, Uint32 range);
		~Ogg_Memory (void);
	};

	/// @brief Structure used to represent an Ogg audio file clip
	struct Ogg_ClipFile : Clip {
	// Methods
		Ogg_ClipFile (ALuint source, std::wstring const & name);
		~Ogg_ClipFile (void);
	};

	/// @brief Structure used to represent an Ogg audio file stream
	struct Ogg_StreamFile : Stream {
	// Enumerations
		enum {
			eVorbis = eStreamNext	///< Indicates that the Vorbis file is opened
		};			
	// Members
		OggVorbis_File mFile;	///< OggVorbis file reader
	// Methods
		Ogg_StreamFile (ALuint source, std::wstring const & name, Uint32 count = 2, Sint32 size = 4096 * 8);
		~Ogg_StreamFile (void);

		void Read (ALuint buffer, bool bFill);
		void Clear (void);
	};

	/// @brief Structure used to represent an Ogg audio archive stream
	struct Ogg_StreamArchive : Ogg_StreamFile {
	// Members
		hFILE mFP;	///< Handle of file used to maintain stream
		Ogg_Memory mMemory;	///< Datum used to load from memory
	// Methods
		Ogg_StreamArchive (ALuint source, std::wstring const & name, hFILE fp, Uint32 count = 2, Sint32 size = 4096 * 8);
		~Ogg_StreamArchive (void);

		void Clear (void);
	};

	BufferFile * Ogg_New (std::wstring const & name);

	size_t Ogg_ReadMemory (void * buffer, size_t size, size_t nmemb, void * pMemory);
	int Ogg_SeekMemory (void * pMemory, ogg_int64_t offset, int whence);
	int Ogg_CloseMemory (void * pMemory);
	long Ogg_TellMemory (void * pMemory);
}

#endif OGG_IMP_H