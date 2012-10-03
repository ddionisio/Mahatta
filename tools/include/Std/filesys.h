#ifndef _filesys_h
#define _filesys_h

#include "STD\std_types.h"

//The game file system
//everything that has got 
//to do with loading is here

//
// File Interface
//

/////////////////////////////////////
// Name:	FSMainCreate
// Purpose:	create a vassal for paths
//			and archives.  You have to
//			create this before using all
//			the FS functions.  After this,
//			you need to call 
//			FSMainSet(hFMAIN).
// Output:	new FSMain created
// Return:	the new FSMain
/////////////////////////////////////
hFMAIN FSMainCreate();

/////////////////////////////////////
// Name:	FSMainDestroy
// Purpose:	destroy given FS Main
//			interface
// Output:	interface destroyed
// Return:	none
/////////////////////////////////////
void FSMainDestroy(hFMAIN fs);

/////////////////////////////////////
// Name:	FSMainSet
// Purpose:	set the vassal for paths
//			and archives.  Basically
//			this just sets the main
//			pointer.
// Output:	global main pointer set.
// Return:	none
/////////////////////////////////////
void FSMainSet(hFMAIN fs);

/////////////////////////////////////
// Name:	FSAddDir
// Purpose:	add a directory for the
//			File Sys. to search files.
//			Make sure you have called
//			FSMainSet(fs) with a valid
//			interface prior to this.
//			FS_DIR_ARCHIVE = if given 'dir' is
//			an archive(pak) file.
//			FS_DIR_READONLY = disallow 
//			writing/creating in
//			this dir.
// Output:	directory appened
// Return:	none
/////////////////////////////////////
void FSMainAddDir(const tCHAR *dir, FLAGS flag);

//
// Interface
//

/////////////////////////////////////
// Name:	FSGetPath
// Purpose:	Get the real path from 
//			the given relative path
// Output:	strOut set
// Return:	TRUE if filepath exists
/////////////////////////////////////
u8 FSGetPath(const tCHAR *filepath, tCHAR *strOut, u32 size);

/////////////////////////////////////
// Name:	FSOpen
// Purpose:	this will open a file
//			relative to game paths
//			(maybe from pack files)
// Output:	file opened
// Return:	file handle
/////////////////////////////////////
hFILE FSOpen(const tCHAR *filepath, const tCHAR *mode);

/////////////////////////////////////
// Name:	FSClose
// Purpose:	close the file
// Output:	file closed
// Return:	none
/////////////////////////////////////
void FSClose(hFILE fp);

/////////////////////////////////////
// Name:	FSIsArchive
// Purpose:	determine if given fp is
//			loaded from archive.
// Output:	none
// Return:	TRUE if so.
/////////////////////////////////////
u8 FSIsArchive(hFILE fp);

/////////////////////////////////////
// Name:	FSFileGet
// Purpose:	in case you really need
//			the FILE *, then call this.
//			This is NULL if fp is from
//			archive (FSIsArchive=TRUE)
// Output:	none
// Return:	the FILE *
/////////////////////////////////////
FILE *FSFileGet(hFILE fp);

/////////////////////////////////////
// Name:	FSBufferGetPtr
// Purpose:	this is only used for 
//			archive fp's (FSIsArchive=TRUE)
//			For external loading of data
// Output:	none
// Return:	the buffer
/////////////////////////////////////
u8 *FSBufferGetPtr(hFILE fp);

/////////////////////////////////////
// Name:	FSBufferGetSize
// Purpose:	get the size of the buffer
//			only archive fp's (FSIsArchive=TRUE)
// Output:	none
// Return:	the size of the buffer (bytes)
/////////////////////////////////////
u32 FSBufferGetSize(hFILE fp);

/////////////////////////////////////
// Name:	FSRead
// Purpose:	read bytes from file
//			size = size in bytes
//			count = num items
// Output:	buffer filled, file updated
// Return:	number of items read
//			may be less than count
/////////////////////////////////////
u64 FSRead(void *buffer, u64 size, u64 count, hFILE fp);

/////////////////////////////////////
// Name:	FSWrite
// Purpose:	write bytes to file
//			size = size in bytes
//			count = num items
//			NOTE: you can only use
//			this on files outside
//			pack files.
// Output:	file updated
// Return:	number of items written
//			may be less than count
/////////////////////////////////////
u64 FSWrite(void *buffer, u64 size, u64 count, hFILE fp);

/////////////////////////////////////
// Name:	FSEndOfFile
// Purpose:	did we reach end of file?
// Output:	none
// Return:	TRUE if so
/////////////////////////////////////
u8 FSEOF(hFILE fp);

/////////////////////////////////////
// Name:	FSGetChar
// Purpose:	get one tCHAR
// Output:	file updated
// Return:	tCHAR
/////////////////////////////////////
s32 FSGetChar(hFILE fp);

/////////////////////////////////////
// Name:	FSPutChar
// Purpose:	put one tCHAR
// Output:	file updated
// Return:	the tCHAR written
/////////////////////////////////////
s32 FSPutChar(s32 c, hFILE fp);

/////////////////////////////////////
// Name:	FSSeek
// Purpose:	seek file position
//	FS_SEEK_SET = beginning
//	FS_SEEK_CUR = current
//	FS_SEEK_END = end
// Output:	file updated
// Return:	0 if success
/////////////////////////////////////
s32 FSSeek(hFILE fp, s32 offset, FS_SEEK origin);

/////////////////////////////////////
// Name:	FSGetPos
// Purpose:	get file position
// Output:	pos set
// Return:	0 if success
/////////////////////////////////////
s32 FSGetPos(hFILE fp, s64 *pos);

/////////////////////////////////////
// Name:	FSSetPos
// Purpose:	set file position
// Output:	file updated
// Return:	0 if success
/////////////////////////////////////
s32 FSSetPos(hFILE fp, s64 *pos);

/////////////////////////////////////
// Name:	FSPrintf
// Purpose:	print string to fp
// Output:	fp updated
// Return:	number of bytes written
/////////////////////////////////////
s32 FSPrintf(hFILE fp, const tCHAR *str, ...);

#endif