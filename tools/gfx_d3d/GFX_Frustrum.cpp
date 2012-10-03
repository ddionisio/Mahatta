#include "gfx_i.h"

typedef enum
{
	eRIGHT,
	eLEFT,
	eBOTTOM,
	eTOP,
	eBACK,
	eFRONT,
	eNUMSIDE
} eFrustumSide;

//the six side frustrum plane
Plane g_Frustum[eNUMSIDE];


/////////////////////////////////////
// Name:	FrustrumCalculate
// Purpose:	calculate the viewing
//			frustrum
// Output:	frustrum plane calculated
// Return:	none
/////////////////////////////////////
void FrustrumCalculate()
{
	//set the right side
	g_Frustum[eRIGHT].a = g_mtxViewProj[FXMTX_NORMAL]._14 - g_mtxViewProj[FXMTX_NORMAL]._11;
	g_Frustum[eRIGHT].b = g_mtxViewProj[FXMTX_NORMAL]._24 - g_mtxViewProj[FXMTX_NORMAL]._21;
	g_Frustum[eRIGHT].c = g_mtxViewProj[FXMTX_NORMAL]._34 - g_mtxViewProj[FXMTX_NORMAL]._31;
	g_Frustum[eRIGHT].d = g_mtxViewProj[FXMTX_NORMAL]._44 - g_mtxViewProj[FXMTX_NORMAL]._41;

	//set the left side
	g_Frustum[eLEFT].a = g_mtxViewProj[FXMTX_NORMAL]._14 + g_mtxViewProj[FXMTX_NORMAL]._11;
	g_Frustum[eLEFT].b = g_mtxViewProj[FXMTX_NORMAL]._24 + g_mtxViewProj[FXMTX_NORMAL]._21;
	g_Frustum[eLEFT].c = g_mtxViewProj[FXMTX_NORMAL]._34 + g_mtxViewProj[FXMTX_NORMAL]._31;
	g_Frustum[eLEFT].d = g_mtxViewProj[FXMTX_NORMAL]._44 + g_mtxViewProj[FXMTX_NORMAL]._41;

	//set the bottom side
	g_Frustum[eBOTTOM].a = g_mtxViewProj[FXMTX_NORMAL]._14 + g_mtxViewProj[FXMTX_NORMAL]._12;
	g_Frustum[eBOTTOM].b = g_mtxViewProj[FXMTX_NORMAL]._24 + g_mtxViewProj[FXMTX_NORMAL]._22;
	g_Frustum[eBOTTOM].c = g_mtxViewProj[FXMTX_NORMAL]._34 + g_mtxViewProj[FXMTX_NORMAL]._32;
	g_Frustum[eBOTTOM].d = g_mtxViewProj[FXMTX_NORMAL]._44 + g_mtxViewProj[FXMTX_NORMAL]._42;

	//set the top side
	g_Frustum[eTOP].a = g_mtxViewProj[FXMTX_NORMAL]._14 - g_mtxViewProj[FXMTX_NORMAL]._12;
	g_Frustum[eTOP].b = g_mtxViewProj[FXMTX_NORMAL]._24 - g_mtxViewProj[FXMTX_NORMAL]._22;
	g_Frustum[eTOP].c = g_mtxViewProj[FXMTX_NORMAL]._34 - g_mtxViewProj[FXMTX_NORMAL]._32;
	g_Frustum[eTOP].d = g_mtxViewProj[FXMTX_NORMAL]._44 - g_mtxViewProj[FXMTX_NORMAL]._42;

	//set the back side
	g_Frustum[eBACK].a = g_mtxViewProj[FXMTX_NORMAL]._14 - g_mtxViewProj[FXMTX_NORMAL]._13;
	g_Frustum[eBACK].b = g_mtxViewProj[FXMTX_NORMAL]._24 - g_mtxViewProj[FXMTX_NORMAL]._23;
	g_Frustum[eBACK].c = g_mtxViewProj[FXMTX_NORMAL]._34 - g_mtxViewProj[FXMTX_NORMAL]._33;
	g_Frustum[eBACK].d = g_mtxViewProj[FXMTX_NORMAL]._44 - g_mtxViewProj[FXMTX_NORMAL]._43;

	//set the front side
	g_Frustum[eFRONT].a = g_mtxViewProj[FXMTX_NORMAL]._14 - g_mtxViewProj[FXMTX_NORMAL]._13;
	g_Frustum[eFRONT].b = g_mtxViewProj[FXMTX_NORMAL]._24 - g_mtxViewProj[FXMTX_NORMAL]._23;
	g_Frustum[eFRONT].c = g_mtxViewProj[FXMTX_NORMAL]._34 - g_mtxViewProj[FXMTX_NORMAL]._33;
	g_Frustum[eFRONT].d = g_mtxViewProj[FXMTX_NORMAL]._44 - g_mtxViewProj[FXMTX_NORMAL]._43;
}

