#include <io.h>

#include "i_parser.h"

#define BIG_BUFFERSIZE 1024

/////////////////////////////////////
// Name:	ParserFindFiles
// Purpose:	get all files in given directory
//			with pattern and process it
// Output:	stuff
// Return:	none
/////////////////////////////////////
void ParserFindFiles(tCHAR *dirname, tCHAR *pattern, BOOL bCheckSubDir, FINDCALLBACK callback, void *userDat)
{
	tCHAR big_buffer[BIG_BUFFERSIZE];
	tCHAR filePath[BIG_BUFFERSIZE];

	long file;		//this is for the findfirst argument
	struct _wfinddata_t stuff; //the structure of finddata_t for use of stuff
	s32 arefiles;
	
	if(wcslen(dirname) + wcslen(pattern) + 2 > BIG_BUFFERSIZE) // why do I add 2 here???
	{														   // because of the backslash!!!
		printf("ERROR - path too long!!!\n");
		exit(1);
	}
	wcscpy(big_buffer, dirname);
	wcscat(big_buffer, L"\\");
	wcscat(big_buffer, pattern);

    // INITIALIZATION STARTS HERE (file listing and such)
	file = _wfindfirst( big_buffer, &stuff );



	if( (file = _wfindfirst( big_buffer, &stuff )) != -1L ) //print the files if there are any
	{
	   do
	   {
		   if ((wcscmp(stuff.name, L".") == 0) || (wcscmp(stuff.name, L"..") == 0))
			   continue; // ignore both dots...

		   //let the user process it
		   if(!( stuff.attrib & _A_SUBDIR))
		   {
			    wcscpy(filePath, dirname);
				wcscat(filePath, L"\\");
				wcscat(filePath, stuff.name);
			if(callback)
				callback(filePath, userDat);
		   }

	   }while( _wfindnext( file, &stuff ) == 0 );
	   
	}

//CHECK FOR OTHER DIR------------------------------------------------------------
	if(bCheckSubDir)
	{
		wcscpy(big_buffer, dirname);
		wcscat(big_buffer, L"\\");
		wcscat(big_buffer, L"*"); //star means check all files in current DIR

		arefiles = 1; //assume that there are files

		if( (file = _wfindfirst( big_buffer, &stuff )) == -1L ) //if it is not a file
			arefiles = 0;
		else if(_wfindnext( file, &stuff ) == -1L) // If you don't skip twice, there will be an endless loop
			arefiles = 0;

		if(arefiles) //same as above after the two dots, if the next file after the 2 dot is a file 
		{
			while( _wfindnext( file, &stuff ) != -1L) //look for more directories...while there is something
			{
				if( stuff.attrib & _A_SUBDIR)    
					{
						wcscpy(big_buffer,dirname);
						wcscat(big_buffer,L"\\");
						wcscat(big_buffer,stuff.name);

						//recall the function with new directory  
						ParserFindFiles(big_buffer, pattern, bCheckSubDir, callback, userDat);
					}  
			}
		}
	}
	
	   _findclose( file );
}