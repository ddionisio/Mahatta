#include "common.h"
#include "STD\log.h"

#define LOG_MAX_LINE		2048

//A simple log interface for the game

wstring g_dump;

vector<wstring> g_lines;

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
void LogClear()
{
	g_lines.clear();
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
void LogPrintf(u32 flags, const tCHAR *str, ...)
{
	tCHAR buff[512];								// Holds Our String

	//remove first element if line number exceeds
	if(g_lines.size() == LOG_MAX_LINE)
		g_lines.erase(g_lines.begin());

	//do the printf thing
	va_list valist;
	va_start(valist, str);
	_vsnwprintf(buff, sizeof(buff), str, valist);
	va_end(valist);

	g_lines.push_back(buff);

	//debug purpose...
	if(TESTFLAGS(flags, LOG_FILE))
	{
		FILE *fp = _wfopen(L"dbg_log.txt", L"a");
		if(fp)
		{
			fwprintf(fp, L"%s\n", buff);

			fclose(fp);
		}
	}

	if(TESTFLAGS(flags, LOG_ASSERT))
		ASSERT_MSG(0, buff, 0);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
void LogMsg(u32 flags, const tCHAR *header, const tCHAR *msg)
{
	//remove first element if line number exceeds
	if(g_lines.size() == LOG_MAX_LINE)
		g_lines.erase(g_lines.begin());

	wstring line;

	if(header)
	{
		line += L"<";
		line += header;
		line += L">: ";
	}
	
	if(msg)
		line += msg;

	g_lines.push_back(line);
	
	//debug purpose...
	if(TESTFLAGS(flags, LOG_FILE))
	{
		FILE *fp = _wfopen(L"dbg_log.txt", L"a");
		if(fp)
		{
			fwprintf(fp, L"%s\n", line.c_str());

			fclose(fp);
		}
	}

	if(TESTFLAGS(flags, LOG_ASSERT))
		ASSERT_MSG(0, msg, header);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
u32 LogGetNumLine()
{
	return (u32)g_lines.size();
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
const tCHAR *LogGetLine(u32 num)
{
	return g_lines[num].c_str();
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
void LogDumpMsgToFile(const tCHAR *filename, u8 bAppend)
{
	FILE *fp = bAppend ? _wfopen(filename, L"a") : _wfopen(filename, L"wt");

	if(fp)
	{
		for(u32 i = 0; i < g_lines.size(); i++)
			fwprintf(fp, L"%s\n", LogGetLine(i));

		fclose(fp);
	}
}