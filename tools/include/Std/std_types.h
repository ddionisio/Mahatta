#ifndef _std_types_h
#define _std_types_h

//
// Log types
//

#define LOG_FILE		0x00000001L		//log message to dbg_log.txt
#define LOG_ASSERT		0x00000002L		//pop-up assert message

//
// Pager ID types
//

#define     IDINVALIDATE(id) ((id).counter = 0)
#define		IDISVALID(id) ((id).counter > 0)
#define		IDISEQUAL(id1, id2) ((id1).ID == (id2).ID && (id1).counter == (id2).counter)

typedef struct _ID {
	u32 ID;			//unique identifier (key)
	u64 counter;	//unique counter (0 == invalid ID)

#ifdef __cplusplus
	u8 operator == ( const struct _ID & d ) const { return IDISEQUAL(*this, d); }
#endif

} ID;

//
// File System Types
//

#define FS_DIR_ARCHIVE		0x00000001L		//add a directory that is a pak file (.pak)
#define FS_DIR_READONLY		0x00000002L		//this directory is a read only, no creation/writing of file allowed.

typedef enum {
	FS_SEEK_SET,
	FS_SEEK_CUR,
	FS_SEEK_END,
} FS_SEEK;

typedef struct _FSMain	*hFMAIN;	//For including the directories and archives when opening files

typedef struct _FSFile	*hFILE;		//The file handle

//
// Parser Types
//

#define	RETCODE_ENDREACHED 2
#define RETCODE_NOTFOUND   3

typedef void (* FINDCALLBACK) (tCHAR *filepath, void *userData);

typedef void (* CMDLINECALLBACK) (const tCHAR *cmdline, void *userData);

//
// Config Types
//

typedef class Cfg {
public:
	virtual void AddRef() = 0;
	virtual s32 Release() = 0;

	/////////////////////////////////////
	// Purpose:	save config file
	// Output:	file saved from cfg path
	// Return:	success if success
	/////////////////////////////////////
	virtual RETCODE FileSave() = 0;

	/////////////////////////////////////
	// Purpose:	check if a section exists
	// Output:	none
	// Return:	TRUE if section exists
	/////////////////////////////////////
	virtual BOOL SectionExist(const tCHAR *section) = 0;

	/////////////////////////////////////
	// Purpose:	check if item exists
	//			within section
	// Output:	none
	// Return:	TRUE if item exists
	/////////////////////////////////////
	virtual BOOL ItemExist(const tCHAR *section, const tCHAR *item) = 0;

	/////////////////////////////////////
	// Purpose:	get item string
	// Output:	sOut is filled
	// Return:	TRUE if found and set given
	//			string
	/////////////////////////////////////
	virtual BOOL GetItemStr(const tCHAR *section, const tCHAR *item, tCHAR *sOut, u32 strSize) = 0;

	/////////////////////////////////////
	// Purpose:	get item s32
	// Output:	none
	// Return:	-1 if item does not exists...
	//			but hey, maybe you did want -1
	/////////////////////////////////////
	virtual s32 GetItemInt(const tCHAR *section, const tCHAR *item) = 0;

	/////////////////////////////////////
	// Purpose:	get item f32
	// Output:	none
	// Return:	-1 if item does not exists...
	//			but hey, maybe you did want -1
	/////////////////////////////////////
	virtual f32 GetItemFloat(const tCHAR *section, const tCHAR *item) = 0;

	/////////////////////////////////////
	// Purpose:	add or set item string
	//			will add item if given item
	//			name does not exists
	// Output:	modify or add item
	// Return:	success if success
	/////////////////////////////////////
	virtual RETCODE AddItemStr(const tCHAR *section, const tCHAR *item, const tCHAR *str) = 0;

	/////////////////////////////////////
	// Purpose:	add or set item s32
	//			will add item if given item
	//			name does not exists
	// Output:	modify or add item
	// Return:	success if success
	/////////////////////////////////////
	virtual RETCODE AddItemInt(const tCHAR *section, const tCHAR *item, s32 val) = 0;

	/////////////////////////////////////
	// Purpose:	add or set item f32
	//			will add item if given item
	//			name does not exists
	// Output:	modify or add item
	// Return:	success if success
	/////////////////////////////////////
	virtual RETCODE AddItemFloat(const tCHAR *section, const tCHAR *item, f32 val) = 0;
} *hCFG;

typedef class ShaderFile {
public:
	virtual void AddRef() = 0;
	virtual s32 Release() = 0;

	/////////////////////////////////////
	// Purpose:	get number of sections in
	//			the shader file
	// Output:	none
	// Return:	number of shader sections
	/////////////////////////////////////
	virtual u32 SectionGetNum() = 0;

	/////////////////////////////////////
	// Purpose:	get the name of the given
	//			section index
	// Output:	none
	// Return:	the name
	/////////////////////////////////////
	virtual const tCHAR *SectionGetName(u32 ind) = 0;

	/////////////////////////////////////
	// Purpose:	get number of variables (lines)
	//			in given section index
	// Output:	none
	// Return:	number of variables (lines)
	/////////////////////////////////////
	virtual u32 SectionGetVarNum(u32 ind) = 0;

	/////////////////////////////////////
	// Purpose:	get the name of the given
	//			variable index
	//			The left hand side
	// Output:	none
	// Return:	the name ptr.
	/////////////////////////////////////
	virtual const tCHAR *VarGetName(u32 sectionInd, u32 ind) = 0;

	/////////////////////////////////////
	// Purpose:	get the param of the given
	//			variable index
	//			The right hand side
	// Output:	none
	// Return:	the param ptr.
	/////////////////////////////////////
	virtual const tCHAR *VarGetParam(u32 sectionInd, u32 ind) = 0;
} *hSHADERF;

#endif