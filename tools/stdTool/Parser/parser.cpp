#include "i_parser.h"

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
									   const tCHAR beginat, const tCHAR stopat)
{
	assert(fp);
	assert(buff);

	memset(buff, 0, sizeof(tCHAR)*buffSize);

	tCHAR ch;
	s32 i = 0;

	//get the begin at, if the user specified it
	if(beginat)
	{
		while(1)
		{
			if(FSEOF(fp))				//that means we have reached the end of file
				return RETCODE_FAILURE;

			ch = FSGetChar(fp);

			//if(ch==0)
			//	return RETCODE_FAILURE;

			if(ch == beginat) //if we found our start, break
				break;
			else if(ch == stopat)		//that means there is nothing to get
				return RETCODE_ENDREACHED;
		}
	}

	while(i < buffSize)
	{
		if(FSEOF(fp))				//that means we have reached the end of file
				return RETCODE_FAILURE;

		ch = FSGetChar(fp);

		//if(ch==0)
		//		return RETCODE_FAILURE;

		if(ch == stopat)
			return RETCODE_ENDREACHED; //we have reached the stopat.
		else
		{
			//otherwise, copy it to buff
			if(i < buffSize)
				buff[i] = ch;
			i++;
		}
	}

	return RETCODE_NOTFOUND;
}

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
									   const tCHAR beginat, const tCHAR stopat)
{
	assert(fp);
	assert(numChar);

	*numChar = 0;

	s64 fPos;

	FSGetPos(fp, &fPos);

	tCHAR ch;

	//get the begin at, if the user specified it
	if(beginat)
	{
		while(1)
		{
			if(FSEOF(fp))				//that means we have reached the end of file
			{ FSSetPos(fp, &fPos); return RETCODE_FAILURE; }

			ch = FSGetChar(fp);

			//if(ch==0)
			//	return RETCODE_FAILURE;

			if(ch == beginat) //if we found our start, break
				break;
			else if(ch == stopat)		//that means there is nothing to get
			{ FSSetPos(fp, &fPos); return RETCODE_ENDREACHED; }
		}
	}

	while(1)
	{
		if(FSEOF(fp))				//that means we have reached the end of file
		{ FSSetPos(fp, &fPos); return RETCODE_FAILURE; }

		ch = FSGetChar(fp);

		//if(ch==0)
		//		return RETCODE_FAILURE;

		if(ch == stopat)
		{ FSSetPos(fp, &fPos); return RETCODE_ENDREACHED; } //we have reached the stopat.
		else
			*numChar += 1;
	}

	FSSetPos(fp, &fPos);
	return RETCODE_NOTFOUND;
}

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
tCHAR ParserReadWordFile(hFILE fp, tCHAR *buff, s32 buffSize, const tCHAR stopat)
{
	assert(fp);
	assert(buff);

	memset(buff, 0, sizeof(tCHAR)*buffSize);

	tCHAR ch;

	//get the first letter
	while(1)
	{
		if(FSEOF(fp))				//that means we have reached the end of file
			return 0;

		ch = FSGetChar(fp);

		if(ch != ' ' && ch != '\n' && ch != CHAR_TAB) //if not any of these, we found our start
		{ buff[0] = ch; break; }
		else if((stopat != ' ' && stopat != '\n' && stopat != CHAR_TAB) 
			&& ch == stopat)		//that means there is nothing to get
			return stopat;
	}

	//now fill in the rest of the word
	s32 i = 1;

	while(1)
	{
		if(FSEOF(fp) || i >= buffSize)//that means we have reached the end of file or buff size
			return 0;

		ch = FSGetChar(fp);

		if(ch == stopat)
		{
			if(ch == stopat)
				return ch;				//we have 	//we have reached the new line, now we must find the stopat
			else
				break;
		}
		else if(ch == '\n' || ch == ' ' || ch == CHAR_TAB)
		{
			if(ch == stopat)
				return ch;				//we have 	//we have reached the new line, now we must find the stopat
			else
				break;
		}
		else if(ch != -1)
		{
			//otherwise, copy it to buff
			buff[i] = ch;
			i++;
		}
	}

	//if we get here, that means we must find the stopat or if we find something else
	//other than '\n' or ' ', return that tCHAR instead
	//find it only if the user specified
	if(stopat)
	{
		while(1)
		{
			if(FSEOF(fp))
				break;

			ch = FSGetChar(fp);

			if(ch == stopat)
				return stopat;				 //we have reached the stopat.  This is what we want
			else if(ch != ' ' && ch != '\n' && ch != CHAR_TAB)
				return ch;					 //if not ' ' or '\n', we found something else
		}
	}
	else
		return ch;

	return 0; //we have reached the end of file on the last loop
}

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
tCHAR ParserCountWordFile(hFILE fp, s32 *numChar, const tCHAR stopat)
{
	assert(fp);
	assert(numChar);

	*numChar = 0;

	s64 fPos;

	FSGetPos(fp, &fPos);

	tCHAR ch;

	//get the first letter
	while(1)
	{
		if(FSEOF(fp))				//that means we have reached the end of file
		{ FSSetPos(fp, &fPos); return 0; }

		ch = FSGetChar(fp);

		if(ch != ' ' && ch != '\n' && ch != CHAR_TAB) //if not any of these, we found our start
		{ *numChar += 1; break; }
		else if((stopat != ' ' && stopat != '\n' && stopat != CHAR_TAB) 
			&& ch == stopat)		//that means there is nothing to get
		{ FSSetPos(fp, &fPos); return stopat; }
	}

	//now fill in the rest of the word
	while(1)
	{
		if(FSEOF(fp))//that means we have reached the end of file
		{ FSSetPos(fp, &fPos); return 0; }

		ch = FSGetChar(fp);

		if(ch == stopat)
		{ FSSetPos(fp, &fPos); return stopat; }          //we have reached the stopat.  This is what we want
		else if(ch == '\n' || ch == ' ' || ch == CHAR_TAB)
		{
			if(ch == stopat)
				return ch;				//we have 	//we have reached the new line, now we must find the stopat
			else
				break;
		}
			/*break;					//we have reached the new line, now we must find the stopat
		else if(ch == ' ')
			break;					//we have reached a space, now we must find the stopat*/
		else
		{
			*numChar += 1;
		}
	}

	//if we get here, that means we must find the stopat or if we find something else
	//other than '\n' or ' ', return that tCHAR instead
	//find it only if the user specified
	if(stopat)
	{
		while(!FSEOF(fp))
		{
			ch = FSGetChar(fp);

			if(ch == stopat)
			{ FSSetPos(fp, &fPos); return stopat; }				 //we have reached the stopat.  This is what we want
			else if(ch != ' ' && ch != '\n' && ch != CHAR_TAB)
			{ FSSetPos(fp, &fPos); return ch; }					 //if not ' ' or '\n', we found something else
		}
	}

	FSSetPos(fp, &fPos);
	return 0; //we have reached the end of file on the last loop
}

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
RETCODE ParserSkipCharFile(hFILE fp, const tCHAR thing, BOOL returnFalseOnOthers)
{
	if(thing)
	{
		tCHAR ch;
		//find the thing
		while(1)
		{
			if(FSEOF(fp)) //that means we have reached the end of file :(
				return RETCODE_FAILURE;

			ch = FSGetChar(fp);
			
			if(ch == thing)
				break; //we are good to go...
			else if(returnFalseOnOthers 
				&& (ch != '\n' && ch != ' ' && ch != CHAR_TAB)) //that means we found something else...which is bad...
				return RETCODE_FAILURE;
		}
	}

	return RETCODE_SUCCESS;
}


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
RETCODE ParserReadDataFile(hFILE fp, void *dest, eDataType dType, const tCHAR stopAt)
{
	tCHAR buff[MAXCHARBUFF];
	tCHAR retChar;

	retChar = ParserReadWordFile(fp, buff, MAXCHARBUFF, stopAt);

	//if retChar is not equal to stopAt display error
	if(retChar != stopAt)
		return RETCODE_FAILURE;

	switch(dType)
	{
	case dataINT:
		{
			s32 i;
			swscanf(buff, L"%d", &i); //this better be right
			memcpy(dest, &i, sizeof(s32));
		}
		break;
	case dataSHORT:
		{
			s16 i;
			swscanf(buff, L"%hd", &i); //this better be right
			memcpy(dest, &i, sizeof(s16));
		}
		break;
	case dataFLOAT:
		{
			float i;
			swscanf(buff, L"%f", &i); //this better be right
			memcpy(dest, &i, sizeof(float));
		}
		break;
	case dataDOUBLE:
		{
			double i;
			swscanf(buff, L"%lf", &i); //this better be right
			memcpy(dest, &i, sizeof(double));
		}
		break;
	}

	//
	//if(fread(data, size, numcount, fp) != numcount)
	//{ assert(!"error reading data in ParserReadDataFile"); return RETCODE_FAILURE; }

	return RETCODE_SUCCESS;//ParserSkipCharFile(fp, stopAt, true);
}

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
BOOL ParserSkipString(hFILE fp, const tCHAR beginAt, const tCHAR stopAt, const tCHAR *string)
{
	tCHAR buff[MAXCHARBUFF];

	while(1)
	{
		if(ParserReadStringFile(fp, buff, MAXCHARBUFF, beginAt, stopAt) == RETCODE_FAILURE) //we have reached end of file
		{ return false; }

		if(_wcsicmp(buff, string) == 0) //we are there!
			return true;
	}

	return false;
}

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
									   const tCHAR beginat, const tCHAR stopat)
{
	assert(theBuff);
	assert(buff);

	memset(buff, 0, sizeof(tCHAR)*buffSize);

	tCHAR ch;
	s32 i = 0;

	//get the begin at, if the user specified it
	if(beginat)
	{
		while(1)
		{
			if(**theBuff == 0)				//that means we have reached the end of theBuff
				return RETCODE_FAILURE;

			ch = **theBuff;
			*theBuff += 1;

			//if(ch==0)
			//	return RETCODE_FAILURE;

			if(ch == beginat) //if we found our start, break
				break;
			else if(ch == stopat)		//that means there is nothing to get
				return RETCODE_ENDREACHED;
		}
	}

	while(i < buffSize)
	{
		if(**theBuff == 0)				//that means we have reached the end of theBuff
				return RETCODE_FAILURE;

		ch = **theBuff;
		*theBuff += 1;

		//if(ch==0)
		//		return RETCODE_FAILURE;

		if(ch == stopat)
			return RETCODE_ENDREACHED; //we have reached the stopat.
		else
		{
			//otherwise, copy it to buff
			if(i < buffSize-1)
				buff[i] = ch;
			i++;
		}
	}

	return RETCODE_NOTFOUND;
}

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
									   const tCHAR beginat, const tCHAR stopat)
{
	assert(theBuff);
	assert(numChar);

	*numChar = 0;

	tCHAR ch;

	//get the begin at, if the user specified it
	if(beginat)
	{
		while(1)
		{
			if(*theBuff == 0)				//that means we have reached the end of theBuff
				return RETCODE_FAILURE;

			ch = *theBuff;
			theBuff++;

			//if(ch==0)
			//	return RETCODE_FAILURE;

			if(ch == beginat) //if we found our start, break
				break;
			else if(ch == stopat)		//that means there is nothing to get
				return RETCODE_ENDREACHED;
		}
	}

	while(1)
	{
		if(*theBuff == 0)				//that means we have reached the end of theBuff
				return RETCODE_FAILURE;

		ch = *theBuff;
		theBuff++;

		//if(ch==0)
		//		return RETCODE_FAILURE;

		if(ch == stopat)
			return RETCODE_ENDREACHED; //we have reached the stopat.
		else
		{
			*numChar += 1;
		}
	}

	return RETCODE_NOTFOUND;
}

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
tCHAR ParserReadWordBuff(tCHAR **theBuff, tCHAR *buff, s32 buffSize, const tCHAR stopat)
{
	assert(theBuff);
	assert(buff);

	memset(buff, 0, sizeof(tCHAR)*buffSize);

	tCHAR ch;

	//get the first letter
	while(1)
	{
		if(**theBuff == 0)				//that means we have reached the end of theBuff
			return 0;

		ch = **theBuff;
		*theBuff += 1;

		if(ch != ' ' && ch != '\n' && ch != CHAR_TAB) //if not any of these, we found our start
		{ buff[0] = ch; break; }
		else if((stopat != ' ' && stopat != '\n' && stopat != CHAR_TAB) 
			&& ch == stopat)		//that means there is nothing to get
			return stopat;
	}

	//now fill in the rest of the word
	s32 i = 1;

	while(1)
	{
		if(**theBuff == 0 || i >= buffSize)//that means we have reached the end of theBuff or buff size
			return 0;

		ch = **theBuff;
		*theBuff += 1;

		if(ch == stopat)
			return stopat;          //we have reached the stopat.  This is what we want
		else if(ch == '\n' || ch == ' ' || ch == CHAR_TAB)
		{
			if(ch == stopat)
				return ch;				//we have 	//we have reached the new line, now we must find the stopat
			else
				break;
		}
		else if(ch != -1)
		{
			//otherwise, copy it to buff
			buff[i] = ch;
			i++;
		}
	}

	//if we get here, that means we must find the stopat or if we find something else
	//other than '\n' or ' ', return that tCHAR instead
	//find it only if the user specified
	if(stopat)
	{
		while(**theBuff != 0)
		{
			ch = **theBuff;
			*theBuff += 1;

			if(ch == stopat)
				return stopat;				 //we have reached the stopat.  This is what we want
			else if(ch != ' ' && ch != '\n' && ch != CHAR_TAB)
				return ch;					 //if not ' ' or '\n', we found something else
		}
	}
	else
		return ch;

	return 0; //we have reached the end of file on the last loop
}

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
tCHAR ParserCountWordBuff(const tCHAR *theBuff, s32 *numChar, const tCHAR stopat)
{
	assert(theBuff);
	assert(numChar);

	*numChar = 0;

	tCHAR ch;

	//get the first letter
	while(1)
	{
		if(*theBuff == 0)				//that means we have reached the end of theBuff
			return 0;

		ch = *theBuff;
		theBuff++;

		if(ch != ' ' && ch != '\n' && ch != CHAR_TAB) //if not any of these, we found our start
		{ *numChar += 1; break; }
		else if((stopat != ' ' && stopat != '\n' && stopat != CHAR_TAB) 
			&& ch == stopat)		//that means there is nothing to get
			return stopat;
	}

	//now fill in the rest of the word

	while(1)
	{
		if(*theBuff == 0)//that means we have reached the end of theBuff
			return 0;

		ch = *theBuff;
		theBuff++;

		if(ch == stopat)
			return stopat;          //we have reached the stopat.  This is what we want
		else if(ch == '\n' || ch == ' ' || ch == CHAR_TAB)
		{
			if(ch == stopat)
				return ch;				//we have 	//we have reached the new line, now we must find the stopat
			else
				break;
		}
		/*	break;					//we have reached the new line, now we must find the stopat
		else if(ch == ' ')
			break;					//we have reached a space, now we must find the stopat
			*/
		else
		{
			*numChar += 1;
		}
	}

	//if we get here, that means we must find the stopat or if we find something else
	//other than '\n' or ' ', return that tCHAR instead
	//find it only if the user specified
	if(stopat)
	{
		while(*theBuff != 0)
		{
			ch = *theBuff;
			theBuff++;

			if(ch == stopat)
				return stopat;				 //we have reached the stopat.  This is what we want
			else if(ch != ' ' && ch != '\n' && ch != CHAR_TAB)
				return ch;					 //if not ' ' or '\n', we found something else
		}
	}

	return 0; //we have reached the end of file on the last loop
}

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
RETCODE ParserSkipCharBuff(tCHAR **theBuff, const tCHAR thing, BOOL returnFalseOnOthers)
{
	if(thing)
	{
		tCHAR ch;
		//find the thing
		while(1)
		{
			if(**theBuff == 0) //that means we have reached the end of theBuff :(
				return RETCODE_FAILURE;

			ch = **theBuff;
			*theBuff += 1;
			
			if(ch == thing)
				break; //we are good to go...
			else if(returnFalseOnOthers 
				&& (ch != '\n' && ch != ' ' && ch != CHAR_TAB)) //that means we found something else...which is bad...
				return RETCODE_FAILURE;
		}
	}

	return RETCODE_SUCCESS;
}

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
RETCODE ParserReadDataBuff(tCHAR **theBuff, void *dest, eDataType dType, const tCHAR stopAt)
{
	tCHAR buff[MAXCHARBUFF];
	tCHAR retChar;

	retChar = ParserReadWordBuff(theBuff, buff, MAXCHARBUFF, stopAt);

	//if retChar is not equal to stopAt display error
	if(retChar != stopAt)
		return RETCODE_FAILURE;

	switch(dType)
	{
	case dataINT:
		{
			s32 i;
			swscanf(buff, L"%d", &i); //this better be right
			memcpy(dest, &i, sizeof(s32));
		}
		break;
	case dataSHORT:
		{
			s16 i;
			swscanf(buff, L"%hd", &i); //this better be right
			memcpy(dest, &i, sizeof(s16));
		}
		break;
	case dataFLOAT:
		{
			float i;
			swscanf(buff, L"%f", &i); //this better be right
			memcpy(dest, &i, sizeof(float));
		}
		break;
	case dataDOUBLE:
		{
			double i;
			swscanf(buff, L"%lf", &i); //this better be right
			memcpy(dest, &i, sizeof(double));
		}
		break;
	}

	//
	//if(fread(data, size, numcount, fp) != numcount)
	//{ assert(!"error reading data in ParserReadDataFile"); return RETCODE_FAILURE; }

	return RETCODE_SUCCESS;//ParserSkipCharFile(fp, stopAt, true);
}

