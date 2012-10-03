#include "gfx_i.h"

map<wstring, hITXT>	  IgfxTexture::s_TXTLIST;			//list of textures and textures w/ animations
list<hITXT>			  IgfxTexture::s_TXTANIMLIST;		//list of textures w/ animation

u8 IgfxTexture::_AddToList(const tCHAR *name)
{
	//add it to our texture table
	if(s_TXTLIST.find(name) == s_TXTLIST.end())
	{
		s_TXTLIST[name] = this;

		//find it again and get the name pointer
		map<wstring, hITXT>::iterator it = s_TXTLIST.find(name); //assert(it == s_TXTLIST.end());

		m_name = (*it).first.c_str();

		return TRUE;
	}

	return FALSE;
}

IgfxTexture::IgfxTexture(const tCHAR *name, u32 width, u32 height, u32 mipLevels, u32 usage, 
						 GFXFORMAT fmt, TXTCSMCREATE func) 
: m_texture(0), m_txtAnim(0), m_curAnim(0), m_numAnim(0), m_animDC(0), m_animD(0),
  m_pSurface(0), m_renderer(0), m_ref(0), m_srcClrKey(0), m_width(0), m_height(0), m_depth(0),
  m_mipLevels(mipLevels), m_usage(usage), m_bpp(fmt), m_func(func), m_type(GFXRTYPE_TEXTURE), m_flag(0)
{
	if(InitImage(width, height))
		_AddToList(name);
	else
		LogMsg(LOG_FILE, name, L"Failed to create image");
}

IgfxTexture::IgfxTexture(const tCHAR *filename, u8 bClrKey, u32 srcClrKey)
: m_texture(0), m_txtAnim(0), m_curAnim(0), m_numAnim(0), m_animDC(0), m_animD(0),
  m_pSurface(0), m_renderer(0), m_ref(0), m_srcClrKey(0), m_width(0), m_height(0), m_depth(0),
  m_mipLevels(0), m_usage(0), m_bpp(GFXFMT_UNKNOWN), m_func(0), m_type(GFXRTYPE_TEXTURE), m_flag(0)
{
	if(bClrKey) SETFLAG(m_flag, TXT_CLRKEY);

	SETFLAG(m_flag, TXT_FILE);

	if(SUCCEEDED(Load(filename)))
	{
		_AddToList(filename);

		//add to the texture animation list
		//if this is an animation texture
		if(m_txtAnim)
			s_TXTANIMLIST.push_back(this);
	}
	else
		LogMsg(LOG_FILE, filename, L"Failed to load image");
}

IgfxTexture::~IgfxTexture()
{
	//just remove the pointer of texture from list
	s_TXTLIST.erase(m_name);

	SAFE_RELEASE(m_pSurface);
	SAFE_RELEASE(m_renderer);
	SAFE_RELEASE(m_texture); //destroy frame

	if(m_txtAnim)
	{
		//remove from texture animation list
		s_TXTANIMLIST.remove(this);

		for(u32 i = 0; i < m_numAnim; i++)
			SAFE_RELEASE(m_txtAnim[i]);

		MemFree(m_txtAnim);
	}
}

s32 IgfxTexture::Release()
{
	if(m_ref == 0)
	{
		//eat it!
		delete this;
	}
	else
	{ m_ref--; return m_ref; }

	return 0;
}

/////////////////////////////////////
// Purpose:	add reference to texture
//			on TextureDestroy, ref
//			is decreased instead.
//			Use this for efficient
//			texture sharing
// Output:	texture reference increase
// Return:	none
/////////////////////////////////////
void IgfxTexture::AddRef()
{
	m_ref++;
}

struct AnimInf {
	wstring name;
	PGFXBASETEXTURE txt;
};

