#ifndef _filesys_i_h
#define _filesys_i_h

#include "common.h"

#include "STD\filesys.h"

#include "zlib\unzip.h"

#ifdef WIN32
#include "zlib\iowin32.h"
#endif

#include <fcntl.h>
#include <io.h>

typedef struct _FSPath {
	wstring		str;

	unzFile		uf;			//the unzip file

	FLAGS		flag;		//FS_DIR_*

	u8			bArchive;	//is this a pack file? If so, str is a filename.
} FSPath;

typedef struct _FSMain {
	list<FSPath>	*paths; //list of directories/archives
} FSMain;

typedef enum {
	UC_None,
	UC_Base,
	UC_BigEndian,
	UC_UTF8
} eUniType;

typedef struct _FSFile {
	FILE	*fp;			//if this is from a file

	eUniType uCType;		//for checking unicode (UC_None, UC_Base, UC_BigEndian, UC_UTF8)
	
	u8		*buff;			//if this is from an archive, this is set
	u32		 buffSize;		//the buffer size
	u32		 buffInd;		//current index pointer to buffer
} FSFile;

extern hFMAIN g_main;

#endif