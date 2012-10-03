#include "gfx_i.h"

#include "GFX\gfx_math.h"

Vec3D g_skyboxTmpLoc(0,0,0);

/////////////////////////////////////
// Name:	SkyBoxBegin
// Purpose:	prepares the skybox render 
//			system
//			(do not subsitute GFXBegin()
//			with this!!!)
//			Between SkyBoxBegin and SkyBoxEnd,
//			you call skybox rendering,
//			but not SCENE!!!
// Output:	sky box rendering prepared
// Return:	success if success
/////////////////////////////////////
u8 F_API SkyBoxBegin()
{
	//////////////////////////////////////////////////
	//temporary view matrix
	g_skyboxTmpLoc.x = g_mtxView[FXMTX_NORMAL]._41;
	g_skyboxTmpLoc.y = g_mtxView[FXMTX_NORMAL]._42;
	g_skyboxTmpLoc.z = g_mtxView[FXMTX_NORMAL]._43;

	g_mtxView[FXMTX_NORMAL]._41 = 0; 
	g_mtxView[FXMTX_NORMAL]._42 = 0; 
	g_mtxView[FXMTX_NORMAL]._43 = 0;

	GFXSetViewProjMtxFX();

	//calculate the frustrum planes
	FrustrumCalculate();

	return TRUE;
}

/////////////////////////////////////
// Name:	SkyBoxEnd
// Purpose:	ends the skybox render system
// Output:	sky box rendering end
// Return:	success if success
/////////////////////////////////////
void F_API SkyBoxEnd()
{
	//reset the global view matrix
	g_mtxView[FXMTX_NORMAL]._41 = g_skyboxTmpLoc.x; 
	g_mtxView[FXMTX_NORMAL]._42 = g_skyboxTmpLoc.y; 
	g_mtxView[FXMTX_NORMAL]._43 = g_skyboxTmpLoc.z;

	GFXSetViewProjMtxFX();

	//calculate the frustrum planes
	FrustrumCalculate();

	//////////////////////////////////////////////////
}