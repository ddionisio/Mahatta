#include "filesys_i.h"

#include "STD\memTool.h"
#include "STD\log.h"

/////////////////////////////////////
// Name:	FSGetPath
// Purpose:	Get the real path from 
//			the given relative path
// Output:	strOut set
// Return:	TRUE if filepath exists
/////////////////////////////////////
u8 FSGetPath(const tCHAR *filepath, tCHAR *strOut, u32 size)
{
	u8 ret = FALSE;
	FILE *fp = 0;
	wstring path;

	const tCHAR *sStr;
	u32 sSize;

	if(!g_main || g_main->paths->size() == 0)
	{
		wcscpy(strOut, filepath);

		//try to open the file
		fp = _wfopen(strOut, L"rb");

		if(fp) //found it?
		{
			fclose(fp);
			ret = TRUE;
		}

		sStr = filepath;
		sSize = wcslen(filepath);
	}
	else
	{
		//go through the list to find where this file is
		for(list<FSPath>::iterator i = g_main->paths->begin(); i != g_main->paths->end(); i++)
		{
			if(!TESTFLAGS((*i).flag, FS_DIR_ARCHIVE))
			{
				path = (*i).str;
				path += L"\\";
				path += filepath;

				//try to open the file
				fp = _wfopen(path.c_str(), L"rb");

				if(fp) //found it?
				{
					fclose(fp);
					ret = TRUE;
					break;
				}
			}
		}

		sStr = path.c_str();
		sSize = path.size();
		//wcscpy(strOut, path.c_str());
	}

	if(size >= sSize)
	{
		memcpy(strOut, sStr, sizeof(tCHAR)*sSize);
		strOut[sSize] = 0;
	}
	else
	{
		memcpy(strOut, sStr, sizeof(tCHAR)*size);
		strOut[size-1] = 0;
	}

	return ret;
}

//filepath = fullpath
// 0 = not exist, 1=success, -1=failed
static s8 _Open(hFILE fp, const FSPath & fsPath, const tCHAR *filepath, const tCHAR *mode)
{
	s8 ret = FALSE;

	//get the write mode
	u8 bWriteMode = FALSE; //we are going to write on the file?

	tCHAR *modePtr = (tCHAR*)mode;
	while(*modePtr != 0) { if(*modePtr == 'w') { bWriteMode = TRUE; break; } modePtr++; }

	if(TESTFLAGS(fsPath.flag, FS_DIR_ARCHIVE))
	{
		//are we trying to write?
		//say it doesn't exist.
		if(bWriteMode)
			return 0;

		char sPath[MAXCHARBUFF]; wcstombs(sPath, filepath, MAXCHARBUFF);

		//switch all '\' to '/'
		u32 sSize = (u32)strlen(sPath);
		for(u32 i = 0; i < sSize; i++)
		{
			if(sPath[i] == '\\') sPath[i] = '/';
		}

		if(fsPath.uf)
		{
			//check to see if the given filepath is in the zip
			if(unzLocateFile(fsPath.uf, sPath, 2) == UNZ_OK)
			{
				unz_file_info file_info;

				unzGetCurrentFileInfo(fsPath.uf, &file_info, sPath, MAXCHARBUFF, 0,0,0,0);

				if(unzOpenCurrentFile(fsPath.uf) == UNZ_OK)
				{
					//allocate the buffer for this file
					fp->buff = (u8*)MemAlloc(sizeof(u8)*file_info.uncompressed_size);

					if(fp->buff)
					{
						//copy the data to the buffer
						unzReadCurrentFile(fsPath.uf, (voidp)fp->buff, file_info.uncompressed_size);

						fp->buffSize = file_info.uncompressed_size;

						//check for unicode (we only use UC_Base/UC_BigEndian for now...)
						if(fp->buff[0] == 0xff && fp->buff[1] == 0xfe)
							fp->uCType = UC_Base;
						else if(fp->buff[0] == 0xfe && fp->buff[1] == 0xff)
							fp->uCType = UC_BigEndian;

						ret = TRUE;
					}
					else { ret = -1; LogMsg(LOG_FILE, L"_Open: unable to allocate buffer", filepath); }

					unzCloseCurrentFile(fsPath.uf);
				}
				else { ret = -1; LogMsg(LOG_FILE, L"_Open: unable to open file in archive", filepath); }
			}
		}
	}
	else
	{
		//are we trying to write?
		//say it doesn't exist if this path is read-only
		if(bWriteMode && TESTFLAGS(fsPath.flag, FS_DIR_READONLY))
			return 0;

		wstring path = fsPath.str;
		path += L"\\";
		path += filepath;

		//try to open the file
		fp->fp = _wfopen(path.c_str(), mode);

		if(fp->fp) 
		{
			//detect unicode...
			u16 magic;
			FSRead(&magic, sizeof(u16), 1, fp);

			if(magic == 0xfffe) fp->uCType = UC_BigEndian;
			else if(magic == 0xfeff) fp->uCType = UC_Base;

			if(fp->uCType != UC_None)
				_setmode( _fileno( fp->fp ), _O_BINARY );
			else
				FSSeek(fp, 0, FS_SEEK_SET);

			ret = TRUE;
		}
	}

	return ret;
}

/////////////////////////////////////
// Name:	FSOpen
// Purpose:	this will open a file
//			relative to game paths
//			(maybe from pack files)
// Output:	file opened
// Return:	file handle
/////////////////////////////////////
hFILE FSOpen(const tCHAR *filepath, const tCHAR *mode)
{
	if(!filepath) return 0;

	hFILE fp=(hFILE)MemAlloc(sizeof(FSFile));

	if(fp)
	{
		wstring path;

		//if there are no paths
		if(!g_main || g_main->paths->size() == 0)
		{
			FSPath fsPath;
			fsPath.flag = 0;
			fsPath.uf = 0;
			fsPath.str = L"\0";

			//try to open the file
			if(_Open(fp, fsPath, filepath, mode) != TRUE)
			{ FSClose(fp); fp = 0; }
		}
		else
		{
			s8 ret = FALSE;

			//go through the list to find where this file is
			for(list<FSPath>::iterator i = g_main->paths->begin(); i != g_main->paths->end(); i++)
			{
				//try to open the file
				ret = _Open(fp, (*i), filepath, mode);

				if(ret != FALSE)
					break;
			}

			//failed to find the file
			if(ret == FALSE || ret == -1) { FSClose(fp); fp = 0; }
		}
	}

	return fp;
}

/////////////////////////////////////
// Name:	FSClose
// Purpose:	close the file
// Output:	file closed
// Return:	none
/////////////////////////////////////
void FSClose(hFILE fp)
{
	if(fp)
	{
		if(fp->fp) fclose(fp->fp);

		if(fp->buff) MemFree(fp->buff);

		MemFree(fp);
	}
}