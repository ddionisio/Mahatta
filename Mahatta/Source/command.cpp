#include "engine_common.h"

#include "command.h"

#include "value.h"

//////////////////////////////////////////////////////////
//Command Globals
typedef map<wstring, COMMANDPROC>::iterator CmdIt;

map<wstring, COMMANDPROC> g_commands;

#define MAX_ARG		32
#define MAX_ARGSTR	128

tCHAR g_argv[MAX_ARG][MAX_ARGSTR];
u32  g_argc;

struct CmdPostDat {
	wstring name;
	wstring param;
};

list<CmdPostDat> g_cmdPosts;		//all posted commands

u8			  g_postWait=FALSE;		//wait for next frame to get to next command

//fill up the global variable argv and argc
static inline void _CmdGetTokenStr(const tCHAR *str)
{
	g_argc = 0; //set to 0 arguments

	tCHAR *buff = (tCHAR*)str;

	for(u8 i = 0; i < MAX_ARG; i++)
	{
		if(*buff == 0)
			break; //done parsing

		//skip spaces, tabs and newlines
		while(*buff == ' ' || *buff == 9 || *buff == '\n')
			buff++;

		//check to see if the first char is '"'
		if(*buff == '"')
			//read until we get the ending '"'
			ParserReadStringBuff(&buff, g_argv[i], MAX_ARGSTR, '"', '"');
		else
			//get the token with space delimiter
			ParserReadWordBuff(&buff, g_argv[i], MAX_ARGSTR, ' ');

		g_argc++;
	}
}

/////////////////////////////////////
// Name:	CmdIsPostWait
// Purpose:	check to see if we should
//			wait.  This will also set
//			postWait back to FALSE
// Output:	g_postWait = FALSE
// Return:	did we want to wait?
/////////////////////////////////////
u8 CmdIsPostWait()
{
	u8 ret = g_postWait;
	g_postWait = FALSE;
	return ret;
}

/////////////////////////////////////
// Name:	CmdAdd
// Purpose:	add a new command with
//			given name
// Output:	new command added
// Return:	TRUE if success
/////////////////////////////////////
s32 CmdAdd(const tCHAR *name, COMMANDPROC proc)
{
	//make sure this command does not exists
	if(!CmdExist(name))
	{
		g_commands[name] = proc;
		return TRUE;
	}

	return FALSE;
}

/////////////////////////////////////
// Name:	CmdExist
// Purpose:	check to see if the given
//			command exists
// Output:	none
// Return:	TRUE if so
/////////////////////////////////////
u8 CmdExist(const tCHAR *name)
{
	CmdIt it = g_commands.find(name);

	return it != g_commands.end();
}

/////////////////////////////////////
// Name:	CmdCall
// Purpose:	call a command with given
//			param (make sure the param
//			matches the command's req.
// Output:	command called
// Return:	return TRUE if command existed
//			and is called
/////////////////////////////////////
s32 CmdCall(const tCHAR *name, const tCHAR *param)
{
	if(CmdExist(name))
	{
		_CmdGetTokenStr(param);
		g_commands[name]();
		return TRUE;
	}

	return FALSE;
}

/////////////////////////////////////
// Name:	CmdCallStr
// Purpose:	call a command from whole
//			string. The first word is
//			the command
// Output:	command called
// Return:	return code from command
/////////////////////////////////////
s32 CmdCallStr(const tCHAR *str)
{
	tCHAR *buff=(tCHAR*)str;
	tCHAR name[MAXCHARBUFF];

	//get the name of command
	ParserReadWordBuff(&buff, name, MAXCHARBUFF, ' ');

	//now parse the params and call the command
	return CmdCall(name, buff);
}

/////////////////////////////////////
// Name:	CmdPost
// Purpose:	post a command with given
//			param (make sure the param
//			matches the command's req.
// Output:	command called
// Return:	TRUE if success
/////////////////////////////////////
s32 CmdPost(const tCHAR *name, const tCHAR *param)
{
	CmdPostDat newPost;
	newPost.name = name;
	newPost.param = param;

	g_cmdPosts.push_back(newPost);

	return TRUE;
}

/////////////////////////////////////
// Name:	CmdPostStr
// Purpose:	post a command from whole
//			string. The first word is
//			the command
// Output:	command called
// Return:	TRUE if success
/////////////////////////////////////
s32 CmdPostStr(const tCHAR *str)
{
	tCHAR *buff=(tCHAR*)str;
	tCHAR name[MAXCHARBUFF];

	//get the name of command
	ParserReadWordBuff(&buff, name, MAXCHARBUFF, ' ');

	return CmdPost(name, buff);
}

