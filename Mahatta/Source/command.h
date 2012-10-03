#ifndef _command_h
#define _command_h

//Commands are conventional ways to communicate across all modules
//Just be careful to give the command the correct param
//EX: spawn_entity name, set_player_head pig 10, execute_action 1

//This also includes Value Interface

/////////////////////////////////////
// Name:	CmdIsPostWait
// Purpose:	check to see if we should
//			wait.  This will also set
//			postWait back to FALSE
// Output:	g_postWait = FALSE
// Return:	did we want to wait?
/////////////////////////////////////
u8 CmdIsPostWait();

/////////////////////////////////////
// Name:	CmdAdd
// Purpose:	add a new command with
//			given name
// Output:	new command added
// Return:	TRUE if success
/////////////////////////////////////
s32 CmdAdd(const tCHAR *name, COMMANDPROC proc);

/////////////////////////////////////
// Name:	CmdExist
// Purpose:	check to see if the given
//			command exists
// Output:	none
// Return:	TRUE if so
/////////////////////////////////////
u8 CmdExist(const tCHAR *name);

/////////////////////////////////////
// Name:	CmdCall
// Purpose:	call a command with given
//			param (make sure the param
//			matches the command's req.
// Output:	command called
// Return:	return TRUE if command existed
//			and is called
/////////////////////////////////////
s32 CmdCall(const tCHAR *name, const tCHAR *param);

/////////////////////////////////////
// Name:	CmdCallStr
// Purpose:	call a command from whole
//			string. The first word is
//			the command
// Output:	command called
// Return:	return code from command
/////////////////////////////////////
s32 CmdCallStr(const tCHAR *str);

/////////////////////////////////////
// Name:	CmdPost
// Purpose:	post a command with given
//			param (make sure the param
//			matches the command's req.
// Output:	command called
// Return:	TRUE if success
/////////////////////////////////////
s32 CmdPost(const tCHAR *name, const tCHAR *param);

/////////////////////////////////////
// Name:	CmdPostStr
// Purpose:	post a command from whole
//			string. The first word is
//			the command
// Output:	command called
// Return:	TRUE if success
/////////////////////////////////////
s32 CmdPostStr(const tCHAR *str);

/////////////////////////////////////
// Name:	CmdExec
// Purpose:	execute all currently
//			posted commands
// Output:	execute and remove commands
// Return:	TRUE if success
/////////////////////////////////////
s32 CmdExec();

/////////////////////////////////////
// Name:	CmdExecFile
// Purpose:	execute commands from file
// Output:	stuff executed
// Return:	TRUE if success
/////////////////////////////////////
s32 CmdExecFile(const tCHAR *filename);

/////////////////////////////////////
// Name:	CmdClear
// Purpose:	clear out posted commands
// Output:	remove all commands
// Return:	none
/////////////////////////////////////
void CmdClear();

/////////////////////////////////////
// Name:	CmdInit
// Purpose:	initialize all common
//			commands
// Output:	common commands added
// Return:	none
/////////////////////////////////////
void CmdInit();

//
// Used by callbacks
//

/////////////////////////////////////
// Name:	CmdArgC
// Purpose:	get the current number of
//			arguments
// Output:	none
// Return:	number of arguments
/////////////////////////////////////
u32 CmdArgC();

/////////////////////////////////////
// Name:	CmdArgV
// Purpose:	get an argument string
// Output:	none
// Return:	argument string
/////////////////////////////////////
tCHAR *CmdArgV(u32 i);

#endif