HRESULT IgfxTexture::_LoadAnim(const tCHAR *filename)
{
	D3DXIMAGE_INFO imgInf;

	HRESULT hr=S_OK;

	hFILE fp = FSOpen(filename, L"rt");
	if(fp)
	{
		tCHAR buff[MAXCHARBUFF]={0};

		//get the delay
		ParserReadStringFile(fp, buff, MAXCHARBUFF, 0, '\n');
		swscanf(buff, L"delay=%f\n", &m_animD);

		//get the number of textures
		ParserReadStringFile(fp, buff, MAXCHARBUFF, 0, '\n');
		swscanf(buff, L"numTxt=%d\n", &m_numAnim);

		if(m_numAnim > 0)
		{
			//temporary string buffer
			AnimInf *txtInfs = new AnimInf[m_numAnim];

			//allocate the animation buffer
			m_txtAnim = (PGFXBASETEXTURE *)MemAlloc(sizeof(PGFXBASETEXTURE)*m_numAnim);

			if(txtInfs && m_txtAnim)
			{
				wstring txtPath;

				//get directory of the file
				tCHAR dir[MAXCHARBUFF]={0}; wcscpy(dir, filename);
				tCHAR *pStr = GetFilePart(dir); if(pStr) *pStr = 0;

				u32 numTxtN=0;	//number of unique paths

				//get each texture
				for(u32 i = 0; i < m_numAnim; i++)
				{
					ParserReadWordFile(fp, buff, MAXCHARBUFF, '=');
					ParserReadStringFile(fp, buff, MAXCHARBUFF, 0, '\n');

					if(buff[0] != 0)
					{
						//check for other existing texture names
						for(u32 j = 0; j < numTxtN; j++)
						{
							if(wcscmp(txtInfs[j].name.c_str(), buff) == 0)
							{
								m_txtAnim[i] = txtInfs[j].txt;

								if(m_txtAnim[i]) m_txtAnim[i]->AddRef();

								break;
							}
						}

						//load it
						if(!m_txtAnim[i])
						{
							//tCHAR imgPath[MAXCHARBUFF];

							txtPath = dir;
							txtPath += buff;

							m_txtAnim[i] = _TextureLoad(txtPath.c_str(), &imgInf, D3DX_DEFAULT, D3DX_DEFAULT, 
								D3DX_DEFAULT, D3DX_DEFAULT, D3DFMT_UNKNOWN, 
								TESTFLAGS(m_flag, TXT_CLRKEY) ? m_srcClrKey : 0);

								txtInfs[numTxtN].txt = m_txtAnim[i];
								txtInfs[numTxtN].name = buff;
								numTxtN++;
						}
					}
				}

				delete [] txtInfs;
			}
			else
				hr=E_FAIL;
		}
		else
			hr = E_FAIL;

		FSClose(fp);
	}
	else
		hr = E_FAIL;

	return hr;
}

//load up the texture's image
HRESULT IgfxTexture::Load(const tCHAR *filename)
{
	u8 bAnimTxt = FALSE;

	//determine if we have an animation texture...
	tCHAR *ext = GetExtension(filename);

	//we need the relative path
	if(ext && wcsicmp(ext, L".ini") == 0)
	{ bAnimTxt = TRUE; }

	HRESULT hr;

	if(bAnimTxt)
	{
		hr = _LoadAnim(filename);

		if(GFXCheckError(hr, filename))
		{ LogMsg(LOG_FILE, L"IgfxTexture::Load", L"Unable to load animated texture"); return E_FAIL; }

		m_type = GFXRTYPE_TEXTURE;
	}
	else
	{
		D3DXIMAGE_INFO imgInf;

		m_texture = _TextureLoad(filename, &imgInf, D3DX_DEFAULT, D3DX_DEFAULT, 
								D3DX_DEFAULT, D3DX_DEFAULT, D3DFMT_UNKNOWN, 
								TESTFLAGS(m_flag, TXT_CLRKEY) ? m_srcClrKey : 0);

		if(!m_texture)
		{ LogMsg(LOG_FILE, L"IgfxTexture::Load", L"Unable to load texture"); return E_FAIL; }

		m_type = (GFXRESOURCETYPE)imgInf.ResourceType;
		m_width = imgInf.Width;
		m_height = imgInf.Height;
	}

	return S_OK;
}

//initialize the texture image
u8 IgfxTexture::InitImage(u32 width, u32 height)
{
	PGFXTEXTURE img=0;

	GFXValidateTxtSize(&width, &height);

	D3DFORMAT d3dFormat = (D3DFORMAT)m_bpp;
	HRESULT hr;

	if(d3dFormat == GFXFMT_UNKNOWN)
		d3dFormat = (D3DFORMAT)g_mode.fmt;

	if(m_usage == 0)
	{
		hr = D3DXCreateTexture(g_p3DDevice, width, height,
			m_mipLevels, m_usage, d3dFormat, D3DPOOL_MANAGED,
			&img);
	}
	else
	{
		hr = D3DXCreateTexture(g_p3DDevice, width, height,
			m_mipLevels, m_usage, d3dFormat, D3DPOOL_DEFAULT,
			&img);
	}

	if(GFXCheckError(hr, L"IgfxTexture::InitImage"))
	{ LogMsg(LOG_FILE, L"IgfxTexture::InitImage", L"Unable to create texture"); return FALSE; }

	D3DSURFACE_DESC d3dsd;

	if(TESTFLAGS(m_usage, GFXUSAGE_RENDERTARGET))
	{
		img->GetSurfaceLevel(0, &m_pSurface);
		m_pSurface->GetDesc(&d3dsd);

		m_width = d3dsd.Width;
		m_height = d3dsd.Height;

		hr = D3DXCreateRenderToSurface(g_p3DDevice, m_width, m_height, 
			d3dsd.Format, TRUE, g_d3dParam.AutoDepthStencilFormat, &m_renderer);

		if(GFXCheckError(hr, L"IgfxTexture::InitImage"))
		{ LogMsg(LOG_FILE, L"IgfxTexture::InitImage", L"Unable to create texture renderer"); return FALSE; }
	}
	else
	{
		img->GetLevelDesc(0, &d3dsd);

		m_width = d3dsd.Width;
		m_height = d3dsd.Height;
	}

	m_texture = img;

	//get the custom texture function and call it
	if(m_func)
		m_func((hTXT)this);

	return TRUE;
}

