#include "gfx_i.h"

#include "GFX\gfx_math.h"

/////////////////////////////////////
// Name:	CameraInit
// Purpose:	initialize camera
// Output:	given pCam initialized
// Return:	none
/////////////////////////////////////
void F_API CameraInit(gfxCamera *pCam, const Vec3D & vEye, f32 yaw, f32 pitch, f32 roll)
{
	memset(pCam, 0, sizeof(gfxCamera));
	
	pCam->vUp.x=0;
	pCam->vUp.y=1;
	pCam->vUp.z=0;

	pCam->vEye.x = vEye.x;
	pCam->vEye.y = vEye.y;
	pCam->vEye.z = vEye.z;

	CameraYawPitchRoll(pCam, yaw, pitch, roll);
}

/////////////////////////////////////
// Name:	CameraSetScene
// Purpose:	set the camera to scene
// Output:	view matrix set
// Return:	none
/////////////////////////////////////
void F_API CameraSetScene(gfxCamera *pCam)
{
	MtxLookAt(&pCam->vEye, &pCam->vTarget, &pCam->vUp, &g_mtxView[FXMTX_NORMAL]);
	g_p3DDevice->SetTransform(D3DTS_VIEW, (const D3DMATRIX*)&g_mtxView[FXMTX_NORMAL]);

	MtxInverse(&g_mtxView[FXMTX_NORMAL], &g_mtxView[FXMTX_INVERSE]);
	MtxTranspose(&g_mtxView[FXMTX_NORMAL], &g_mtxView[FXMTX_TRANSPOSE]);
	MtxTranspose(&g_mtxView[FXMTX_INVERSE], &g_mtxView[FXMTX_INVTRANSPOSE]);

	//set viewproj matrix
   GFXSetViewProjMtxFX();

	//update the billboard to face this camera
   IgfxTexture::sBillboardPrepare(pCam->vDir);

	//calculate the frustrum planes
	FrustrumCalculate();

	//cull some lights
	LightCullView(pCam->vEye);

	//set the eye pt.
	g_eyePt = pCam->vEye;
	g_eyeVec = pCam->vTarget - pCam->vEye;
}

/////////////////////////////////////
// Name:	CameraMov
// Purpose:	move camera
// Output:	pCam moved
// Return:	none
/////////////////////////////////////
void F_API CameraMov(gfxCamera *pCam, const Vec3D & amt)
{
	pCam->vEye += amt;

	pCam->vTarget += amt;
}

/////////////////////////////////////
// Name:	CameraSet
// Purpose:	set camera's loc.
// Output:	pCam loc. set
// Return:	none
/////////////////////////////////////
void F_API CameraSet(gfxCamera *pCam, const Vec3D & vEye)
{
	//set up look-at
	pCam->vTarget += vEye - pCam->vEye;

	pCam->vEye = vEye;
}

/////////////////////////////////////
// Name:	CameraSetTarget
// Purpose:	set camera target
// Output:	set the camera target
// Return:	none
/////////////////////////////////////
void F_API CameraSetTarget(gfxCamera *pCam, const Vec3D & vTarget)
{
	pCam->vTarget = vTarget;

	//set up dir
	pCam->vDir = pCam->vTarget-pCam->vEye;
	Vec3DNormal(&pCam->vDir, &pCam->vDir);
}

/////////////////////////////////////
// Name:	CameraMovDir
// Purpose:	move camera to its dir.
// Output:	pCam moved
// Return:	none
/////////////////////////////////////
void F_API CameraMovDir(gfxCamera *pCam, f32 amt)
{
	pCam->vEye += pCam->vDir*amt;
	pCam->vTarget += pCam->vDir*amt;

	/*D3DXMATRIX mtxTran;
	D3DXMatrixTranslation(&mtxTran, -xAmt, -yAmt, -zAmt);
	D3DXMatrixMultiply(&cam->mtxCamera,&cam->mtxCamera,&mtxTran);*/
}

/////////////////////////////////////
// Name:	CameraYawPitchRoll
// Purpose:	rotate camera with
//			yaw, pitch and roll
// Output:	camera set
// Return:	none
/////////////////////////////////////
void F_API CameraYawPitchRoll(gfxCamera *pCam, f32 yaw, f32 pitch, f32 roll)
{
	f32 cosY,cosP,cosR, sinY,sinP,sinR;

	cosY = cosf(yaw);   sinY = sinf(yaw);
	cosP = cosf(pitch); sinP = sinf(pitch);
	cosR = cosf(roll);  sinR = sinf(roll);

	//set up dir(fwd vect)
	pCam->vDir.x = sinY*cosP;
	pCam->vDir.y = sinP;
	pCam->vDir.z = cosP*-cosY;

	//set up look-at
	pCam->vTarget = pCam->vDir+pCam->vEye;
}

//this will get the farthest distance (squared) of given AABB from current camera loc/vec
f32 CameraGetBoundFarDistSq(const Vec3D & min, const Vec3D & max, const Matrix * pWrldMtx)
{
	f32 farD = 0;

	Vec3D pts[8];
	
	pts[0] = min;
	pts[1].x = min.x; pts[1].y = min.y; pts[1].z = max.z;
	pts[2].x = min.x; pts[2].y = max.y; pts[2].z = max.z;
	pts[3] = max;
	pts[4].x = max.x; pts[4].y = max.y; pts[4].z = min.z;
	pts[5].x = max.x; pts[5].y = min.y; pts[5].z = min.z;
	pts[6].x = min.x; pts[6].y = max.y; pts[6].z = min.z;
	pts[7].x = max.x; pts[7].y = min.y; pts[7].z = max.z;

	Vec3D v;
	f32 d;

	for(u8 i = 0; i < 8; i++)
	{
		if(pWrldMtx) { Vec3DTransformCoord(&pts[i], pWrldMtx, &v); v -= g_eyePt; }
		else { v = pts[i] - g_eyePt; }

		d = Vec3DDot(&g_eyeVec, &v);

		//make sure we are facing the point
		if(d > 0)
		{
			d = Vec3DDot(&v, &v);
			if(d > farD)
				farD = d;
		}
	}

	return farD;
}