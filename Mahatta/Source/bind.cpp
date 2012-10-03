#include "engine_local.h"

#include "bind.h"

#include "command.h"

#include "value.h"

typedef struct _BindCmd {
	wstring			cmd;		//the command
	wstring			param;		//the command parameters
} BindCmd;

class BindKey {
public:
	BindKey(eInputType type, u8	key, const tCHAR *str) 
		: m_type(type), m_key(key), m_buff(str), m_bIsReleased(TRUE), m_startInd(0)
	{
		tCHAR line[MAXCHARBUFF], name[MAXCHARBUFF];
		tCHAR *strPtr = (tCHAR *)str, *strPtrSub;

		//go through the buffer and get all the commands
		while(*strPtr != 0)
		{
			BindCmd cmd;

			ParserReadStringBuff(&strPtr, line, MAXCHARBUFF, 0, ';');
			
			strPtrSub=(tCHAR*)line;

			//get the name of command
			ParserReadWordBuff(&strPtrSub, name, MAXCHARBUFF, ' ');
			cmd.cmd = name;

			//get the rest of the line
			cmd.param = strPtrSub;

			//insert command
			m_cmds.push_back(cmd);
		}
	}

	s32 Execute() //execute commands
	{
		tCHAR cmdChar;

		for(u32 i = m_startInd; i < m_cmds.size(); i++)
		{
			//check the first char of the command
			cmdChar = m_cmds[i].cmd.c_str()[0];

			//if '-', only through release we can call this command
			if(cmdChar == '-')
			{
				if(EngineInputIsReleased(m_type, m_key))
				{
					//set checking for neutral commands as released
					m_bIsReleased = TRUE;

					CmdCall(m_cmds[i].cmd.c_str(), m_cmds[i].param.c_str());
				}
			}
			//if '+', only through pressed we can call this command
			else if(cmdChar == '+')
			{
				if(EngineInputIsPressed(m_type, m_key))
				{
					//set checking for neutral commands as pressed
					m_bIsReleased = FALSE;

					CmdCall(m_cmds[i].cmd.c_str(), m_cmds[i].param.c_str());
				}
			}
			//neutral commands
			else if((m_bIsReleased && EngineInputIsReleased(m_type, m_key))
				||  (!m_bIsReleased && EngineInputIsPressed(m_type, m_key)))
			{
				CmdCall(m_cmds[i].cmd.c_str(), m_cmds[i].param.c_str());
				
				//let's break if we see a wait
				if(CmdIsPostWait())
				{ m_startInd = i; break; }
			}
		}

		//we've reached the end of commands
		if(m_startInd == m_cmds.size())
		{
			m_startInd = 0;
			m_bIsReleased = TRUE;
		}

		return TRUE;
	}

	eInputType GetType() { return m_type; }
	u8		   GetKey() { return m_key; }
	const tCHAR *GetBuffer() { return m_buff.c_str(); }

private:
	eInputType		m_type;		//which input device this bind belongs
	u8				m_key;		//the input key

	wstring			m_buff;		//the whole bind param

	vector<BindCmd> m_cmds;		//the commands to execute

	u8				m_bIsReleased;
	u32				m_startInd;
};

list<BindKey *> g_binds;		//all the bindings

static BindKey * _GetBind(eInputType type, u8 key)
{
	BindKey *BindPtr = 0;

	for(list<BindKey *>::iterator i = g_binds.begin(); i != g_binds.end(); i++)
	{
		if((*i)->GetType() == type && (*i)->GetKey() == key)
		{ BindPtr = *i; break; }
	}

	return BindPtr;
}

//get the bind key by given buff representing the bind param
static BindKey * _GetBindBuff(const tCHAR *buff)
{
	BindKey *BindPtr = 0;

	for(list<BindKey *>::iterator i = g_binds.begin(); i != g_binds.end(); i++)
	{
		if(wcscmp((*i)->GetBuffer(), buff) == 0)
		{ BindPtr = *i; break; }
	}

	return BindPtr;
}

/////////////////////////////////////
// Name:	BindGetBuffer
// Purpose:	get the binding buffer
//			of given key
// Output:	none
// Return:	the bind buffer
/////////////////////////////////////
const tCHAR *BindGetBuffer(eInputType device, u8 key)
{
	BindKey *bind = _GetBind(device, key);
	
	if(bind)
		return bind->GetBuffer();

	return 0;
}

/////////////////////////////////////
// Name:	BindUnSetAllKey
// Purpose:	remove all key bindings
// Output:	all key bind removed
// Return:	none
/////////////////////////////////////
void BindUnSetAllKey()
{
	for(list<BindKey *>::iterator i = g_binds.begin(); i != g_binds.end(); i++)
	{
		if(*i)
			delete (*i);
	}

	g_binds.clear();
}

