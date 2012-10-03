#ifndef _cfg_i_h
#define _cfg_i_h

#include "common.h"

#include "STD\std_types.h"

#include "STD\memTool.h"
#include "STD\pager.h"
#include "STD\log.h"
#include "STD\filesys.h"

#include "STD\parser.h"

#include "STD\cfg.h"

typedef map <wstring, wstring> CfgItemList;

typedef map <wstring, CfgItemList > CfgSectionList;

typedef struct _ShaderVar {
	wstring var;
	wstring param;
} ShaderVar;

typedef struct _ShaderSec {
	wstring			name;

	vector<ShaderVar> vars;
} ShaderSec;

typedef struct _ShaderF {
	vector<ShaderSec> *sections;
} ShaderF;

#endif