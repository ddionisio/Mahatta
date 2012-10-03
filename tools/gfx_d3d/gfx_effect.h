#ifndef _gfx_effect_h
#define _gfx_effect_h

/////////////////////////////////////////////////
//This is internally used by: GFX_Effect*.cpp

void _FXGetDir(const tCHAR *filepath, wstring & strOut, u8 bRel = FALSE);

//buffer error dispaly
void _FXCompilerErrorDump(const tCHAR *name, LPD3DXBUFFER error);

class FXIncludeManager : public ID3DXInclude
{
protected:
	wstring m_dir;
	wstring m_filepath;
	wstring m_absfilepath;
public:
	FXIncludeManager()
	{
		m_dir = L"\0";
	}

	FXIncludeManager(const tCHAR *filepath)
	{
		m_filepath = filepath;

		tCHAR absFilePath[MAXCHARBUFF];
		FSGetPath(filepath, absFilePath, MAXCHARBUFF);
		m_absfilepath = absFilePath;

		_FXGetDir(filepath, m_dir, TRUE);
	}

	void SetDir(const wstring & str) { m_dir = str; }

    STDMETHOD(Open)(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes);
    STDMETHOD(Close)(LPCVOID pData);
};

#endif