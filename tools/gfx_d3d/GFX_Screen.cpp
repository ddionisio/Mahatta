#include "gfx_i.h"

#define SCRN_TXT_W	256
#define SCRN_TXT_H	256

u32					g_surfW, g_surfH;

GFXSURFACE * g_surface = 0;		//the buffer to be filled by the screen

vector< vector< hTXT > > g_scrnTxts;	//the screen textures

/////////////////////////////////////
// Name:	GFXScreenDestroy
// Purpose:	destroy the buffer screen
//			and textures
// Output:	stuff destroyed
// Return:	none
/////////////////////////////////////
void GFXScreenDestroy()
{
	SAFE_RELEASE(g_surface);

	for(u32 i = 0; i < g_scrnTxts.size(); i++)
	{
		for(u32 j = 0; j < g_scrnTxts[i].size(); j++)
		{
			SAFE_RELEASE(g_scrnTxts[i][j]); 
			g_scrnTxts[i][j] = 0;
		}
	}
}

/////////////////////////////////////
// Name:	GFXScreenCreate
// Purpose:	create the surface screen
//			and the textures
// Output:	initializes the surface
//			and textures
// Return:	none
/////////////////////////////////////
void GFXScreenCreate(u32 w, u32 h)
{
	GFXScreenDestroy();

	D3DDISPLAYMODE dm;
	dm.Width = dm.Height = 0;

	g_p3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dm);

	g_surfW = w;
	g_surfH = h;

	tCHAR buff[MAXCHARBUFF]; swprintf(buff, L"w:%d h:%d", g_surfW, g_surfH);

	//create the surface
	if(GFXCheckError(g_p3DDevice->CreateOffscreenPlainSurface(g_surfW, g_surfH,
    D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &g_surface, 0), buff))//"GFX_ScreenCreate"))
	{ return; }

	//set the textures up
	tCHAR txtname[MAXCHARBUFF];

	g_scrnTxts.resize((g_surfH/SCRN_TXT_H)+1);

	for(u32 i = 0; i < g_scrnTxts.size(); i++)
	{
		g_scrnTxts[i].resize((g_surfW/SCRN_TXT_W)+1);

		for(u32 j = 0; j < g_scrnTxts[i].size(); j++)
		{
			swprintf(txtname, L"scrnTXT_%d_%d", i, j);

			g_scrnTxts[i][j] = TextureCreate(txtname,
								SCRN_TXT_W, SCRN_TXT_H, 3,
								GFXUSAGE_DYNAMIC, GFXFMT_A8R8G8B8, 0);

			if(!g_scrnTxts[i][j]) //that means something went wrong
			{ GFXScreenDestroy(); return; }
		}
	}
}

/////////////////////////////////////
// Name:	GFXScreenCapture
// Purpose:	take a snapshot of what
//			currently is on screen
// Output:	screen stuff filled
// Return:	success if success
/////////////////////////////////////
u32 F_API GFXScreenCapture()
{
	//copy the surface
	IDirect3DSwapChain9 *pSwapChain;
	HRESULT hr = g_p3DDevice->GetSwapChain(0, &pSwapChain);

	if(SUCCEEDED(hr) && pSwapChain)
	{
		D3DDISPLAYMODE ddisp;
		pSwapChain->GetDisplayMode(&ddisp);

		D3DSURFACE_DESC ddesc;
		g_surface->GetDesc(&ddesc);

		if(GFXCheckError(g_p3DDevice->GetFrontBufferData(0,g_surface), L"GFXScreenCapture"))
			return FALSE;

		//lock the surface
		D3DLOCKED_RECT surfRect;

		if(GFXCheckError(g_surface->LockRect(&surfRect, 0, D3DLOCK_READONLY), L"GFXScreenCapture"))
			return FALSE;

		//now fill up the textures
		for(u32 i = 0; i < g_scrnTxts.size(); i++)
		{
			for(u32 j = 0; j < g_scrnTxts[i].size(); j++)
			{
				assert(g_scrnTxts[i][j]); //this should not happen...

				s32 xStart = j*SCRN_TXT_W + g_CLIENTRECT.left;
				s32 yStart = i*SCRN_TXT_H + g_CLIENTRECT.top;

				s32 xMax = xStart + SCRN_TXT_W;
				s32 yMax = yStart + SCRN_TXT_H;

				if(xMax > g_surfW)
					xMax = g_surfW;

				if(yMax > g_surfH)
					yMax = g_surfH;

				s32 pitch;
				u8 *pTxtBits, *thisBit, *surfBit;

				if(g_scrnTxts[i][j]->Lock(0, 0, D3DLOCK_DISCARD, &pitch, (void **)&pTxtBits) != TRUE)
				{ LogMsg(LOG_FILE, L"GFXScreenCapture", L"Failed to lock a texture!"); return FALSE; }

				//now fill 'er up
				for(s32 y = yStart, yTxt = 0; y < yMax; y++, yTxt++)
				{
					surfBit = ((BYTE*)surfRect.pBits) + (y*surfRect.Pitch);
					surfBit += xStart*4;

					thisBit = pTxtBits + (yTxt*pitch);

					for(s32 x = xStart, xTxt = 0; x < xMax; xTxt++, x++)
					{
						thisBit[3] = 255;//surfBit[3];
						thisBit[2] = surfBit[2];
						thisBit[1] = surfBit[1];
						thisBit[0] = surfBit[0];

						thisBit+=4;
						surfBit+=4;
					}
				}

				g_scrnTxts[i][j]->Unlock(0);
			}
		}

		g_surface->UnlockRect();

		pSwapChain->Release();
	}

	return TRUE;
}

/////////////////////////////////////
// Name:	GFXScreenDisplay
// Purpose:	display the snapshot of
//			the screen
//			NOTE: call TextureBegin
//				  first!!!
// Output:	stuff displayed
// Return:	success if success
/////////////////////////////////////
u32 F_API GFXScreenDisplay(f32 xOfs, f32 yOfs, gfxBlt *pBlt)
{
	for(u32 i = 0; i < g_scrnTxts.size(); i++)
	{
		for(u32 j = 0; j < g_scrnTxts[i].size(); j++)
		{
			if(g_scrnTxts[i][j]->Blt((f32)(j*SCRN_TXT_W) + xOfs, (f32)(i*SCRN_TXT_H) + yOfs, 0, pBlt) != TRUE)
				return FALSE;
		}
	}

	return TRUE;
}