// This function returns the file portion of the filename.  If there is
// no filename (ie; if PathName is an empty string, or if PathName appears
// to name a directory (ie; final character is a backslash)) then it returns
// a pointer to the null character at the end of the filename (ie; a null
// string).  If there are no backslashes then it is assumed that the entire
// name represents a filename, so the entire string is returned. This function
// will never return zero. You can remove the filename from any path by
// writing zero to the tCHAR pointed at by the return value.

tCHAR *GetFilePart(const tCHAR *PathName)
{
	assert(PathName);
	tCHAR* LastSlash = (tCHAR *)wcsrchr(PathName, '\\');

	tCHAR* LastSlashR = (tCHAR *)wcsrchr(PathName, '/');

	if(LastSlash 
		&& LastSlashR 
		&& wcslen(LastSlash) < wcslen(LastSlashR))
		return LastSlash + 1;
	else if(LastSlashR)
		return LastSlashR + 1;
	else if(LastSlash)
		return LastSlash + 1;

	/*if (LastSlash)
		return LastSlash + 1;
	else
	{
		

		if (LastSlash)
			return LastSlash + 1;
	}*/
	return (tCHAR *)PathName;
}

// This function returns the file extension, pointing at the period, if there
// is one.  If there is no file extension, it points to the null character at
// the end of the filename (ie; a null string).