/////////////////////////////////////
// Name:	TextureCreate
// Purpose:	create a texture with
//			given func, if not then
//			just a blank texture.
//			if fmt==GFXFMT_UNKNOWN, use gfx's
//			mode
// Output:	new texture added
// Return:	new texture
/////////////////////////////////////
hTXT F_API TextureCreate(const tCHAR *name, u32 width, u32 height, u32 mipLevels, u32 usage, 
						 GFXFORMAT fmt, TXTCSMCREATE func)
{
	//first make sure this texture does not exist
	hTXT newTxt = TextureSearchName(name);
	
	if(newTxt)
		newTxt->AddRef();
	else
	{
		newTxt = (hTXT)new IgfxTexture(name, width, height, mipLevels, usage, fmt, func);

		if(!newTxt)
		{ LogMsg(LOG_FILE, L"TextureCreate Cannot Allocate", name); return 0; }
	}

	return newTxt;
}

/////////////////////////////////////
// Name:	TextureLoad
// Purpose:	load a texture from file
// Output:	new texture added
// Return:	new texture
/////////////////////////////////////
hTXT F_API TextureLoad(const tCHAR *filename, u8 bClrKey, u32 srcClrKey)
{
	hTXT newTxt = TextureSearchName(filename);

	//we found an identical name, so use that
	if(newTxt)
		newTxt->AddRef();
	else
	{
		newTxt = (hTXT)new IgfxTexture(filename, bClrKey, srcClrKey);

		if(!newTxt)
		{ LogMsg(LOG_FILE, L"TextureLoad Cannot Allocate", filename); return 0; }
	}

	return newTxt;
}

hITXT IgfxTexture::sFind(const tCHAR *name)
{
	map<wstring, hITXT>::iterator it = s_TXTLIST.find(name);

	if(it != s_TXTLIST.end())
		return (*it).second;

	return 0;
}