/////////////////////////////////////
// Name:	FrustrumCheckPoint
// Purpose:	check point in frustrum
// Output:	none
// Return:	TRUE if so
/////////////////////////////////////
u8 F_API FrustrumCheckPoint(f32 x, f32 y, f32 z)
{
	//just go through and plug-in the point to check if > 0
	for(int i = 0; i < eNUMSIDE; i++ )
	{
		// Calculate the plane equation and check if the point is behind a side of the frustum
		if(g_Frustum[i].a * x + g_Frustum[i].b * y + g_Frustum[i].c * z + g_Frustum[i].d <= 0)
		{
			// The point was behind a side, so it ISN'T in the frustum
			return FALSE;
		}
	}

	return TRUE;
}

/////////////////////////////////////
// Name:	FrustrumCheckSphere
// Purpose:	check sphere in frustrum
// Output:	none
// Return:	TRUE if so
/////////////////////////////////////
u8 F_API FrustrumCheckSphere(f32 x, f32 y, f32 z, f32 r)
{
	//just go through and plug-in the point to check within radius
	for(int i = 0; i < eNUMSIDE; i++ )
	{
		// Calculate the plane equation and check if the point is behind a side of the frustum
		if(g_Frustum[i].a * x + g_Frustum[i].b * y + g_Frustum[i].c * z + g_Frustum[i].d <= -r)
		{
			// The point was behind a side, so it ISN'T in the frustum
			return FALSE;
		}
	}

	return TRUE;
}

/////////////////////////////////////
// Name:	FrustrumCheckCube
// Purpose:	check cube in frustrum
// Output:	none
// Return:	TRUE if so
/////////////////////////////////////
u8 F_API FrustrumCheckCube(f32 x, f32 y, f32 z, f32 s)
{
	for(int i = 0; i < eNUMSIDE; i++ )
	{
		if(g_Frustum[i].a * (x - s) + g_Frustum[i].b * (y - s) + g_Frustum[i].c * (z - s) + g_Frustum[i].d > 0)
		   continue;
		if(g_Frustum[i].a * (x + s) + g_Frustum[i].b * (y - s) + g_Frustum[i].c * (z - s) + g_Frustum[i].d > 0)
		   continue;
		if(g_Frustum[i].a * (x - s) + g_Frustum[i].b * (y + s) + g_Frustum[i].c * (z - s) + g_Frustum[i].d > 0)
		   continue;
		if(g_Frustum[i].a * (x + s) + g_Frustum[i].b * (y + s) + g_Frustum[i].c * (z - s) + g_Frustum[i].d > 0)
		   continue;
		if(g_Frustum[i].a * (x - s) + g_Frustum[i].b * (y - s) + g_Frustum[i].c * (z + s) + g_Frustum[i].d > 0)
		   continue;
		if(g_Frustum[i].a * (x + s) + g_Frustum[i].b * (y - s) + g_Frustum[i].c * (z + s) + g_Frustum[i].d > 0)
		   continue;
		if(g_Frustum[i].a * (x - s) + g_Frustum[i].b * (y + s) + g_Frustum[i].c * (z + s) + g_Frustum[i].d > 0)
		   continue;
		if(g_Frustum[i].a * (x + s) + g_Frustum[i].b * (y + s) + g_Frustum[i].c * (z + s) + g_Frustum[i].d > 0)
		   continue;

		// If we get here, it isn't in the frustum
		return FALSE;
	}

	return TRUE;
}