/////////////////////////////////////
// Name:	BindUnSetKey
// Purpose:	remove bindings on given
//			key
// Output:	bind removed
// Return:	none
/////////////////////////////////////
void BindUnSetKey(eInputType device, u8 key)
{
	BindKey *bind = _GetBind(device, key);

	if(bind)
	{
		g_binds.remove(bind);
		delete bind;
	}
}

/////////////////////////////////////
// Name:	BindSetKey
// Purpose:	set the command for given
//			key
// Output:	bind added
// Return:	none
/////////////////////////////////////
void BindSetKey(eInputType device, u8 key, const tCHAR *str)
{
	//unset previous bind, if exist
	BindUnSetKey(device, key);

	BindKey *bind = new BindKey(device, key, str);

	if(bind)
		g_binds.push_back(bind);
}

/////////////////////////////////////
// Name:	BindGetKey
// Purpose:	get the binding key
//			of given buffer of
//			bind parameter
// Output:	input device and key
// Return:	TRUE if found and
//			key is set.
/////////////////////////////////////
u8 BindGetKey(const tCHAR *param, eInputType *deviceOut, u8 *keyOut)
{
	BindKey *bind = _GetBindBuff(param);
	
	if(bind)
	{
		if(deviceOut) *deviceOut = bind->GetType();
		if(keyOut) *keyOut = bind->GetKey();

		return TRUE;
	}

	return FALSE;
}

/////////////////////////////////////
// Name:	BindExecute
// Purpose:	Execute all key bindings
// Output:	stuff happens
// Return:	none
/////////////////////////////////////
void BindExecute()
{
	for(list<BindKey *>::iterator i = g_binds.begin(); i != g_binds.end(); i++)
		(*i)->Execute();
}

/***********************************************************************/

// Basic Commands

void CMD_UnBind()
{
	eInputType device;
	u8 key;

	if (CmdArgC() != 1)
	{
		LogMsg(0, 0, L"unbind <key> : remove commands from a key.");
		return;
	}

	//get the device and keycode from string
	if(!EngineInputGetKey(CmdArgV(0), &device, &key))
	{
		//invalid string
		LogPrintf(0, L"\"%s\" isn't a valid key", CmdArgV(0));
		return;
	}

	//remove binding
	BindUnSetKey(device, key);
}

void CMD_UnBindAll()
{
	BindUnSetAllKey();
}

void CMD_Bind()
{
	eInputType device;
	u8 key;

	if (CmdArgC() != 2)
	{
		LogMsg(0, 0, L"bind <key> [command] : attach a command to a key.");
		return;
	}

	//get the device and keycode from string
	if(!EngineInputGetKey(CmdArgV(0), &device, &key))
	{
		//invalid string
		LogPrintf(0, L"\"%s\" isn't a valid key", CmdArgV(0));
		return;
	}

	//set binding

	//check to see if argument 1 is a variable
	if(ValueExist(CmdArgV(1)))
	{
		tCHAR cmdStr[MAXCHARBUFF];
		ValueGetStr(CmdArgV(1), cmdStr, MAXCHARBUFF);

		BindSetKey(device, key, cmdStr);
	}
	else
		BindSetKey(device, key, CmdArgV(1));
}

void CMD_BindList()
{
	tCHAR keyStr[MAXCHARBUFF];

	for(list<BindKey *>::iterator i = g_binds.begin(); i != g_binds.end(); i++)
	{
		if(EngineInputGetStr((*i)->GetType(), (*i)->GetKey(), keyStr, MAXCHARBUFF))
			LogPrintf(0, L"%s \"%s\"", keyStr, (*i)->GetBuffer());
	}
}

/***********************************************************************/

/////////////////////////////////////
// Name:	BindCmdInit
// Purpose:	Initialize all bind
//			commands
// Output:	all bind commands set
// Return:	none
/////////////////////////////////////
void BindCmdInit()
{
	CmdAdd(L"unbind", CMD_UnBind);
	CmdAdd(L"unbindall", CMD_UnBindAll);
	CmdAdd(L"bind", CMD_Bind);
	CmdAdd(L"bindlist", CMD_BindList);
}

/////////////////////////////////////
// Name:	BindSave
// Purpose:	save all bindings to file
// Output:	binds added to file
// Return:	none
/////////////////////////////////////
void BindSave(hFILE fp)
{
	tCHAR keyStr[MAXCHARBUFF];

	for(list<BindKey *>::iterator i = g_binds.begin(); i != g_binds.end(); i++)
	{
		if(EngineInputGetStr((*i)->GetType(), (*i)->GetKey(), keyStr, MAXCHARBUFF))
			FSPrintf(fp, L"bind %s \"%s\"\n", keyStr, (*i)->GetBuffer());
	}
}