/////////////////////////////////////
// Name:	TextureSearchName
// Purpose:	get texture by it's name
// Output:	none
// Return:	the texture if found
/////////////////////////////////////
hTXT F_API TextureSearchName(const tCHAR *name)
{
	return (hTXT)IgfxTexture::sFind(name);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
void IgfxTexture::sDestroyAll()
{
	map<wstring, hITXT>::iterator iNext;
	for(map<wstring, hITXT>::iterator i = s_TXTLIST.begin(); i != s_TXTLIST.end(); i = iNext)
	{
		iNext = i; iNext++;

		if((*i).second) delete (*i).second;
	}

	s_TXTLIST.clear();
}

/////////////////////////////////////
// Name:	TextureSet
// Purpose:	sets texture for texture
//			mapping.  Base is usually
//			0.
// Output:	texture is set
// Return:	none
/////////////////////////////////////
void F_API TextureSet(hTXT txt, u32 base)
{
	if(txt)
		g_p3DDevice->SetTexture(base, ((hITXT)txt)->GetImg());
	else
		g_p3DDevice->SetTexture(base, 0);
}

/////////////////////////////////////
// Purpose:	Make sure the given texture 
//			is created with 
//			GFXUSAGE_RENDERTARGET.
//			Instead of calling GFXBegin,
//			you call this instead.
// Output:	render target set and ready
// Return:	TRUE if success
/////////////////////////////////////
u32 IgfxTexture::BeginScene()
{
	if(m_pSurface && m_renderer)
	{
		GFXVIEWPORT vp;
		vp.X = vp.Y = 0;
		vp.MinZ = 0.0f; vp.MaxZ = 1.0f;
		vp.Width = m_width;
		vp.Height = m_height;

		if(TESTFLAGS(g_FLAGS, GFX_BEGINSCENESET))
			g_p3DDevice->EndScene(); //in case outside, the user already began gfx scene

		return SUCCEEDED(m_renderer->BeginScene(m_pSurface, &vp));
	}

	return FALSE;
}

/////////////////////////////////////
// Purpose:	after you are done rendering
//			to a texture (TextureBeginScene)
//			call this function
// Output:	texture rendering is OVER!
// Return:	none
/////////////////////////////////////
void IgfxTexture::EndScene()
{
	if(m_renderer)
	{
		m_renderer->EndScene(0);

		if(TESTFLAGS(g_FLAGS, GFX_BEGINSCENESET))
			g_p3DDevice->BeginScene(); //we are done, so back to that begin scene
	}
}

/////////////////////////////////////
// Purpose:	get width of texture
// Output:	none
// Return:	texture's width
/////////////////////////////////////
u32 IgfxTexture::GetWidth()
{
	return m_width;
}

/////////////////////////////////////
// Purpose:	get height of texture
// Output:	none
// Return:	texture's height
/////////////////////////////////////
u32 IgfxTexture::GetHeight()
{
	return m_height;
}

/////////////////////////////////////
// Purpose:	lock texture
// Output:	texture locked
// Return:	TRUE if success
/////////////////////////////////////
u32 IgfxTexture::Lock(u32 lvl, const iRect *pRect, u32 flags, s32 *pitchOut, void **bitsOut)
{
	if(!m_texture || m_type != GFXRTYPE_TEXTURE)
		return 0;

	PGFXTEXTURE img = (PGFXTEXTURE)m_texture;

	D3DLOCKED_RECT lRect;

	if(FAILED(img->LockRect(lvl, &lRect, (const RECT *)pRect, flags)))
		return FALSE;

	*pitchOut = lRect.Pitch;

	*bitsOut = lRect.pBits;

	return TRUE;
}

/////////////////////////////////////
// Purpose:	unlock texture
// Output:	texture unlocked
// Return:	none
/////////////////////////////////////
void IgfxTexture::Unlock(u32 lvl)
{
	if(m_texture && m_type == GFXRTYPE_TEXTURE)
	{
		((PGFXTEXTURE)m_texture)->UnlockRect(lvl);
	}
}

/////////////////////////////////////
// Purpose:	invalidate textures
// Output:	textures invalidated
// Return:	none
/////////////////////////////////////
void IgfxTexture::sInvalidateAll()
{
	hITXT txt;

	for(map<wstring, hITXT>::iterator i = s_TXTLIST.begin(); i != s_TXTLIST.end(); ++i)
	{
		txt = (*i).second;

		if(txt->m_usage != 0 || TESTFLAGS(txt->m_flag, TXT_FILE))
		{
			SAFE_RELEASE(txt->m_pSurface);
			SAFE_RELEASE(txt->m_renderer);
			SAFE_RELEASE(txt->m_texture);

			if(txt->m_txtAnim)
			{
				for(u32 j = 0; j < txt->m_numAnim; j++)
					SAFE_RELEASE(txt->m_txtAnim[j]);

				MemFree(txt->m_txtAnim); txt->m_txtAnim = 0;
			}
		}
		else
			txt->m_texture->PreLoad();
	}
}

/////////////////////////////////////
// Purpose:	restore textures
// Output:	textures restored
// Return:	none
/////////////////////////////////////
void IgfxTexture::sRestoreAll()
{
	hITXT txt;

	for(map<wstring, hITXT>::iterator i = s_TXTLIST.begin(); i != s_TXTLIST.end(); ++i)
	{
		txt = (*i).second;

		if(txt->m_usage != 0 || TESTFLAGS(txt->m_flag, TXT_FILE))
		{
			if(TESTFLAGS(txt->m_flag, TXT_FILE))
				txt->Load(txt->m_name);
			else
				txt->InitImage(txt->GetWidth(), txt->GetHeight());
		}
	}
}

/////////////////////////////////////
// Purpose:	this will update all 
//			textures with animation
// Output:	animation updated
// Return:	none
/////////////////////////////////////
void IgfxTexture::sAnimUpdateAll()
{
	for(list<hITXT>::iterator i = s_TXTANIMLIST.begin(); i != s_TXTANIMLIST.end(); i++)
	{
		(*i)->m_animDC += g_time;
		if((*i)->m_animDC >= (*i)->m_animD)
		{
			(*i)->m_animDC = 0;
			(*i)->m_curAnim++; if((*i)->m_curAnim == (*i)->m_numAnim) (*i)->m_curAnim = 0;
		}
	}
}

/////////////////////////////////////
// Purpose:	get the texture's image
// Output:	none
// Return:	the texture image
/////////////////////////////////////
PGFXBASETEXTURE IgfxTexture::GetImg()
{
	if(m_texture)
		return m_texture;

	return m_txtAnim[m_curAnim];
}