/////////////////////////////////////
// Name:	FrustrumCheckBox
// Purpose:	check box in frustrum
// Output:	none
// Return:	TRUE if so
/////////////////////////////////////
u8 F_API FrustrumCheckBox(const Vec3D & min, const Vec3D & max)
{
	for(int i = 0; i < eNUMSIDE; i++ )
	{
		if(g_Frustum[i].a * min.x + g_Frustum[i].b * min.y + g_Frustum[i].c * min.z + g_Frustum[i].d > 0)  continue;
		if(g_Frustum[i].a * max.x + g_Frustum[i].b * min.y + g_Frustum[i].c * min.z + g_Frustum[i].d > 0)  continue;
		if(g_Frustum[i].a * min.x + g_Frustum[i].b * max.y + g_Frustum[i].c * min.z + g_Frustum[i].d > 0)  continue;
		if(g_Frustum[i].a * max.x + g_Frustum[i].b * max.y + g_Frustum[i].c * min.z + g_Frustum[i].d > 0)  continue;
		if(g_Frustum[i].a * min.x + g_Frustum[i].b * min.y + g_Frustum[i].c * max.z + g_Frustum[i].d > 0)  continue;
		if(g_Frustum[i].a * max.x + g_Frustum[i].b * min.y + g_Frustum[i].c * max.z + g_Frustum[i].d > 0)  continue;
		if(g_Frustum[i].a * min.x + g_Frustum[i].b * max.y + g_Frustum[i].c * max.z + g_Frustum[i].d > 0)  continue;
		if(g_Frustum[i].a * max.x + g_Frustum[i].b * max.y + g_Frustum[i].c * max.z + g_Frustum[i].d > 0)  continue;

		// If we get here, it isn't in the frustum
		return FALSE;
	}

	return TRUE;
}

#define V_BOX_IN			1
#define V_BOX_OUT			2
#define V_BOX_INTERSECT		3

/////////////////////////////////////
// Name:	FrustrumCheckBoxEX
// Purpose:	check box in frustrum
// Output:	none
// Return:	BBOX_IN
//			BBOX_OUT
//			BBOX_INTERSECT
/////////////////////////////////////
s32 F_API FrustrumCheckBoxEX(const Vec3D & min, const Vec3D & max)
{
	BYTE mode=0;					// set IN and OUT bit to 0
	for(int i = 0; i < eNUMSIDE; i++ )
	{
		mode &= V_BOX_OUT;				// clear the IN bit to 0 
		
		if(g_Frustum[i].a * min.x + g_Frustum[i].b * min.y + g_Frustum[i].c * min.z + g_Frustum[i].d > 0)  mode |= V_BOX_IN;
		else mode |= V_BOX_OUT;

		if(mode == V_BOX_INTERSECT) continue;

		if(g_Frustum[i].a * max.x + g_Frustum[i].b * min.y + g_Frustum[i].c * min.z + g_Frustum[i].d > 0)  mode |= V_BOX_IN;
		else mode |= V_BOX_OUT;

		if(mode == V_BOX_INTERSECT) continue;

		if(g_Frustum[i].a * min.x + g_Frustum[i].b * max.y + g_Frustum[i].c * min.z + g_Frustum[i].d > 0)  mode |= V_BOX_IN;
		else mode |= V_BOX_OUT;

		if(mode == V_BOX_INTERSECT) continue;

		if(g_Frustum[i].a * max.x + g_Frustum[i].b * max.y + g_Frustum[i].c * min.z + g_Frustum[i].d > 0)  mode |= V_BOX_IN;
		else mode |= V_BOX_OUT;

		if(mode == V_BOX_INTERSECT) continue;

		if(g_Frustum[i].a * min.x + g_Frustum[i].b * min.y + g_Frustum[i].c * max.z + g_Frustum[i].d > 0)  mode |= V_BOX_IN;
		else mode |= V_BOX_OUT;

		if(mode == V_BOX_INTERSECT) continue;

		if(g_Frustum[i].a * max.x + g_Frustum[i].b * min.y + g_Frustum[i].c * max.z + g_Frustum[i].d > 0)  mode |= V_BOX_IN;
		else mode |= V_BOX_OUT;

		if(mode == V_BOX_INTERSECT) continue;

		if(g_Frustum[i].a * min.x + g_Frustum[i].b * max.y + g_Frustum[i].c * max.z + g_Frustum[i].d > 0)  mode |= V_BOX_IN;
		else mode |= V_BOX_OUT;

		if(mode == V_BOX_INTERSECT) continue;

		if(g_Frustum[i].a * max.x + g_Frustum[i].b * max.y + g_Frustum[i].c * max.z + g_Frustum[i].d > 0)  mode |= V_BOX_IN;
		else mode |= V_BOX_OUT;

		if(mode == V_BOX_INTERSECT) continue;

		if (mode == V_BOX_IN) continue;

		// If we get here, it isn't in the frustum
		return BBOX_OUT;
	}

	if (mode == V_BOX_INTERSECT) return BBOX_INTERSECT;

	return BBOX_IN;
}