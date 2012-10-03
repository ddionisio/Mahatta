#include "gfx_i.h"

LPD3DXLINE g_gfxLine;

void GFXLineDestroy()
{
	SAFE_RELEASE(g_gfxLine);
}

s32 GFXLineInit()
{
	GFXLineDestroy();

	return SUCCEEDED(D3DXCreateLine(g_p3DDevice, &g_gfxLine));
}

void GFXLineInvalidate()
{
	if(g_gfxLine)
		g_gfxLine->OnLostDevice();
}

void GFXLineRestore()
{
	if(g_gfxLine)
		g_gfxLine->OnResetDevice();
}

/////////////////////////////////////
// Name:	LineDraw
// Purpose:	draws a line strip in
//			screen-space.
// Output:	line strip drawn in 2d
// Return:	TRUE if success
/////////////////////////////////////
u32 F_API LineDraw(const Vec2D *pVtxList, u32 vtxNum, u32 clr)
{
	if(g_gfxLine)
		return SUCCEEDED(g_gfxLine->Draw((CONST D3DXVECTOR2 *)pVtxList, vtxNum, clr));

	return FALSE;
}

/////////////////////////////////////
// Name:	LineDraw3D
// Purpose:	draws a line strip in
//			screen-space (transformed)
// Output:	line strip drawn
// Return:	TRUE if success
/////////////////////////////////////
u32 F_API LineDraw3D(const Vec3D *pVtxList, u32 vtxNum, const Matrix *pMtx, u32 clr)
{
	if(g_gfxLine)
		return SUCCEEDED(g_gfxLine->DrawTransform((CONST D3DXVECTOR3 *)pVtxList, 
												vtxNum, (CONST D3DXMATRIX *)pMtx, clr));

	return FALSE;
}

/////////////////////////////////////
// Name:	LineGetAntiAlias
// Purpose:	Gets the line antialiasing 
//			state. 
// Output:	none
// Return:	the anti-alias state
/////////////////////////////////////
u8 F_API LineGetAntiAlias()
{
	return g_gfxLine ? g_gfxLine->GetAntialias() : FALSE;
}

/////////////////////////////////////
// Name:	LineSetAntiAlias
// Purpose:	Toggles line antialiasing.
// Output:	antialias set on/off
// Return:	none
/////////////////////////////////////
void F_API LineSetAntiAlias(u8 bState)
{
	if(g_gfxLine)
		g_gfxLine->SetAntialias(bState);
}

/////////////////////////////////////
// Name:	LineGetPattern
// Purpose:	Gets the line stipple pattern. 
// Output:	none
// Return:	the line stipple pattern: 
//			1 is opaque, 0 is transparent.
/////////////////////////////////////
u32 F_API LineGetPattern()
{
	return g_gfxLine ? g_gfxLine->GetPattern() : 0;
}

/////////////////////////////////////
// Name:	LineSetPattern
// Purpose:	Applies a stipple pattern 
//			to the line. 
// Output:	pattern set
// Return:	none
/////////////////////////////////////
void F_API LineSetPattern(u32 val)
{
	if(g_gfxLine)
		g_gfxLine->SetPattern(val);
}

/////////////////////////////////////
// Name:	LineGetPatternScale
// Purpose:	Gets the stipple-pattern 
//			scale value.
// Output:	none
// Return:	1.0f is the default value 
//			and represents no scaling. 
//			A value less than 1.0f shrinks 
//			the pattern, and a value greater 
//			than 1.0 stretches the pattern.
/////////////////////////////////////
f32 F_API LineGetPatternScale()
{
	return g_gfxLine ? g_gfxLine->GetPatternScale() : 1.0f;
}

/////////////////////////////////////
// Name:	LineSetPatternScale
// Purpose:	Stretches the stipple 
//			pattern along the line 
//			direction. 
// Output:	1.0f is the default value 
//			and represents no scaling. 
//			A value less than 1.0f shrinks 
//			the pattern, and a value greater 
//			than 1.0 stretches the pattern.
// Return:	none
/////////////////////////////////////
void F_API LineSetPatternScale(f32 val)
{
	if(g_gfxLine)
		g_gfxLine->SetPatternScale(val);
}

/////////////////////////////////////
// Name:	LineGetWidth
// Purpose:	Gets the thickness of 
//			the line. 
// Output:	none
// Return:	the thickness
/////////////////////////////////////
f32 F_API LineGetWidth()
{
	return g_gfxLine ? g_gfxLine->GetPatternScale() : 0.0f;
}

/////////////////////////////////////
// Name:	LineSetWidth
// Purpose:	Specifies the thickness 
//			of the line.
// Output:	thickness set
// Return:	none
/////////////////////////////////////
void F_API LineSetWidth(f32 val)
{
	if(g_gfxLine)
		g_gfxLine->SetWidth(val);
}