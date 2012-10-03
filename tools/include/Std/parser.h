#ifndef _parser_h
#define _parser_h

#include "STD\std_types.h"

/**********************************************************
;
;	Name:		ParserReadStringFile
;
;	Purpose:	reads a string to a given buffer until it
;				reaches the stopat tCHAR.  Will also return
;				if it reaches a newline
;
;	Input:		the file, buff to fill, size of buff,
;				the stopat tCHAR
;
;	Output:		the buff is filled
;
;	Return:		RETCODE_FAILURE if reached end of file...
;				RETCODE_ENDREACHED if stop at reached
;
**********************************************************/
RETCODE ParserReadStringFile(hFILE fp, tCHAR *buff, s32 buffSize, 
									   const tCHAR beginat, const tCHAR stopat);

/**********************************************************
;
;	Name:		ParserCountStringFile
;
;	Purpose:	reads a string to a given buffer until it
;				reaches the stopat tCHAR.  Will also return
;				if it reaches a newline.  This will set the
;				file back to it's original position before
;				returning.
;
;	Input:		the file, numChar to fill number of letters,
;				the stopat tCHAR
;
;	Output:		numChar is filled
;
;	Return:		RETCODE_FAILURE if reached end of file...
;				RETCODE_ENDREACHED if stop at reached
;
**********************************************************/
RETCODE ParserCountStringFile(hFILE fp, s32 *numChar, 
									   const tCHAR beginat, const tCHAR stopat);

/**********************************************************
;
;	Name:		ParserReadWordFile
;
;	Purpose:	finds a word until it sees a ' ', '\n' or
;				the given tCHAR stopat...or if end of file.
;				This function will ensure that the fp will
;				point after the stop at
;
;	Input:		the fp, buff to fill, size of buff, 
;				end tCHAR(optional)
;
;	Output:		the buff is filled
;
;	Return:		the tCHAR that the ReadWord reached after getting
;				the word...or the stopat.  If it is zero, that
;				means the end of file
;
**********************************************************/
tCHAR ParserReadWordFile(hFILE fp, tCHAR *buff, s32 buffSize, const tCHAR stopat);

/**********************************************************
;
;	Name:		ParserCountWordFile
;
;	Purpose:	finds a word until it sees a ' ', '\n' or
;				the given tCHAR stopat...or if end of file.
;				This function will set the fp back to it's
;				original position after return.
;
;	Input:		the fp, the numChar to fill, 
;				end tCHAR(optional)
;
;	Output:		the numChar is filled with number of letters
;
;	Return:		the tCHAR that the ReadWord reached after getting
;				the word...or the stopat.  If it is zero, that
;				means the end of file
;
**********************************************************/
tCHAR ParserCountWordFile(hFILE fp, s32 *numChar, const tCHAR stopat);

/**********************************************************
;
;	Name:		ParserSkipCharFile
;
;	Purpose:	This will just set the fp after the given tCHAR,
;				if it finds it...
;				If it finds something else, then it returns Failure
;
;	Input:		FILE fp, const tCHAR thing
;
;	Output:		fp will point at the end of the tCHAR
;
;	Return:		SUCCESS if we found the 'thing', FAILURE otherwise...
;
**********************************************************/
RETCODE ParserSkipCharFile(hFILE fp, const tCHAR thing, BOOL returnFalseOnOthers=FALSE);

typedef enum {
	dataINT,
	dataSHORT,
	dataFLOAT,
	dataDOUBLE
} eDataType;

/**********************************************************
;
;	Name:		ParserReadDataFile
;
;	Purpose:	reads data from the given file and
;				stuff it inside the dest
;
;	Input:		the file, s32 to fill, and stop at
;
;	Output:		fp points after stop at and dest is filled
;
;	Return:		SUCCESS if we reached stopAt, FAILURE otherwise...
;
**********************************************************/
RETCODE ParserReadDataFile(hFILE fp, void *data, eDataType dType, const tCHAR stopAt);

/**********************************************************
;
;	Name:		
;
;	Purpose:	
;
;	Input:		
;
;	Output:		
;
;	Return:		
;
**********************************************************/
BOOL ParserSkipString(hFILE fp, const tCHAR beginAt, const tCHAR stopAt, const tCHAR *string);

//
// string buffer version
//

/**********************************************************
;
;	Name:		ParserReadStringBuff
;
;	Purpose:	reads a string to a given buffer until it
;				reaches the stopat tCHAR.  Will also return
;				if it reaches a newline
;
;	Input:		the buff, buff to fill, size of buff,
;				the stopat tCHAR
;
;	Output:		the buff is filled
;
;	Return:		RETCODE_FAILURE if reached end of file...
;				RETCODE_ENDREACHED if stop at reached
;
**********************************************************/
RETCODE ParserReadStringBuff(tCHAR **theBuff, tCHAR *buff, s32 buffSize, 
									   const tCHAR beginat, const tCHAR stopat);

