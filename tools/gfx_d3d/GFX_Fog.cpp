#include "gfx_i.h"

//TODO: save fog info, so we can restore it on GFXReset
u8		g_fogEnable=FALSE;
u32		g_fogMode=GFXFOG_NONE;
u32		g_fogClr=0;
f32		g_fogStart,g_fogEnd,g_fogDensity;

/////////////////////////////////////
// Name:	FogEnable
// Purpose:	enable/disable the fog
// Output:	fog enabled/disabled
// Return:	none
/////////////////////////////////////
void F_API FogEnable(u8 bYes)
{
	g_fogEnable = bYes;
	g_p3DDevice->SetRenderState(D3DRS_FOGENABLE, (DWORD)bYes);
}

/////////////////////////////////////
// Name:	FogIsEnable
// Purpose:	check to see if fog is
//			enabled
// Output:	none
// Return:	true if enabled
/////////////////////////////////////
u8 F_API FogIsEnable()
{
	DWORD state;

	g_p3DDevice->GetRenderState(D3DRS_FOGENABLE, &state);

	return (u8)state;
}

/////////////////////////////////////
// Name:	
// Purpose:	set mode to:
//			 GFXFOG_NONE
//			 GFXFOG_EXP    (uses density)
//			 GFXFOG_EXP2   (uses density)
//			 GFXFOG_LINEAR (uses start/end)
//			note: vertex fog can only
//				  use linear
// Output:	mode set
// Return:	none
/////////////////////////////////////
void F_API FogSetMode(u32 mode)
{
	g_fogMode = mode;

	if((g_d3dcaps.RasterCaps & D3DPRASTERCAPS_FOGTABLE)
		&& ((g_d3dcaps.RasterCaps&D3DPRASTERCAPS_ZFOG) || (g_d3dcaps.RasterCaps&D3DPRASTERCAPS_WFOG)))
	{
		g_p3DDevice->SetRenderState( D3DRS_FOGVERTEXMODE,  GFXFOG_NONE );
        g_p3DDevice->SetRenderState( D3DRS_FOGTABLEMODE,   mode );

		g_p3DDevice->SetRenderState( D3DRS_RANGEFOGENABLE, FALSE );
	}
	else
	{
		if(g_fogMode != GFXFOG_LINEAR)
			g_fogMode = GFXFOG_NONE;

		g_p3DDevice->SetRenderState( D3DRS_FOGTABLEMODE,   GFXFOG_NONE );
        g_p3DDevice->SetRenderState( D3DRS_FOGVERTEXMODE,  g_fogMode );

		if(g_d3dcaps.RasterCaps & D3DPRASTERCAPS_FOGRANGE)
			g_p3DDevice->SetRenderState( D3DRS_RANGEFOGENABLE, TRUE );
	}
}

/////////////////////////////////////
// Name:	FogSetColor
// Purpose:	set color of fog
// Output:	color of fog is set
// Return:	none
/////////////////////////////////////
void F_API FogSetColor(u32 clr)
{
	g_fogClr = clr;

	g_p3DDevice->SetRenderState( D3DRS_FOGCOLOR,  clr );
}

/////////////////////////////////////
// Name:	FogSetStart
// Purpose:	set the start of fog,
//			used in linear mode
// Output:	fog start set
// Return:	none
/////////////////////////////////////
void F_API FogSetStart(f32 val)
{
	g_fogStart = val;

	g_p3DDevice->SetRenderState( D3DRS_FOGSTART, *(DWORD*)(&val));
}

/////////////////////////////////////
// Name:	FogSetEnd
// Purpose:	set the end of fog,
//			used in linear mode
// Output:	fog end set
// Return:	none
/////////////////////////////////////
void F_API FogSetEnd(f32 val)
{
	g_fogEnd = val;

	g_p3DDevice->SetRenderState( D3DRS_FOGEND, *(DWORD*)(&val));
}

/////////////////////////////////////
// Name:	FogSetDensity
// Purpose:	set the density of fog,
//			used in exp/exp2 mode
// Output:	fog density set
// Return:	none
/////////////////////////////////////
void F_API FogSetDensity(f32 val)
{
	g_fogDensity = val;

	g_p3DDevice->SetRenderState( D3DRS_FOGDENSITY, *(DWORD*)(&val));
}

//call this after the device has been restored
void FogOnReset()
{
	FogEnable(g_fogEnable);
	FogSetMode(g_fogMode);
	FogSetColor(g_fogClr);
	FogSetStart(g_fogStart);
	FogSetEnd(g_fogEnd);
	FogSetDensity(g_fogDensity);
}