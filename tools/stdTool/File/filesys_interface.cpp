#include "filesys_i.h"

/////////////////////////////////////
// Name:	FSIsArchive
// Purpose:	determine if given fp is
//			loaded from archive.
// Output:	none
// Return:	TRUE if so.
/////////////////////////////////////
u8 FSIsArchive(hFILE fp)
{
	if(fp->fp) return FALSE;

	return TRUE;
}

/////////////////////////////////////
// Name:	FSFileGet
// Purpose:	in case you really need
//			the FILE *, then call this.
//			This is NULL if fp is from
//			archive (FSIsArchive=TRUE)
// Output:	none
// Return:	the FILE *
/////////////////////////////////////
FILE *FSFileGet(hFILE fp) { return fp->fp; }

/////////////////////////////////////
// Name:	FSBufferGetPtr
// Purpose:	this is only used for 
//			archive fp's (FSIsArchive=TRUE)
//			For external loading of data
// Output:	none
// Return:	the buffer
/////////////////////////////////////
u8 *FSBufferGetPtr(hFILE fp) { return fp->buff; }

/////////////////////////////////////
// Name:	FSBufferGetSize
// Purpose:	get the size of the buffer
//			only archive fp's (FSIsArchive=TRUE)
// Output:	none
// Return:	the size of the buffer (bytes)
/////////////////////////////////////
u32 FSBufferGetSize(hFILE fp) { return fp->buffSize; }

/////////////////////////////////////
// Name:	FSRead
// Purpose:	read bytes from file
//			size = size in bytes
//			count = num items
// Output:	buffer filled, file updated
// Return:	number of items read
//			may be less than count
/////////////////////////////////////
u64 FSRead(void *buffer, u64 size, u64 count, hFILE fp)
{
	if(FSIsArchive(fp))
	{
		u32 sSize = (u32)(size*count);

		if(sSize > fp->buffSize-fp->buffInd)
			sSize = fp->buffSize-fp->buffInd;

		memcpy(buffer, &fp->buff[fp->buffInd], sSize);
		
		fp->buffInd += sSize;
		
		return sSize;
	}

	return fread(buffer, (size_t)size, (size_t)count, fp->fp);
}

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
u64 FSWrite(void *buffer, u64 size, u64 count, hFILE fp)
{
	if(FSIsArchive(fp))
	{
		//don't allow
		return 0;
	}

	return fwrite(buffer, (size_t)size, (size_t)count, fp->fp);
}

/////////////////////////////////////
// Name:	FSEndOfFile
// Purpose:	did we reach end of file?
// Output:	none
// Return:	TRUE if so
/////////////////////////////////////
u8 FSEOF(hFILE fp)
{
	if(FSIsArchive(fp))
	{
		return (fp->buffInd == fp->buffSize);
	}

	return feof(fp->fp);
}

/////////////////////////////////////
// Name:	FSGetChar
// Purpose:	get one tCHAR
// Output:	file updated
// Return:	tCHAR
/////////////////////////////////////
s32 FSGetChar(hFILE fp)
{
	s32 c;

	if(FSIsArchive(fp))
	{
		if(FSEOF(fp)) return 0;

		if(fp->uCType != UC_None)
		{ 
			do
			{
				//skip the magic number for unicode
				if(fp->buffInd == 0) fp->buffInd += sizeof(u16);
				//re-align buffInd if we have to
				else if(fp->buffInd%2 != 0) fp->buffInd++;

				switch(fp->uCType)
				{
				case UC_Base:
					c = (((u16)fp->buff[fp->buffInd+1])<<8)|((u16)(fp->buff[fp->buffInd]&0xff));
					break;
				case UC_BigEndian:
					c = (((u16)fp->buff[fp->buffInd])<<8)|((u16)(fp->buff[fp->buffInd+1]&0xff));
					break;
				}

				fp->buffInd += sizeof(u16);
			} while(c==13); //skip carriage returns
		}
		else
		{ 
			do 
			{
				c = fp->buff[fp->buffInd]; 
				fp->buffInd++; 
			} while(c==13); //skip carriage returns
		}
	}
	else
	{
		do
		{
			c = fgetwc(fp->fp);

			switch(fp->uCType)
			{
				case UC_BigEndian:
					{
						//swap bytes
						u16 sc = (c>>8)&0x00ff;
						c = ((((u16)c)<<8)&0xff00)|sc;
					}
					break;
			}

		} while(c==13); //skip carriage returns
	}

	return c;
}

/////////////////////////////////////
// Name:	FSPutChar
// Purpose:	put one tCHAR
// Output:	file updated
// Return:	the tCHAR written
/////////////////////////////////////
s32 FSPutChar(s32 c, hFILE fp)
{
	if(FSIsArchive(fp))
	{
		//don't allow
		return 0;
	}

	return fputwc(c, fp->fp);
}

/////////////////////////////////////
// Name:	FSSeek
// Purpose:	seek file position
//	FS_SEEK_SET = beginning
//	FS_SEEK_CUR = current
//	FS_SEEK_END = end
// Output:	file updated
// Return:	0 if success
/////////////////////////////////////
s32 FSSeek(hFILE fp, s32 offset, FS_SEEK origin)
{
	if(FSIsArchive(fp))
	{
		u32 prev = fp->buffInd;

		switch(origin)
		{
		case FS_SEEK_SET:
			fp->buffInd = offset;
			break;
		case FS_SEEK_CUR:
			fp->buffInd += ((s32)fp->buffInd)+offset;
			break;
		case FS_SEEK_END:
			fp->buffInd = ((s32)fp->buffSize)+offset;
			break;
		}

		if(fp->buffInd > fp->buffSize)
			return -1;

		return 0;
	}

	return fseek(fp->fp, offset, origin);
}

/////////////////////////////////////
// Name:	FSGetPos
// Purpose:	get file position
// Output:	pos set
// Return:	0 if success
/////////////////////////////////////
s32 FSGetPos(hFILE fp, s64 *pos)
{
	if(FSIsArchive(fp))
	{
		*pos = fp->buffInd;
		return 0;
	}

	return fgetpos(fp->fp, pos);
}

/////////////////////////////////////
// Name:	FSSetPos
// Purpose:	set file position
// Output:	file updated
// Return:	0 if success
/////////////////////////////////////
s32 FSSetPos(hFILE fp, s64 *pos)
{
	if(FSIsArchive(fp))
	{
		fp->buffInd = (u32)(*pos);

		if(fp->buffInd > fp->buffSize)
			return -1;

		return 0;
	}

	return fsetpos(fp->fp, pos);
}

/////////////////////////////////////
// Name:	FSPrintf
// Purpose:	print string to fp
// Output:	fp updated
// Return:	number of bytes written
/////////////////////////////////////
s32 FSPrintf(hFILE fp, const tCHAR *str, ...)
{
	tCHAR buff[512];								// Holds Our String

	//do the printf thing
	va_list valist;
	va_start(valist, str);
	_vsnwprintf(buff, sizeof(buff), str, valist);
	va_end(valist);

	if(FSIsArchive(fp))
	{
		//do not allow
		return 0;
	}

	return fwprintf(fp->fp, buff);
}