/**********************************************************
;
;	Name:		ParserCountStringBuff
;
;	Purpose:	reads a string to a given buffer until it
;				reaches the stopat tCHAR.  Will also return
;				if it reaches a newline
;
;	Input:		the buff, numChar to fill number of letters,
;				the stopat tCHAR
;
;	Output:		the numChar is filled
;
;	Return:		RETCODE_FAILURE if reached end of file...
;				RETCODE_ENDREACHED if stop at reached
;
**********************************************************/
RETCODE ParserCountStringBuff(const tCHAR *theBuff, s32 *numChar, 
									   const tCHAR beginat, const tCHAR stopat);

/**********************************************************
;
;	Name:		ParserReadWordBuff
;
;	Purpose:	finds a word until it sees a ' ', '\n' or
;				the given tCHAR stopat...or if end of file.
;				This function will ensure that the fp will
;				point after the stop at
;
;	Input:		the buff to read, buff to fill, size of buff, 
;				end tCHAR(optional)
;
;	Output:		the buff is filled
;
;	Return:		the tCHAR that the ReadWord reached after getting
;				the word...or the stopat.  If it is zero, that
;				means the end of file
;
**********************************************************/
tCHAR ParserReadWordBuff(tCHAR **theBuff, tCHAR *buff, s32 buffSize, const tCHAR stopat);

/**********************************************************
;
;	Name:		ParserCountWordBuff
;
;	Purpose:	finds a word until it sees a ' ', '\n' or
;				the given tCHAR stopat...or if end of file.
;				This function will ensure that the fp will
;				point after the stop at
;
;	Input:		the buff to read, buff to fill, size of buff, 
;				end tCHAR(optional)
;
;	Output:		numChar is filled with number of letters
;
;	Return:		the tCHAR that the ReadWord reached after getting
;				the word...or the stopat.  If it is zero, that
;				means the end of file
;
**********************************************************/
tCHAR ParserCountWordBuff(const tCHAR *theBuff, s32 *numChar, const tCHAR stopat);

/**********************************************************
;
;	Name:		ParserSkipCharBuff
;
;	Purpose:	This will just set the fp after the given tCHAR,
;				if it finds it...
;				If it finds something else, then it returns Failure
;
;	Input:		tCHAR **theBuff, const tCHAR thing
;
;	Output:		theBuff will point at the end of the tCHAR
;
;	Return:		SUCCESS if we found the 'thing', FAILURE otherwise...
;
**********************************************************/
RETCODE ParserSkipCharBuff(tCHAR **theBuff, const tCHAR thing, BOOL returnFalseOnOthers=false);

/**********************************************************
;
;	Name:		ParserReadDataBuff
;
;	Purpose:	reads data from the given file and
;				stuff it inside the dest
;
;	Input:		the file, s32 to fill, and stop at
;
;	Output:		fp points after stop at and dest is filled
;
;	Return:		SUCCESS if we reached stopAt, FAILURE otherwise...
;
**********************************************************/
RETCODE ParserReadDataBuff(tCHAR **theBuff, void *dest, eDataType dType, const tCHAR stopAt);

// This function returns the file portion of the filename.  If there is
// no filename (ie; if PathName is an empty string, or if PathName appears
// to name a directory (ie; final character is a backslash)) then it returns
// a pointer to the null character at the end of the filename (ie; a null
// string).  If there are no backslashes then it is assumed that the entire
// name represents a filename, so the entire string is returned. This function
// will never return zero. You can remove the filename from any path by
// writing zero to the tCHAR pointed at by the return value.

tCHAR *GetFilePart(const tCHAR *PathName);

// This function returns the file extension, pointing at the period, if there
// is one.  If there is no file extension, it points to the null character at
// the end of the filename (ie; a null string).

tCHAR *GetExtension(const tCHAR *PathName);

// Get the filename ex: c:\dumbshit\asshole.jpg -> asshole
void GetFilename(const tCHAR *PathName, tCHAR *str, s32 strLen);

void ConcatinateFilePath(const tCHAR *path, tCHAR *relativePath,
								tCHAR *out);

/////////////////////////////////////
// Name:	ParserFindFiles
// Purpose:	get all files in given directory
//			with pattern and process it
// Output:	stuff
// Return:	none
/////////////////////////////////////
void ParserFindFiles(tCHAR *dirname, tCHAR *pattern, BOOL bCheckSubDir, FINDCALLBACK callback, void *userDat);

/////////////////////////////////////
// Name:	ParserReadCommandLine
// Purpose:	reads the command line and
//			calls 'callback' with given
//			command
//			separator = '-', '/'...etc.
// Output:	
// Return:	
/////////////////////////////////////
RETCODE ParserReadCommandLine(const tCHAR *cmdline,
									 tCHAR separator,
									 CMDLINECALLBACK callback,
									 void *userData);

/////////////////////////////////////
// Name:	ParserGrabAllTextFile
// Purpose:	will grab all text in
//			given file.
//			note: you must free the
//			returned text yourself.
// Output:	allocated string
// Return:	the allocated string
//			with stuff inside.  NULL
//			if file non-existent
/////////////////////////////////////
tCHAR * ParserGrabAllTextFile(const tCHAR *filename);

#endif