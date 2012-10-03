#include "gfx_i.h"

list<hIFNT>		IgfxFont::s_fntList;			//list of all fonts created

void IgfxFont::sInvalidateAll()
{
	for(list<hIFNT>::iterator i = s_fntList.begin(); i != s_fntList.end(); i++)
	{
		if((*i) && (*i)->m_pD3DFont)
		{
			(*i)->m_pD3DFont->OnLostDevice();
		}
	}
}

void IgfxFont::sRestoreAll()
{
	for(list<hIFNT>::iterator i = s_fntList.begin(); i != s_fntList.end(); i++)
	{
		if((*i) && (*i)->m_pD3DFont)
		{
			(*i)->m_pD3DFont->OnResetDevice();
		}
	}
}

IgfxFont::IgfxFont(const gfxFontDesc *pDesc) : m_ref(0), m_pD3DFont(0)
{
	assert(pDesc);

	m_fntDesc = *pDesc;

	HRESULT hr = D3DXCreateFontIndirect(g_p3DDevice, (D3DXFONT_DESC *)pDesc,
		&m_pD3DFont);

	if(!GFXCheckError(hr, L"IgfxFont::IgfxFont"))
		s_fntList.push_back(this);
}

IgfxFont::~IgfxFont()
{
	s_fntList.remove(this);

	SAFE_RELEASE(m_pD3DFont);
}

void IgfxFont::AddRef()
{
	m_ref++;
}

s32 IgfxFont::Release()
{
	if(m_ref == 0)
		delete this;
	else
	{ m_ref--; return m_ref; }

	return 0;
}

/////////////////////////////////////
// Name:	FontCreateIndirect
// Purpose:	create font with given
//			description
// Output:	new font created
// Return:	the new font
/////////////////////////////////////
hFNT F_API FontCreateIndirect(const gfxFontDesc *pDesc)
{
	if(!pDesc) return 0; //serves you right

	hFNT newFnt = (hFNT)new IgfxFont(pDesc);

	if(!newFnt)
	{ LogMsg(LOG_FILE, L"FontCreateIndirect", L"Unable to allocate new font"); return 0; }

	return newFnt;
}

/////////////////////////////////////
// Name:	FontCreate
// Purpose:	create a font with given
//			type and format
// Output:	font created
// Return:	new font
/////////////////////////////////////
hFNT F_API FontCreate(const tCHAR *typeName, s32 size, u32 fmtFlag)
{
	gfxFontDesc fntDesc={0};
	
	HDC hDC = GetDC( NULL );
    s32 nLogPixelsY = GetDeviceCaps(hDC, LOGPIXELSY);
    ReleaseDC( NULL, hDC );

	fntDesc.Height = -size * nLogPixelsY / 72;
	fntDesc.Weight = TESTFLAGS(fmtFlag, GFXFONT_BOLD) ? FW_BOLD : FW_REGULAR;
	fntDesc.Italic = TESTFLAGS(fmtFlag, GFXFONT_ITALIC);
	fntDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
	fntDesc.Quality = TESTFLAGS(fmtFlag, GFXFONT_ANTIALIAS) ? ANTIALIASED_QUALITY : DEFAULT_QUALITY;
	fntDesc.PitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	
	wcscpy(fntDesc.FaceName, typeName);
	
	return FontCreateIndirect(&fntDesc);
}

/////////////////////////////////////
// Purpose:	resizes font
// Output:	this will re-create the
//			font, so don't use it
//			every update.
// Return:	TRUE if success
/////////////////////////////////////
u32 IgfxFont::Resize(s32 size)
{
	HDC hDC = GetDC( NULL );
    s32 nLogPixelsY = GetDeviceCaps(hDC, LOGPIXELSY);
    ReleaseDC( NULL, hDC );

	m_fntDesc.Height = -size * nLogPixelsY / 72;

	SAFE_RELEASE(m_pD3DFont);

	HRESULT hr = D3DXCreateFontIndirect(g_p3DDevice, (D3DXFONT_DESC *)&m_fntDesc, 
		&m_pD3DFont);

	if(FAILED(hr))
		return FALSE;

	return TRUE;
}

/////////////////////////////////////
// Purpose:	display font in a box
//			count = -1 for NULL 
//			terminated text
//			flags are: GFXDT_*
// Output:	font blt in a box
// Return:	success
/////////////////////////////////////
u32 IgfxFont::TextBox(const tCHAR *text, s32 count, const iRect *r, 
					  u32 flags, u32 color)
{
	return m_pD3DFont->DrawText(0, text, count, (LPRECT)r, flags, color);
}

/////////////////////////////////////
// Purpose:	blt string on screen with
//			given font
// Output:	string blted on screen
// Return:	TRUE if success
/////////////////////////////////////
u32 IgfxFont::Printf(f32 x, f32 y, u32 color, u32 flags, const tCHAR *text, ...)
{
	static tCHAR buff[1024];								// Holds Our String

	//do the printf thing
	va_list valist;
	va_start(valist, text);
	_vsnwprintf(buff, sizeof(buff), text, valist);
	va_end(valist);

	iRect r={(s32)x,(s32)y,0,0};

	return TextBox(buff, -1, &r, flags | GFXDT_NOCLIP, color);
}

/////////////////////////////////////
// Purpose:	get the size of string
//			based on given font
// Output:	pSize set
// Return:	none
/////////////////////////////////////
void IgfxFont::GetStrSize(const tCHAR *str, iPoint *pSize)
{
	RECT r={0};

	m_pD3DFont->DrawText(0, str, -1, &r, DT_CALCRECT, 0xffffffff);

	pSize->x = r.right;
	pSize->y = r.bottom;
}