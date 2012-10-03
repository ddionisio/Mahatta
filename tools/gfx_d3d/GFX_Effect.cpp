#include "gfx_i.h"

#include "GFX\gfx_math.h"

#include "gfx_effect.h"
#include "gfx_effect_scene.h"

map<wstring, hIEFFECT>  IgfxEffect::s_FXLIST;

HRESULT FXIncludeManager::Open(D3DXINCLUDE_TYPE IncludeType, LPCSTR pName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
{
	hFILE fp;

	tCHAR path[MAXCHARBUFF];
	mbstowcs(path, pName, MAXCHARBUFF);

	if(wcscmp(m_absfilepath.c_str(), path) != 0)//m_dir[0] != 0)
	{
		tCHAR filepath[MAXCHARBUFF];

		wcscpy(filepath, m_dir.c_str());
		wcscat(filepath, L"\\");
		wcscat(filepath, path);

		fp = FSOpen(filepath, L"rb");
	}
	else
	{
		fp = FSOpen(m_filepath.c_str(), L"rb");
	}

	if(fp)
	{
		u8 *dat;
		s64 len;
		FSSeek(fp, 0, FS_SEEK_END);
		FSGetPos(fp, &len);
		FSSeek(fp, 0, FS_SEEK_SET);
		dat = (u8*)MemAlloc((u32)len * sizeof(u32));

		if( dat == NULL )
		{ FSClose(fp); return E_OUTOFMEMORY; }

		FSRead(dat, (u32)len, 1, fp);

		*ppData = dat;
		*pBytes = (u32)len;

		FSClose(fp);
	}
	else
		return E_FAIL;

	return S_OK;
}

HRESULT FXIncludeManager::Close(LPCVOID pData)
{
	if(pData) MemFree((void*)pData);

	return S_OK;
}

//buffer error dispaly
void _FXCompilerErrorDump(const tCHAR *name, LPD3DXBUFFER error)
{
	tCHAR *str = (tCHAR*)MemAlloc(sizeof(tCHAR)*error->GetBufferSize());
	
	if(str)
	{
		mbstowcs(str, (const char *)error->GetBufferPointer(), error->GetBufferSize());

		LogMsg(LOG_FILE, name, L"Compile Errors\n");
		LogMsg(LOG_FILE, 0, str);

		MemFree(str);
	}
}

void _FXGetDir(const tCHAR *filepath, wstring & strOut, u8 bRel)
{
	tCHAR buff[MAXCHARBUFF]={0}; 
	
	if(filepath)
	{
		tCHAR path[MAXCHARBUFF];
		FSGetPath(filepath, path, MAXCHARBUFF);

		if(!bRel)
			wcscpy(buff, path);
		else
			wcscpy(buff, filepath);

		tCHAR* LastSlash = (tCHAR *)wcsrchr(buff, '\\');

		tCHAR* LastSlashR = (tCHAR *)wcsrchr(buff, '/');

		if(LastSlash && LastSlashR && wcslen(LastSlash) < wcslen(LastSlashR))
			(*(LastSlash)) = 0;
		else if(LastSlashR)
			(*(LastSlashR)) = 0;
		else if(LastSlash)
			(*(LastSlash)) = 0;
		else
			buff[0] = 0;
	}

	strOut = buff;
}

IgfxEffect::IgfxEffect(const tCHAR *name, const s8 *buff)
: m_ref(0), m_pScn(0), m_buff(buff), m_flag(0),
m_hMatDiffuse(0), m_hMatAmbient(0), m_hMatSpecular(0), m_hMatEmissive(0), m_hMatPower(0),
m_hTime(0), m_hTimeElapse(0), m_hVPPixelSize(0), m_lodDist(0), m_dfltTech(0), m_fxCB(0)
{
	LPD3DXBUFFER errors=0;

	FXIncludeManager inc;

	HRESULT hr;

	hr = D3DXCreateEffect(g_p3DDevice, buff, (u32)strlen((const char *)buff), NULL, &inc, 0, NULL, &m_effect, &errors);

	if(errors)
		_FXCompilerErrorDump(name, errors);

	if(GFXCheckError(hr, name))
	{ LogMsg(LOG_FILE, L"EffectCreate", L"Error creating effect"); SAFE_RELEASE(errors); }

	_InitSceneStuffBuff();

	//if we truly are a scene, then make sure all techniques work!
	//if not, then this FX is not any good
	if(TESTFLAGS(m_flag, EFFECT_SCENE)
		&& !_ValidateAllTechnique())
	{ LogMsg(LOG_FILE, L"EffectCreate", name); SAFE_RELEASE(errors); }
	
	_CreateCommon(name);
	_SceneParseBuff(buff);

	SAFE_RELEASE(errors);
}

IgfxEffect::IgfxEffect(const tCHAR *filename)
: m_ref(0), m_pScn(0), m_flag(EFFECT_FILE), m_buff(0),
m_hMatDiffuse(0), m_hMatAmbient(0), m_hMatSpecular(0), m_hMatEmissive(0), m_hMatPower(0),
m_hTime(0), m_hTimeElapse(0), m_hVPPixelSize(0), m_lodDist(0), m_dfltTech(0), m_fxCB(0)
{
	HRESULT hr;

	LPD3DXBUFFER errors=0;

	FXIncludeManager inc(filename);

	//hr = D3DXCreateEffectFromFile(g_p3DDevice, filepath, NULL, 0, D3DXSHADER_SKIPVALIDATION, NULL, &m_effect, &errors);
	//determine if we want to create
	hFILE fp = FSOpen(filename, L"rt");

	if(fp)
	{
		_InitSceneStuffFile(fp);

		if(FSIsArchive(fp))
		{
			hr = D3DXCreateEffect(g_p3DDevice, FSBufferGetPtr(fp), FSBufferGetSize(fp), NULL, &inc, 0, NULL, &m_effect, &errors);

			FSClose(fp);
		}
		else
		{
			FSClose(fp);

			tCHAR filepath[MAXCHARBUFF];
			FSGetPath(filename, filepath, MAXCHARBUFF);

			hr = D3DXCreateEffectFromFile(g_p3DDevice, filepath, NULL, &inc, 0, NULL, &m_effect, &errors);
		}
	}
	else
	{ LogMsg(LOG_FILE, L"EffectLoad: file does not exist", filename); }

	if(errors)
		_FXCompilerErrorDump(filename, errors);
	
	if(GFXCheckError(hr, filename))
	{ LogMsg(LOG_FILE, L"EffectLoad", L"Error creating effect"); SAFE_RELEASE(errors); }

	//if we truly are a scene, then make sure all techniques work!
	//if not, then this FX is not any good
	if(TESTFLAGS(m_flag, EFFECT_SCENE)
		&& !_ValidateAllTechnique())
	{ LogMsg(LOG_FILE, L"EffectLoad", filename); SAFE_RELEASE(errors); }

	_CreateCommon(filename);
	_SceneParseFile(filename);

	SAFE_RELEASE(errors);
}

IgfxEffect::~IgfxEffect()
{
	//remove from effect map
	s_FXLIST.erase(m_name);

	u32 i;

	//destroy scene stuff
	if(m_pScn)
	{
		//destroy all render textures
		for(i = 0; i < m_pScn->depthSurfs.size(); i++)
		{
			SAFE_RELEASE(m_pScn->depthSurfs[i].surf);
		}

		//destroy all renderers
		for(i = 0; i < m_pScn->renderTxts.size(); i++)
		{
			SAFE_RELEASE(m_pScn->renderTxts[i].surf);
			SAFE_RELEASE(m_pScn->renderTxts[i].txt);
		}

		//destroy all commands of this target
		for(i = 0; i < m_pScn->commands.size(); i++)
		{
			SAFE_DELETE(m_pScn->commands[i]);
		}

		delete m_pScn;
	}

	SAFE_RELEASE(m_effect); //destroy frame

	//destroy animated textures
	for(i = 0; i < m_pAnimTxts.size(); i++)
		SAFE_RELEASE(m_pAnimTxts[i].txt);
}

void IgfxEffect::AddRef()
{
	m_ref++;
}

s32 IgfxEffect::Release()
{
	if(m_ref == 0)
		delete this;
	else
	{ m_ref--; return m_ref; }

	return 0;
}

void IgfxEffect::_SetCompatTechnique()
{
	//HRESULT hr;

	//hr = m_effect->FindNextValidTechnique(NULL, &m_dfltTech);
	//_EffectCheckCompatibility(hEFFECT fx, GFXHANDLE tech)

	D3DXEFFECT_DESC desc;
	m_effect->GetDesc(&desc);
	GFXHANDLE hTech=0;

	for(u32 i = 0; i < desc.Techniques; i++)
	{
		hTech = m_effect->GetTechnique(i);
		if(_CheckCompatibility(hTech))
		{
			m_dfltTech = hTech;
			m_effect->SetTechnique(m_dfltTech);
			break;
		}
	}
}

//for scene FXs to fully function, we must make sure all
//techniques are valid.
u8 IgfxEffect::_ValidateAllTechnique()
{
	D3DXEFFECT_DESC desc;
	m_effect->GetDesc(&desc);
	D3DXHANDLE hTechnique;
	u8 ret = TRUE;

	for(u32 i = 0; i < desc.Techniques; i++)
	{
		hTechnique = m_effect->GetTechnique(i);

		//this technique working?
		if(hTechnique
			&& FAILED(m_effect->ValidateTechnique(hTechnique)))
		{
			D3DXTECHNIQUE_DESC techDesc;
			m_effect->GetTechniqueDesc(hTechnique, &techDesc);
			tCHAR name[MAXCHARBUFF]; mbstowcs(name, techDesc.Name, MAXCHARBUFF);
			LogMsg(LOG_FILE, L"Effect: Invalid Technique", name);
			ret = FALSE;
		}
	}

	return ret;
}

//basically just create pScn if scene command entry is found
void IgfxEffect::_InitSceneStuffFile(hFILE fp)
{
	u8 bIsScene = FALSE;

	if(fp)
	{
		tCHAR buff[MAXCHARBUFF]={0};

		s64 fsPos=0;
		FSGetPos(fp, &fsPos);

		while(!FSEOF(fp))
		{
			if(ParserReadStringFile(fp, buff, MAXCHARBUFF, '<', '>') == RETCODE_ENDREACHED)
			{
				if(wcscmp(L"scene_commands", buff) == 0)
				{ bIsScene = TRUE; break; }
			}
		}

		FSSetPos(fp, &fsPos);
	}

	if(bIsScene)
	{
		SETFLAG(m_flag, EFFECT_SCENE);

		m_pScn = new gfxEffectScn;
	}
}

void IgfxEffect::_InitSceneStuffBuff()
{
	u8 bIsScene = FALSE;

	s8 buff[MAXCHARBUFF]={0};
	u32 count=0;
	s8 *pBuff = (s8*)m_buff;

	while(*pBuff != 0)
	{
		if(*pBuff == '<')
		{
			if(*pBuff == '>')
			{
				if(strcmp("scene_commands", (LPCSTR)buff) == 0)
				{ bIsScene = TRUE; break; }

				memset(buff, 0, sizeof(buff));
				count=0;
			}
			else
			{
				buff[count]=*pBuff;
				count++;
			}
		}
		
		pBuff++;
	}

	if(bIsScene)
	{
		SETFLAG(m_flag, EFFECT_SCENE);

		m_pScn = new gfxEffectScn;
	}
}

void IgfxEffect::_CreateCommon(const tCHAR *name)
{
	memset(&m_lDir, 0, sizeof(m_lDir));
	memset(&m_lPoint, 0, sizeof(m_lPoint));
	memset(m_hBaseTxt, 0, sizeof(m_hBaseTxt));
	memset(m_mtx, 0, sizeof(m_mtx));

	//add it to our effect table
	//better not be there!!!
	assert(s_FXLIST.find(name) == s_FXLIST.end());

	s_FXLIST[name] = this;

	//find it again and get the name pointer
	map<wstring, hIEFFECT>::iterator it = s_FXLIST.find(name); //assert(it == s_FXLIST.end());

	m_name = (*it).first.c_str();

	//parse parameters
	_ParseParam();

	//set compatible technique
	_SetCompatTechnique();
}

/////////////////////////////////////
// Name:	EffectCreate
// Purpose:	create a new effect with
//			given NULL terminated
//			buffer
// Output:	new effect created
// Return:	new effect
/////////////////////////////////////
hEFFECT F_API EffectCreate(const tCHAR *name, const s8 *buff)
{
	hEFFECT newEffect = EffectSearchName(name);

	if(newEffect)
	{ newEffect->AddRef(); return newEffect; }
	
	newEffect = (hEFFECT)new IgfxEffect(name, buff);

	if(!newEffect)
	{ LogMsg(LOG_FILE, L"EffectCreate Cannot Allocate", name); return 0; }

	return newEffect;
}

/////////////////////////////////////
// Name:	EffectLoad
// Purpose:	create a new effect from
//			file
// Output:	new effect created
// Return:	new effect
/////////////////////////////////////
hEFFECT F_API EffectLoad(const tCHAR *filename)
{
	hEFFECT newEffect = EffectSearchName(filename);

	if(newEffect)
	{ newEffect->AddRef(); return newEffect; }
	
	newEffect = (hEFFECT)new IgfxEffect(filename);

	if(!newEffect)
	{ LogMsg(LOG_FILE, L"EffectLoad Cannot Allocate", filename); return 0; }

	return newEffect;
}

hIEFFECT IgfxEffect::sFind(const tCHAR *name)
{
	map<wstring, hIEFFECT>::iterator it = s_FXLIST.find(name);

	if(it != s_FXLIST.end())
		return (*it).second;

	return 0;
}

/////////////////////////////////////
// Name:	EffectSearchName
// Purpose:	find effect based on name
//			(filepath for loaded fxs)
// Output:	none
// Return:	the effect if found
/////////////////////////////////////
hEFFECT F_API EffectSearchName(const tCHAR *name)
{
	return (hEFFECT)IgfxEffect::sFind(name);
}

/////////////////////////////////////
// Purpose:	destroy all effect
// Output:	effects destroyed
// Return:	none
/////////////////////////////////////
void IgfxEffect::sDestroyAll()
{
	map<wstring, hIEFFECT>::iterator iNext;
	for(map<wstring, hIEFFECT>::iterator i = s_FXLIST.begin(); i != s_FXLIST.end(); i = iNext)
	{
		iNext = i; iNext++;
		if((*i).second) delete (*i).second;
	}

	s_FXLIST.clear();
}

/////////////////////////////////////
// Purpose:	invalidates effects
// Output:	effects invalidated
// Return:	none
/////////////////////////////////////
void IgfxEffect::sInvalidateAll()
{
	hIEFFECT fx;

	for(map<wstring, hIEFFECT>::iterator i = s_FXLIST.begin(); i != s_FXLIST.end(); ++i)
	{
		fx = (*i).second;

		fx->m_effect->OnLostDevice();

		if(fx->m_pScn)
		{
			u32 j;

			for(j = 0; j < fx->m_pScn->depthSurfs.size(); j++)
			{
				SAFE_RELEASE(fx->m_pScn->depthSurfs[j].surf);
			}

			for(j = 0; j < fx->m_pScn->renderTxts.size(); j++)
			{
				SAFE_RELEASE(fx->m_pScn->renderTxts[j].surf);
				SAFE_RELEASE(fx->m_pScn->renderTxts[j].txt);
			}
		}
	}
}

/////////////////////////////////////
// Purpose:	restore effects
// Output:	effects restored
// Return:	none
/////////////////////////////////////
void IgfxEffect::sRestoreAll()
{
	hIEFFECT fx;

	for(map<wstring, hIEFFECT>::iterator i = s_FXLIST.begin(); i != s_FXLIST.end(); ++i)
	{
		fx = (*i).second;

		fx->m_effect->OnResetDevice();

		if(fx->m_pScn)
		{
			u32 j;

			//recreate all the depth surfaces
			for(j = 0; j < fx->m_pScn->depthSurfs.size(); j++)
			{
				D3DSURFACE_DESC	   &ddsd=fx->m_pScn->depthSurfs[j].desc;
				GFXSURFACE		   *surf;

				if(fx->m_pScn->depthSurfs[j].vpSize.x != 0
					&& fx->m_pScn->depthSurfs[j].vpSize.y != 0)
				{
					ddsd.Width = (s32)((f32)g_mode.width*fx->m_pScn->depthSurfs[j].vpSize.x);
					ddsd.Height = (s32)((f32)g_mode.height*fx->m_pScn->depthSurfs[j].vpSize.y);
				}

				if(FAILED(g_p3DDevice->CreateDepthStencilSurface(ddsd.Width, ddsd.Height, ddsd.Format, ddsd.MultiSampleType, 
					ddsd.MultiSampleQuality, FALSE, &surf, 0)))
					continue;

				fx->m_pScn->depthSurfs[j].surf = surf;
			}

			//recreate all the render targets
			for(j = 0; j < fx->m_pScn->renderTxts.size(); j++)
			{
				s32					mipLevel=fx->m_pScn->renderTxts[j].mipLevel;
				D3DSURFACE_DESC	   &ddsd=fx->m_pScn->renderTxts[j].desc;
				PGFXTEXTURE			txt;
				GFXSURFACE		   *surf;

				if(fx->m_pScn->renderTxts[j].vpSize.x != 0
					&& fx->m_pScn->renderTxts[j].vpSize.y != 0)
				{
					ddsd.Width = (s32)((f32)g_mode.width*fx->m_pScn->renderTxts[j].vpSize.x);
					ddsd.Height = (s32)((f32)g_mode.height*fx->m_pScn->renderTxts[j].vpSize.y);
				}

				if(FAILED(D3DXCreateTexture(g_p3DDevice, ddsd.Width, ddsd.Height, mipLevel, ddsd.Usage, ddsd.Format, ddsd.Pool, &txt)))
					continue;
				
				txt->GetSurfaceLevel(0, &surf);

				fx->m_pScn->renderTxts[j].txt = txt;
				fx->m_pScn->renderTxts[j].surf = surf;
			}
		}
	}
}

/////////////////////////////////////
// Purpose:	get effect's parameter by
//			name. parent = 0 for
//			top-level access.
// Output:	none
// Return:	the parameter
/////////////////////////////////////
GFXHANDLE IgfxEffect::GetParam(GFXHANDLE parent, const tCHAR *name)
{
	char theName[MAXCHARBUFF]; wcstombs(theName, name, MAXCHARBUFF);
	return m_effect->GetParameterByName(parent, theName);
}

/////////////////////////////////////
// Purpose:	get value from fx with
//			given param (this can be
//			a string)
// Output:	dat is set
// Return:	TRUE if variable is found
//			and dat is set
/////////////////////////////////////
u8 IgfxEffect::GetValue(GFXHANDLE param, void *dat, u32 numBytes)
{
	return SUCCEEDED(m_effect->GetValue(param, dat, numBytes));
}

/////////////////////////////////////
// Purpose:	set value from given fx's
//			param (can be a string)
// Output:	value in fx set
// Return:	TRUE if variable is found
//			and is set
/////////////////////////////////////
u8 IgfxEffect::SetValue(GFXHANDLE param, void *dat, u32 numBytes)
{
	return SUCCEEDED(m_effect->SetValue(param, dat, numBytes));
}

/////////////////////////////////////
// Purpose:	set texture from given fx's
//			param (can be a string)
//			Don't use EffectSetValue
//			for textures, use this.
// Output:	value in fx set
// Return:	TRUE if variable is found
//			and is set
/////////////////////////////////////
u8 IgfxEffect::SetTexture(GFXHANDLE param, hTXT txt)
{
	HRESULT hr;

	LPDIRECT3DBASETEXTURE9 pTex = ((hITXT)txt)->GetImg();

	//pTex->AddRef();
	hr = m_effect->SetTexture(param, pTex);

	/*if(SUCCEEDED(hr))
	{ SAFE_RELEASE(pTex); return TRUE; }*/
	
	return SUCCEEDED(hr);
}

/////////////////////////////////////
// Purpose:	get the effect's technique
//			by name
// Output:	none
// Return:	the technique, 0 if not found
/////////////////////////////////////
GFXHANDLE IgfxEffect::GetTechnique(const tCHAR *name)
{
	char theName[MAXCHARBUFF]; wcstombs(theName, name, MAXCHARBUFF);
	return m_effect->GetTechniqueByName(theName);
}

/////////////////////////////////////
// Purpose:	get the next valid technique
//			set 0 for tech to get the first
//			valid tech.
// Output:	techOut set, if NULL, then
//			tech is the last technique
// Return:	TRUE if success
/////////////////////////////////////
u32 IgfxEffect::GetNextValidTechnique(GFXHANDLE tech, GFXHANDLE *techOut)
{
	return SUCCEEDED(m_effect->FindNextValidTechnique(tech, techOut));
}

/////////////////////////////////////
// Purpose:	checks to see if given
//			technique is valid
// Output:	none
// Return:	TRUE if valid
/////////////////////////////////////
u8 IgfxEffect::ValidateTechnique(GFXHANDLE tech)
{
	return SUCCEEDED(m_effect->ValidateTechnique(tech));
}

/////////////////////////////////////
// Purpose:	set the current technique
//			of fx
// Output:	technique set
// Return:	TRUE if success
/////////////////////////////////////
u8 IgfxEffect::SetTechnique(GFXHANDLE tech)
{
	return SUCCEEDED(m_effect->SetTechnique(tech));
}

/////////////////////////////////////
// Purpose:	set the effect's callback
//			for changing parameters
//			when rendering
// Output:	parameter callback set
// Return:	none
/////////////////////////////////////
void IgfxEffect::SetCallback(EFFECTFUNC fxCB)
{
	m_fxCB = fxCB;
}

//get the depth surface index
s32 IgfxEffect::GetDepthSurfInd(const tCHAR *name)
{
	if(m_pScn && name)
	{
		for(u32 i = 0; i < m_pScn->depthSurfs.size(); i++)
		{
			if(_wcsicmp(name, m_pScn->depthSurfs[i].name.c_str()) == 0)
				return i;
		}
	}

	return -1;
}

//get the render target texture index
s32 IgfxEffect::GetRenderTxtInd(const tCHAR *name)
{
	if(m_pScn && name)
	{
		for(u32 i = 0; i < m_pScn->renderTxts.size(); i++)
		{
			if(_wcsicmp(name, m_pScn->renderTxts[i].name.c_str()) == 0)
				return i;
		}
	}

	return -1;
}

//set the LOD tech based on given distance square
GFXHANDLE IgfxEffect::GetTechLOD(f32 distSq)
{
	if(m_lodDist > 0 && m_pLodTech.size() > 0)
	{
		f32 t = distSq/m_lodDist; if(t > 1) t = 1;
		u32 ind = (u32)(t*(f32)(m_pLodTech.size()-1));

		return m_pLodTech[ind];
	}

	return 0;
}

/////////////////////////////////////
// Purpose:	set texture from given fx's
//			base texture.
//			Maximum is MAXBASETXT
// Output:	value in fx set
// Return:	TRUE if variable is found
//			and is set
/////////////////////////////////////
u8 IgfxEffect::SetBaseTexture(u32 ind, hTXT txt)
{
	HRESULT hr=S_OK;

	LPDIRECT3DBASETEXTURE9 pTex = txt ? ((hITXT)txt)->GetImg() : 0;

	//pTex->AddRef();
	if(m_hBaseTxt[ind])
		hr = m_effect->SetTexture(m_hBaseTxt[ind], pTex);

	/*if(SUCCEEDED(hr))
	{ SAFE_RELEASE(pTex); return TRUE; }*/
	
	return SUCCEEDED(hr);
}