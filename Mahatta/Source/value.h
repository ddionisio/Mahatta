#ifndef _value_h
#define _value_h

//
// Value Interface
//

/////////////////////////////////////
// Name:	ValueSetFlags
// Purpose:	set existing value's
//			flags.
// Output:	value's flag set
// Return:	none
/////////////////////////////////////
void ValueSetFlags(const tCHAR *name, u32 flag);

/////////////////////////////////////
// Name:	ValueGetFlags
// Purpose:	get value's flags.
// Output:	none
// Return:	the flags
/////////////////////////////////////
u32 ValueGetFlags(const tCHAR *name);

/////////////////////////////////////
// Name:	ValueExist
// Purpose:	check to see if given
//			value exists
// Output:	none
// Return:	TRUE if so
/////////////////////////////////////
u8 ValueExist(const tCHAR *name);

/////////////////////////////////////
// Name:	ValueSetStr
// Purpose:	set given value's string
//			NOTE: if given name does
//			not exist, it will be created
// Output:	value is set
// Return:	TRUE if success
/////////////////////////////////////
s32 ValueSetStr(const tCHAR *name, const tCHAR *str);

/////////////////////////////////////
// Name:	ValueSetFloat
// Purpose:	set given value's float
//			NOTE: if given name does
//			not exist, it will be created
// Output:	value is set
// Return:	TRUE if success
/////////////////////////////////////
s32 ValueSetFloat(const tCHAR *name, f32 val);

/////////////////////////////////////
// Name:	ValueGetStr
// Purpose:	get given value's string
// Output:	strOut is set
// Return:	TRUE if success
/////////////////////////////////////
s32 ValueGetStr(const tCHAR *name, tCHAR *strOut, u32 size);

/////////////////////////////////////
// Name:	ValueGetFloat
// Purpose:	get given value's float
// Output:	fOut is set
// Return:	TRUE if success
/////////////////////////////////////
s32 ValueGetFloat(const tCHAR *name, f32 *fOut);

/////////////////////////////////////
// Name:	ValueCmdInit
// Purpose:	initialize value basic
//			commands
// Output:	more basic commands (like set)
// Return:	none
/////////////////////////////////////
void ValueCmdInit();

/////////////////////////////////////
// Name:	ValueSaveArchived
// Purpose:	save all values with
//			flag VALUE_FLAG_ARCHIVE
// Output:	values added to file
// Return:	none
/////////////////////////////////////
void ValueSaveArchived(hFILE fp);

#endif