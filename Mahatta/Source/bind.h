#ifndef _bind_h
#define _bind_h

/////////////////////////////////////
// Name:	BindGetBuffer
// Purpose:	get the binding buffer
//			of given key
// Output:	none
// Return:	the bind buffer
/////////////////////////////////////
const tCHAR *BindGetBuffer(eInputType device, u8 key);

/////////////////////////////////////
// Name:	BindUnSetAllKey
// Purpose:	remove all key bindings
// Output:	all key bind removed
// Return:	none
/////////////////////////////////////
void BindUnSetAllKey();

/////////////////////////////////////
// Name:	BindUnSetKey
// Purpose:	remove bindings on given
//			key
// Output:	bind removed
// Return:	none
/////////////////////////////////////
void BindUnSetKey(eInputType device, u8 key);

/////////////////////////////////////
// Name:	BindSetKey
// Purpose:	set the command for given
//			key
// Output:	bind added
// Return:	none
/////////////////////////////////////
void BindSetKey(eInputType device, u8 key, const tCHAR *str);

/////////////////////////////////////
// Name:	BindGetKey
// Purpose:	get the binding key
//			of given buffer of
//			bind parameter
// Output:	input device and key
// Return:	TRUE if found and
//			key is set.
/////////////////////////////////////
u8 BindGetKey(const tCHAR *param, eInputType *deviceOut, u8 *keyOut);

/////////////////////////////////////
// Name:	BindExecute
// Purpose:	Execute all key bindings
// Output:	stuff happens
// Return:	none
/////////////////////////////////////
void BindExecute();

/////////////////////////////////////
// Name:	BindCmdInit
// Purpose:	Initialize all bind
//			commands
// Output:	all bind commands set
// Return:	none
/////////////////////////////////////
void BindCmdInit();

/////////////////////////////////////
// Name:	BindSave
// Purpose:	save all bindings to file
// Output:	binds added to file
// Return:	none
/////////////////////////////////////
void BindSave(hFILE fp);

#endif