tCHAR *GetExtension(const tCHAR *PathName)
{
	assert(PathName);
	tCHAR* FilePart = GetFilePart(PathName);
	assert(FilePart);
	tCHAR* LastPeriod = wcsrchr(FilePart, '.');
	if (LastPeriod)
		return LastPeriod;
	return FilePart + wcslen(FilePart);
}

void GetFilename(const tCHAR *PathName, tCHAR *str, s32 strLen)
{
	tCHAR *filePart=GetFilePart(PathName);

	//copy till we see a period, or string ends
	for(s32 i = 0; i < strLen; i++)
	{
		if(filePart[i] == '.')
			break;

		str[i]=filePart[i];
	}
}

void ConcatinateFilePath(const tCHAR *path, tCHAR *relativePath,
								tCHAR *out)
{
	if(relativePath[0] == 0)
		return;

	wcscpy(out, path);

	//get the slash at the end
	tCHAR* LastSlash = (tCHAR *)wcsrchr(out, '\\');

	if(!LastSlash)
		LastSlash = out;

	*LastSlash = 0; //set to null

	//now check the relative path
	tCHAR dir[4];
	tCHAR *pRelative;

	for(pRelative = relativePath; *pRelative != 0;)
	{
		memset(dir, 0, sizeof(tCHAR)*4);

		dir[0] = *pRelative;

		//check to see if it's not a '.' or '\\'
		if(dir[0] == '.' || dir[0] == '\\')
		{
			pRelative++;
			//check to see if we have a slash
			if(dir[0] == '\\')
				break;
			
			dir[1] = *pRelative;	pRelative++;

			//check for ".\\"
			if(_wcsicmp(dir, L".\\") == 0)
				continue;
			
			dir[2] = *pRelative;	pRelative++;

			//check for "..\\"
			if(_wcsicmp(dir, L"..\\") == 0)
			{
				//go back one directory
				LastSlash = (tCHAR *)wcsrchr(out, '\\');

				if(!LastSlash)
					LastSlash = out;

				*LastSlash = 0;
			}
		}
		else
			break;
	}

	if(out[0] != 0)
		wcscat(out, L"\\");

	wcscat(out, pRelative);
}

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
tCHAR * ParserGrabAllTextFile(const tCHAR *filename)
{
	s64 len;
	tCHAR *buf;
	hFILE fp;
	fp =  FSOpen(filename, L"rt");
	if(!fp)
		return NULL;
	
	FSSeek(fp, 0, FS_SEEK_END);
	FSGetPos(fp, &len);
	FSSeek(fp, 0, FS_SEEK_SET);
	buf = (tCHAR*)MemAlloc((s32)(len+1) * sizeof(s32));
	FSRead(buf, (s32) len, 1, fp);
	buf[len] = 0;
	return buf;
}