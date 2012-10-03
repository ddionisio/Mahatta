#include "cfg_i.h"

class ICfg : public Cfg {
public:
ICfg(const tCHAR *filename) : m_ref(0), m_path(filename)
{
	hFILE fp = FSOpen(filename, L"rt");

	if(fp)
	{
		_LoadSectionsNItems(fp);

		FSClose(fp);
	}
}

~ICfg()
{
}

void AddRef()
{
	m_ref++;
}

s32 Release()
{
	if(m_ref == 0)
		delete this;
	else
	{ m_ref--; return m_ref; }

	return 0;
}

/////////////////////////////////////
// Purpose:	save config file
// Output:	file saved from cfg path
// Return:	success if success
/////////////////////////////////////
RETCODE FileSave()
{
	hFILE fp = FSOpen(m_path.c_str(), L"w+");//"wt");

	if(!fp) { return RETCODE_FAILURE; }

	for(CfgSectionList::iterator i =  m_sections.begin(); i != m_sections.end(); i++)
	{
		//write the section
		FSPrintf(fp, L"[%s]\n", (*i).first.c_str());

		for(CfgItemList::iterator j = (*i).second.begin(); j != (*i).second.end(); j++)
		{
			//write item and its value
			FSPrintf(fp, L"%s=%s\n", (*j).first.c_str(), (*j).second.c_str());
		}
	}

	FSClose(fp);

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Purpose:	check if a section exists
// Output:	none
// Return:	TRUE if section exists
/////////////////////////////////////
BOOL SectionExist(const tCHAR *section)
{
	CfgSectionList::iterator it = m_sections.find(section);

	return it != m_sections.end();
}

/////////////////////////////////////
// Purpose:	check if item exists
//			within section
// Output:	none
// Return:	TRUE if item exists
/////////////////////////////////////
BOOL ItemExist(const tCHAR *section, const tCHAR *item)
{
	if(SectionExist(section))
	{
		CfgItemList::iterator it = m_sections[section].find(item);

		return it != m_sections[section].end();
	}

	return FALSE;
}

/////////////////////////////////////
// Purpose:	get item string
// Output:	sOut is filled
// Return:	TRUE if found and set given
//			string
/////////////////////////////////////
BOOL GetItemStr(const tCHAR *section, const tCHAR *item, tCHAR *sOut, u32 strSize)
{
	if(ItemExist(section, item))
	{//strSize
		wstring & str = m_sections[section][item];
		
		if(strSize >= str.size())
		{
			memcpy(sOut, str.c_str(), sizeof(tCHAR)*str.size());
			sOut[str.size()] = 0;
		}
		else
		{
			memcpy(sOut, str.c_str(), sizeof(tCHAR)*strSize);
			sOut[strSize-1] = 0;
		}
		
		//wcscpy(sOut, (*cfg->sections)[section][item].c_str());

		return TRUE;
	}

	return FALSE;
}

/////////////////////////////////////
// Purpose:	get item s32
// Output:	none
// Return:	-1 if item does not exists...
//			but hey, maybe you did want -1
/////////////////////////////////////
s32 GetItemInt(const tCHAR *section, const tCHAR *item)
{
	if(ItemExist(section, item))
		return _wtoi(m_sections[section][item].c_str());

	return -1;
}

/////////////////////////////////////
// Purpose:	get item f32
// Output:	none
// Return:	-1 if item does not exists...
//			but hey, maybe you did want -1
/////////////////////////////////////
f32 GetItemFloat(const tCHAR *section, const tCHAR *item)
{
	if(ItemExist(section, item))
		return (f32)_wtof(m_sections[section][item].c_str());

	return -1;
}

/////////////////////////////////////
// Purpose:	add or set item string
//			will add item if given item
//			name does not exists
// Output:	modify or add item
// Return:	success if success
/////////////////////////////////////
RETCODE AddItemStr(const tCHAR *section, const tCHAR *item, const tCHAR *str)
{
	m_sections[section][item] = str;

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Purpose:	add or set item s32
//			will add item if given item
//			name does not exists
// Output:	modify or add item
// Return:	success if success
/////////////////////////////////////
RETCODE AddItemInt(const tCHAR *section, const tCHAR *item, s32 val)
{
	tCHAR buff[33];

	swprintf(buff, L"%d", val);

	return AddItemStr(section, item, buff);
}

/////////////////////////////////////
// Purpose:	add or set item f32
//			will add item if given item
//			name does not exists
// Output:	modify or add item
// Return:	success if success
/////////////////////////////////////
RETCODE AddItemFloat(const tCHAR *section, const tCHAR *item, f32 val)
{
	tCHAR buff[34];

	swprintf(buff, L"%f", val);

	return AddItemStr(section, item, buff);
}

private:
	BOOL _IsSection(tCHAR *buff)
	{
		if(buff[0] == '[' && buff[wcslen(buff)-1] == ']')
			return TRUE;

		return FALSE;
	}

	void _GetItem(const tCHAR *section, tCHAR *buff)
	{
		tCHAR *buffWalker = buff;

		tCHAR itmBuff[MAXCHARBUFF];

		//get item
		if(ParserReadWordBuff(&buffWalker, itmBuff, MAXCHARBUFF, '=') == '=')
		{
			m_sections[section][itmBuff] = buffWalker;
		}
	}

	RETCODE _LoadSectionsNItems(hFILE fp)
	{
		tCHAR *buff;
		s32   buffSize;

		RETCODE ret;

		wstring lastSec;

		while(!FSEOF(fp))
		{
			ret = ParserCountStringFile(fp, &buffSize, 0, '\n');

			if(buffSize <= 1) //something bad
			{
				ParserSkipCharFile(fp, '\n', FALSE);
				continue;
			}

			if(ret != RETCODE_FAILURE)
			{
				buffSize++;

				buff = (tCHAR*)MemAlloc(sizeof(tCHAR)*buffSize);

				ParserReadStringFile(fp, buff, buffSize, 0, '\n');
			}
			else
			{
				if(buffSize==0)
					continue;

				buff = (tCHAR*)MemAlloc(sizeof(tCHAR)*buffSize);

				ParserReadStringFile(fp, buff, buffSize, 0, '\n');
				buff[buffSize-1]=0;
			}

			//determine if it is a section, just check the two ends for '[' and ']'
			if(_IsSection(buff))
			{
				tCHAR *stuffBuff = buff;
				
				stuffBuff[buffSize-2] = 0; //remove the '[' and ']'
				stuffBuff++;

				lastSec = stuffBuff;
			}
			//it's an item, attempt to get the item if a section exists
			//if so, get the last
			else if(buff[0] != '#')
			{
				_GetItem(lastSec.c_str(), buff);
			}

			MemFree(buff);
		}

		return RETCODE_SUCCESS;
	}

	wstring			m_path; //file path
	CfgSectionList	m_sections;	//sections
	s32				m_ref;
};

/////////////////////////////////////
// Name:	CfgFileLoad
// Purpose:	load a config file
// Output:	config file loaded
// Return:	the new config file
/////////////////////////////////////
hCFG CfgFileLoad(const tCHAR *filename)
{
	hCFG newConfig = 0;

	newConfig = (hCFG)new ICfg(filename);

	if(!newConfig) { LogMsg(LOG_FILE, L"CfgFileLoad", L"Unable to allocate config interface"); }

	return newConfig;
}