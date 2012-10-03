#include "filesys_i.h"

#include "STD\memTool.h"
#include "STD\log.h"

hFMAIN g_main=0;	//this is set by the user to specify the current list of paths/archives

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
hFMAIN FSMainCreate()
{
	hFMAIN newMain = (hFMAIN)MemAlloc(sizeof(FSMain));

	if(!newMain) { LogMsg(LOG_FILE, L"FSCreateMain", L"Failed to allocate"); }

	newMain->paths = new list<FSPath>;

	return newMain;
}

/////////////////////////////////////
// Name:	FSMainDestroy
// Purpose:	destroy given FS Main
//			interface
// Output:	interface destroyed
// Return:	none
/////////////////////////////////////
void FSMainDestroy(hFMAIN fs)
{
	if(fs)
	{
		if(fs == g_main)
			g_main = 0;

		if(fs->paths)
		{
			//close all zip files
			for(list<FSPath>::iterator i = fs->paths->begin(); i != fs->paths->end(); i++)
			{
				if((*i).uf)
					unzClose((*i).uf);
			}

			delete fs->paths;
		}

		MemFree(fs);
	}
}

/////////////////////////////////////
// Name:	FSMainSet
// Purpose:	set the vassal for paths
//			and archives.  Basically
//			this just sets the main
//			pointer.
// Output:	global main pointer set.
// Return:	none
/////////////////////////////////////
void FSMainSet(hFMAIN fs)
{
	g_main = fs;
}

//
// File System Manager
//

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
void FSMainAddDir(const tCHAR *dir, FLAGS flag)
{
	if(g_main)
	{
		FSPath path;
		path.str = dir;
		path.flag = flag;
		path.uf = 0;

		if(TESTFLAGS(path.flag, FS_DIR_ARCHIVE))
		{
#ifdef WIN32
			char sDir[MAXCHARBUFF]; wcstombs(sDir, dir, MAXCHARBUFF);

			zlib_filefunc_def ffunc;
			fill_win32_filefunc(&ffunc);

			//open the zip file
			path.uf = unzOpen2(sDir,&ffunc);
#else
			path.uf = unzOpen(sDir);
#endif
		}

		g_main->paths->push_front(path);
	}
}