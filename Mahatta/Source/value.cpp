#include "engine_common.h"

#include "value.h"

#include "command.h"

//////////////////////////////////////////////////////////
//Value Globals
struct ValDat {
	ValDat() : str(L"\0"), val(0), flag(0) {}

	wstring str;	//string version of val
	f32	   val;		//float version of val
	FLAGS  flag;	//value flags (for user/client protection, etc...)
};

typedef map<wstring, ValDat>::iterator ValIt;
map<wstring, ValDat> g_vals;

////////////////////////////////////////////////////////////////
// Value Interface
//

/////////////////////////////////////
// Name:	ValueSetFlags
// Purpose:	set existing value's
//			flags.
// Output:	value's flag set
// Return:	none
/////////////////////////////////////
void ValueSetFlags(const tCHAR *name, u32 flag)
{
	if(ValueExist(name))
		g_vals[name].flag = flag;
}

/////////////////////////////////////
// Name:	ValueGetFlags
// Purpose:	get value's flags.
// Output:	none
// Return:	the flags
/////////////////////////////////////
u32 ValueGetFlags(const tCHAR *name)
{
	if(ValueExist(name))
		return g_vals[name].flag;

	return 0;
}

/////////////////////////////////////
// Name:	ValueExist
// Purpose:	check to see if given
//			value exists
// Output:	none
// Return:	TRUE if so
/////////////////////////////////////
u8 ValueExist(const tCHAR *name)
{
	//check in string list
	ValIt it = g_vals.find(name);

	return it != g_vals.end();
}

/////////////////////////////////////
// Name:	ValueSetStr
// Purpose:	set given value's string
//			NOTE: if given name does
//			not exist, it will be created
// Output:	value is set
// Return:	TRUE if success
/////////////////////////////////////
s32 ValueSetStr(const tCHAR *name, const tCHAR *str)
{
	tCHAR *strStop;
	
	g_vals[name].val = (f32)wcstod(str, &strStop);

	g_vals[name].str = str;

	return TRUE;
}

/////////////////////////////////////
// Name:	ValueSetFloat
// Purpose:	set given value's float
//			NOTE: if given name does
//			not exist, it will be created
// Output:	value is set
// Return:	TRUE if success
/////////////////////////////////////
s32 ValueSetFloat(const tCHAR *name, f32 val)
{
	tCHAR buff[MAXCHARBUFF]; swprintf(buff, L"%f", val);
	
	g_vals[name].str = buff;
	g_vals[name].val = val;

	return TRUE;
}

/////////////////////////////////////
// Name:	ValueGetStr
// Purpose:	get given value's string
// Output:	strOut is set
// Return:	TRUE if success
/////////////////////////////////////
s32 ValueGetStr(const tCHAR *name, tCHAR *strOut, u32 size)
{
	if(ValueExist(name))
	{
		
		wstring & str = g_vals[name].str;
		
		if(size >= str.size())
		{
			memcpy(strOut, str.c_str(), sizeof(tCHAR)*str.size());
			strOut[str.size()] = 0;
		}
		else
		{
			memcpy(strOut, str.c_str(), sizeof(tCHAR)*size);
			strOut[size-1] = 0;
		}
		
		//wcscpy(strOut, g_vals[name].str.c_str());
		return TRUE;
	}

	return FALSE;
}

/////////////////////////////////////
// Name:	ValueGetFloat
// Purpose:	get given value's float
// Output:	fOut is set
// Return:	TRUE if success
/////////////////////////////////////
s32 ValueGetFloat(const tCHAR *name, f32 *fOut)
{
	if(ValueExist(name))
	{
		*fOut = g_vals[name].val;
		return TRUE;
	}

	return FALSE;
}

/***********************************************************************/

// Basic Commands

void CMD_Set()
{
	if(CmdArgC() != 2) 
	{
		LogMsg(0, 0, L"usage: set <variable> <value>.");
		return;
	}

	ValueSetStr(CmdArgV(0), CmdArgV(1));
}

//set as archived (save to autoexec.cfg)
void CMD_SetA()
{
	if(CmdArgC() != 2) 
	{
		LogMsg(0, 0, L"usage: seta <variable> <value>.");
		return;
	}

	ValueSetStr(CmdArgV(0), CmdArgV(1));
	ValueSetFlags(CmdArgV(0), VALUE_FLAG_ARCHIVE);
}

void CMD_Toggle()
{
	if(CmdArgC() != 1) 
	{
		LogMsg(0, 0, L"usage: toggle <variable>.");
		return;
	}

	f32 val;

	if(ValueGetFloat(CmdArgV(0), &val))
	{
		ValueSetFloat(CmdArgV(0), val ? 0.0f : 1.0f);
	}
	else
		LogPrintf(0, L"value %s does not exist", CmdArgV(0));
}

void CMD_VarList()
{
	for(ValIt it = g_vals.begin(); it != g_vals.end(); it++)
	{
		wstring attr;

		if(TESTFLAGS((*it).second.flag, VALUE_FLAG_ARCHIVE))
			attr += 'A';
		else
			attr += ' ';

		LogPrintf(0, L"%s %s \"%s\"", attr.c_str(), (*it).first.c_str(), (*it).second.str.c_str());
	}

	LogPrintf(0, L"%d variables", g_vals.size());
}

/***********************************************************************/

/////////////////////////////////////
// Name:	ValueCmdInit
// Purpose:	initialize value basic
//			commands
// Output:	more basic commands (like set)
// Return:	none
/////////////////////////////////////
void ValueCmdInit()
{
	CmdAdd(L"set", CMD_Set);
	CmdAdd(L"seta", CMD_SetA);
	CmdAdd(L"toggle", CMD_Toggle);
	CmdAdd(L"varlist", CMD_VarList);
}

/////////////////////////////////////
// Name:	ValueSaveArchived
// Purpose:	save all values with
//			flag VALUE_FLAG_ARCHIVE
// Output:	values added to file
// Return:	none
/////////////////////////////////////
void ValueSaveArchived(hFILE fp)
{
	for(ValIt it = g_vals.begin(); it != g_vals.end(); it++)
		FSPrintf(fp, L"seta %s \"%s\"\n", (*it).first.c_str(), (*it).second.str.c_str());
}