/////////////////////////////////////
// Name:	CmdExec
// Purpose:	execute all currently
//			posted commands
// Output:	execute and remove commands
// Return:	TRUE if success
/////////////////////////////////////
s32 CmdExec()
{
	list<CmdPostDat>::iterator i, next;

	for(list<CmdPostDat>::iterator i = g_cmdPosts.begin(); i != g_cmdPosts.end(); i = next)
	{
		next = i; next++;

		CmdCall((*i).name.c_str(), (*i).param.c_str());

		//remove command post
		g_cmdPosts.erase(i);

		if(g_postWait)
		{
			g_postWait = FALSE;
			break; //let's execute others later...
		}
	}

	return TRUE;
}

/////////////////////////////////////
// Name:	CmdExecFile
// Purpose:	execute commands from file
// Output:	stuff executed
// Return:	TRUE if success
/////////////////////////////////////
s32 CmdExecFile(const tCHAR *filename)
{
	hFILE fp = FSOpen(filename, L"rt");

	if(!fp)
		return FALSE;

	tCHAR buff[MAXCHARBUFF];

	while(!FSEOF(fp))
	{
		ParserReadStringFile(fp, buff, MAXCHARBUFF, 0, '\n');
		
		if(buff[0] != 0)
			CmdCallStr(buff);
	}

	//in case we had 'wait' commands in the file, which we should not
	g_postWait = FALSE;

	return TRUE;
}

/////////////////////////////////////
// Name:	CmdClear
// Purpose:	clear out posted commands
// Output:	remove all commands
// Return:	none
/////////////////////////////////////
void CmdClear()
{
	g_cmdPosts.clear();
}

/***********************************************************************/

// Basic Commands

//List out all commands to console
void CMD_List()
{
	for(CmdIt i = g_commands.begin(); i != g_commands.end(); i++)
	{
		LogMsg(0, 0, (*i).first.c_str());
	}

	LogPrintf(0, L"%d commands", g_commands.size());
}

//wait one frame command
void CMD_Wait()
{
	g_postWait = TRUE;
}

//execute a script file
void CMD_Exec()
{
	if(CmdArgC() != 1)
	{
		LogMsg(0, 0, L"exec <filename> : execute a script file.");
		return;
	}

	tCHAR *pStr = CmdArgV(0);

	tCHAR cmdStr[MAXCHARBUFF];

	//check to see if argument 0 is a variable
	if(ValueExist(pStr))
	{
		ValueGetStr(pStr, cmdStr, MAXCHARBUFF);
		pStr = cmdStr;
	}

	LogPrintf(0, L"execing %s", pStr);

	//get filename and execute the commands
	if(!CmdExecFile(pStr))
		LogPrintf(0, L"couldn't exec %s", pStr);
}

//echo parameters
void CMD_Echo()
{
	tCHAR *pStr;
	tCHAR cmdStr[MAXCHARBUFF];

	for(u32 i = 0; i < CmdArgC(); i++)
	{
		pStr = CmdArgV(i);

		//check to see if argument 0 is a variable
		if(ValueExist(pStr))
		{
			ValueGetStr(pStr, cmdStr, MAXCHARBUFF);
			pStr = cmdStr;
		}

		LogPrintf(0, L"%s ", pStr);
	}
}

//execute a variable
void CMD_VStr()
{
	if(CmdArgC() != 1) 
	{
		LogMsg(0, 0, L"vstr <variable> : execute a variable command.");
		return;
	}

	tCHAR buff[512], line[MAXCHARBUFF], cmd[MAXCHARBUFF];
	tCHAR *strPtr, *strPtrSub;

	if(ValueExist(CmdArgV(0)))
		ValueGetStr(CmdArgV(0), buff, 512);
	else
		wcscpy(buff, CmdArgV(0));

	strPtr = buff;

	//go through the buffer and get all the commands
	while(*strPtr != 0)
	{
		ParserReadStringBuff(&strPtr, line, MAXCHARBUFF, 0, ';');
		
		strPtrSub=(tCHAR*)line;

		//get the name of command
		ParserReadWordBuff(&strPtrSub, cmd, MAXCHARBUFF, ' ');

		//call command
		CmdCall(cmd, strPtrSub);
	}
}

/***********************************************************************/

/////////////////////////////////////
// Name:	CmdInit
// Purpose:	initialize all common
//			commands
// Output:	common commands added
// Return:	none
/////////////////////////////////////
void CmdInit()
{
	CmdAdd(L"cmdlist", CMD_List);
	CmdAdd(L"wait", CMD_Wait);
	CmdAdd(L"exec", CMD_Exec);
	CmdAdd(L"echo", CMD_Echo);
	CmdAdd(L"vstr", CMD_VStr);
}

////////////////////////////////////////////////////////////////
// Used by callbacks
//

/////////////////////////////////////
// Name:	CmdArgC
// Purpose:	get the current number of
//			arguments
// Output:	none
// Return:	number of arguments
/////////////////////////////////////
u32 CmdArgC()
{
	return g_argc;
}

/////////////////////////////////////
// Name:	CmdArgV
// Purpose:	get an argument string
// Output:	none
// Return:	argument string
/////////////////////////////////////
tCHAR *CmdArgV(u32 i)
{
	return (tCHAR*)g_argv[i];
}