#ifndef AUDIO_IMP_H
#define AUDIO_IMP_H

#include <functional>
#include <set>
#include <string>
#include <vector>
#include <AL/al.h>
#include <AL/alc.h>
#include <SDL/SDL_types.h>

namespace Audio 
{
	/// @brief Structure used to represent a sound source
	struct Source {
	// Members
		ALuint mSource;	///< OpenAL source
	// Methods
		Source (ALuint source);
		virtual ~Source (void);

		virtual void Clear (void);
		virtual void Update (void) {}
	};

	/// @brief Functor used to sort sound sources in a set
	struct Source_Less {
	// Methods
		bool operator () (Source * const & _L, Source * const & _R) const
		{
			return _L->mSource < _R->mSource;
		}
	};

	/// @brief Concise sound source set definition
	typedef std::set<Source*, Source_Less> SourceSet;

	/// @brief Structure used to represent a sound buffer
	struct Buffer {
	// Members
		ALuint mBuffer;	///< Shared audio buffer
		Uint32 mRefCount;	///< Count of buffer references
	// Methods
		Buffer (void);
		virtual ~Buffer (void);

		virtual void Remove (void) = 0;

		void Release (void);
	};

	/// @brief Structure used to represent a clip-style sound source
	struct Clip : Source {
	// Members
		Buffer * mBufferData;	///< Data for buffer referred to by clip
	// Methods
		Clip (ALuint source);
		~Clip (void);

		void Clear (void);
	};

	/// @brief Structure used to represent a stream-style sound source
	struct Stream : Source {
	// Enumerations
		enum {
			eBuffersLoaded,	///< Indicates that all buffers loaded succesfully
			eStreamNext	///< Used to continue the bits in derived types
		};
	// Members
		std::bitset<32> mStatus;///< Current status of stream
		ALuint * mBuffers;	///< Buffers used to stream content
		char * mData;	///< Stream data cache
		Uint32 mCount;	///< Buffer count 
		Sint32 mSize;	///< Size of buffer data cache
	// Methods
		Stream (ALuint source, Uint32 count, Sint32 size);
		~Stream (void);

		virtual void Read (ALuint buffer, bool bFill) = 0;

		void PrepareBuffers (void);
		void Clear (void);
		void Update (void);
	};

	/// @brief Structure used to represent a file-based sound buffer
	struct BufferFile : Buffer {
	// Members
		std::wstring mName;	///< Name of file from which buffer is loaded
	// Methods
		BufferFile (std::wstring const & name);
		~BufferFile (void);

		void Remove (void);
	};

	/// @brief Functor used to sort file-based sound buffers in a set
	struct BufferFile_Less {
	// Methods
		bool operator () (BufferFile * const & _L, BufferFile * const & _R) const
		{
			return _L->mName < _R->mName;
		}
	};

	/// @brief Concise file-based sound buffer set definition
	typedef std::set<BufferFile*, BufferFile_Less> BufferFileSet;

	/// @brief Structure used to represent the audio manager
	struct Main {
	// Members
		SourceSet mSources;	///< Source bindings
		BufferFileSet mBufferFiles;	///< File-based buffer storage
		Source * mSourceKey;///< Dummy key used for source lookups
		BufferFile * mBufferFileKey;///< Dummy key used for buffer file lookups
		ALCcontext * mContext;	///< OpenAL OS bindings
		ALCdevice * mDevice;///< Audio device connection
		bool mInitialized;	///< Indicates that the system is initialized
	// Methods
		Main (void);
		~Main (void);

		static Main & Get (void);

		void Empty (void);
		void UnmapSource (ALuint source);
		void Ogg_LoadFromFile (ALuint source, std::wstring const & name, bool bStreaming);

		SourceSet::iterator FindSource (ALuint source);
		BufferFileSet::iterator FindBufferFile (std::wstring const & name);

		BufferFile * GetBufferFile (std::wstring const & name, BufferFile * New (std::wstring const &));
	};

	bool alCheck (void);
}

#endif // AUDIO_IMP_H