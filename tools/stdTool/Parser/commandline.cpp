#include "i_parser.h"

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
									 void *userData)
{
	tCHAR bufOut[MAXCHARBUFF]={0};

	tCHAR *buff = (tCHAR*)cmdline;

	//skip the first separator
	if(ParserSkipCharBuff(&buff, separator, true) == RETCODE_SUCCESS)
	{
		while(*buff != 0)
		{
			ParserReadStringBuff(&buff, bufOut, MAXCHARBUFF, 0, separator);
			
			if(callback)
				callback(bufOut, userData);
		}
	}

	return RETCODE_SUCCESS;
}