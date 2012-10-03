#include "..//gfx_i.h"

#include "GFX\gfx_math.h"

#include "gfx_qbsp.h"
#include "gfx_qbsp_i.h"

/////////////////////////////////////
// Purpose:	get the number of models
// Output:	none
// Return:	number of models
/////////////////////////////////////
u32	IgfxQBSP::GetNumModels()
{
	return m_numModels;
}

/////////////////////////////////////
// Purpose:	get the bbox
// Output:	pOut filled
// Return:	none
/////////////////////////////////////
void IgfxQBSP::RayModelGetBBoxMin(u32 modelInd, Vec3D *pOut)
{
	(*pOut) = m_models[modelInd].min+m_models[modelInd].wrldTrans;
}

/////////////////////////////////////
// Purpose:	get the bbox
// Output:	pOut filled
// Return:	none
/////////////////////////////////////
void IgfxQBSP::RayModelGetBBoxMax(u32 modelInd, Vec3D *pOut)
{
	(*pOut) = m_models[modelInd].max+m_models[modelInd].wrldTrans;
}

/////////////////////////////////////
// Purpose:	get radius of the model
// Output:	none
// Return:	the radius
/////////////////////////////////////
f32 IgfxQBSP::RayModelGetRadius(u32 modelInd)
{
	return m_models[modelInd].radius;
}

static inline void _mdlCollBrush(ConvexPolyhedron *pBrush, u8 bIsPt, 
							  const Vec3D *mins, const Vec3D *maxs, 
							  const Vec3D & p, const Vec3D & dir, 
							  gfxTrace *pTrace)
{
	s32 txtInd = -1;

	//go through each half-space and determine the interval t[min,max]
	f32 tMin=0, tMax=1;

	f32 vDotn, pDotn, tInt;

	f32 *pPlane, ofs[3], dist;

	s32 indMin=0, indMax=0;

	//go through the sides
	for(s32 i = 0; i < pBrush->numPlane; i++)
	{
		if(tMin > tMax) //no intersect!
			break;

		pPlane = (f32*)pBrush->tPlanes[i];

		if(!bIsPt)
		{	// general box case
			// push the plane out apropriately for mins/maxs
			ofs[0] = pPlane[0] < 0 ? maxs->x : mins->x;
			ofs[1] = pPlane[1] < 0 ? maxs->y : mins->y;
			ofs[2] = pPlane[2] < 0 ? maxs->z : mins->z;

			dist = pPlane[0]*ofs[0] + pPlane[1]*ofs[1] + pPlane[2]*ofs[2];
			dist += pPlane[3];//-((-pPlane[ePD]) - dist);
		}
		else
		{	// special point case
			dist = pPlane[3];
		}

		//normal dot ray vector
		vDotn = pPlane[0]*dir.x + pPlane[1]*dir.y + pPlane[2]*dir.z;

		pDotn = pPlane[0]*p.x + pPlane[1]*p.y + pPlane[2]*p.z;

		if(vDotn < 0)
		{
			tInt = -(pDotn + dist - DIST_EPSILON)/vDotn;

			if (tInt > tMin) {
			  tMin = tInt;
			  indMin = i;
			}
			
		}
		else if(vDotn > 0)
		{
			tInt = -(pDotn + dist + DIST_EPSILON)/vDotn;

			if(tInt < tMax)
			{
				tMax = tInt;
				indMax = i;
			}

		}
		else
		{
			if(pDotn + dist > 0)
				tMin = tMax+1;
		}
	}

	//now let's see if we intersect something...
	if(tMin < tMax && tMin < pTrace->t)
	{
		pTrace->t = tMin;

		pTrace->norm.x = pBrush->tPlanes[indMin].a;
		pTrace->norm.y = pBrush->tPlanes[indMin].b;
		pTrace->norm.z = pBrush->tPlanes[indMin].c;
		pTrace->d	   = -pBrush->tPlanes[indMin].d;

		pTrace->txtInd   = (u32)pBrush->txtID[indMin];
		
		pTrace->bStartSolid = tMin < 0 ? TRUE : FALSE;
		pTrace->bAllSolid = (tMax < 0 && pTrace->bStartSolid) ? TRUE : FALSE;

		pTrace->brushIndex = -1;
	}
	else
	{
		if(tMin < 0)
		{
			pTrace->bStartSolid = TRUE;

			if(tMax < 0)
				pTrace->bAllSolid = TRUE;

			pTrace->brushIndex = -1;
		}
	}
}

/////////////////////////////////////
// Purpose:	find collision with the given
//			model inside the map.
//			NOTE: testT is the starting
//				  't' value (starting minimum)
//				  usu. set as 't = 1'
//				  set mins & maxs to 0 to test
//				  as point
// Output:	pTrace filled
// Return:	true if collided
/////////////////////////////////////
u8 IgfxQBSP::ModelCollision(u32 modelInd, const Vec3D *mins, const Vec3D *maxs,
							const Vec3D &pt1, const Vec3D &pt2, gfxTrace *pTrace, f32 testT)
{
	memset(pTrace, 0, sizeof(gfxTrace));
	pTrace->t = testT;

	if(TESTFLAGS(m_models[modelInd].status, QBSP_MODEL_FLAG_DISABLE))
		return FALSE;

	Vec3D vec(pt2-pt1);

	//determine if we are colliding with bbox or pt.
	u8 bIsPt = (mins && maxs) ? FALSE : TRUE;

	Vec3D bbMin, bbMax;
	RayModelGetBBoxMin(modelInd, &bbMin);
	RayModelGetBBoxMax(modelInd, &bbMax);

	// build a bounding box of the entire move
	Vec3D trace_absmins, trace_absmaxs;
	Vec3D point;

	BoundsClear(trace_absmins, trace_absmaxs);

	point = mins ? pt1 + (*mins) : pt1;
	BoundsAddPoint(point, trace_absmins, trace_absmaxs);
	point = maxs ? pt1 + (*maxs) : pt1;
	BoundsAddPoint(point, trace_absmins, trace_absmaxs);
	point = mins ? pt2 + (*mins) : pt2;
	BoundsAddPoint(point, trace_absmins, trace_absmaxs);
	point = maxs ? pt2 + (*maxs) : pt2;
	BoundsAddPoint(point, trace_absmins, trace_absmaxs);

	if(BoundsIntersect(bbMin, bbMax, trace_absmins, trace_absmaxs))
	{
		// Loop through and check all of the brushes in this model
		for(s32 i = 0; i < m_models[modelInd].numOfBrushes; i++)
		{
			_mdlCollBrush(&m_models[modelInd].brushes[i], bIsPt, mins, maxs, 
				pt1, vec, pTrace);
		}
	}

	if(pTrace->t < testT)
		return TRUE;

	return FALSE;
}

static inline void _BrushTransormPlanes(ConvexPolyhedron *pBrush, Matrix *pMtx)
{
	//go through the sides
	for(s32 i = 0; i < pBrush->numPlane; i++)
	{
		PlaneTransform(pMtx, &pBrush->planes[i], &pBrush->tPlanes[i]);
		PlaneNormalize(&pBrush->tPlanes[i], &pBrush->tPlanes[i]);
	}
}

void IgfxQBSP::_ModelTransformPlanes(gfxBSPModel *mdl)
{
	Matrix  InvTMtx;

	MtxInverse(&mdl->wrldMtx[FXMTX_NORMAL], &InvTMtx);
	MtxTranspose(&InvTMtx, &InvTMtx);

	// Loop through and tranform the brush side planes
	for(s32 i = 0; i < mdl->numOfBrushes; i++)
		_BrushTransormPlanes(&mdl->brushes[i], &InvTMtx);
}

/////////////////////////////////////
// Purpose:	translate the model
// Output:	model is translated
//			as well as it's planes
// Return:	none
/////////////////////////////////////
void IgfxQBSP::ModelTranslate(u32 modelInd, const Vec3D & trans, u8 bAdd)
{
	gfxBSPModel *mdl = &m_models[modelInd];

	if(bAdd)
		mdl->wrldTrans += trans;
	else
		mdl->wrldTrans = trans;

	mdl->wrldMtx[FXMTX_NORMAL]._41 = mdl->wrldTrans.x;
	mdl->wrldMtx[FXMTX_NORMAL]._42 = mdl->wrldTrans.y;
	mdl->wrldMtx[FXMTX_NORMAL]._43 = mdl->wrldTrans.z;

	//transform the planes
	for(int i = 0; i < mdl->numOfBrushes; i++)
	{
		ConvexPolyhedron *pBrush = &mdl->brushes[i];

		for(int j = 0; j < pBrush->numPlane; j++)
		{
			pBrush->tPlanes[j].d = pBrush->planes[j].d - (pBrush->planes[j].a*mdl->wrldTrans.x + pBrush->planes[j].b*mdl->wrldTrans.y + pBrush->planes[j].c*mdl->wrldTrans.z);
		}
	}
	
	//_ModelTransformPlanes(qbsp, mdl);

}

/////////////////////////////////////
// Purpose:	rotate the model
// Output:	model is rotated
//			as well as its planes
// Return:	none
/////////////////////////////////////
void IgfxQBSP::ModelRotate(u32 modelInd, const Vec3D & rot, u8 bAdd)
{
	if(bAdd)
		m_models[modelInd].wrldRot += rot;
	else
		m_models[modelInd].wrldRot = rot;

	MtxYawPitchRoll(
		m_models[modelInd].wrldRot.y,
		m_models[modelInd].wrldRot.x,
		m_models[modelInd].wrldRot.z,
		&m_models[modelInd].wrldMtx[FXMTX_NORMAL]); 

	m_models[modelInd].wrldMtx[FXMTX_NORMAL]._41 = m_models[modelInd].wrldTrans.x;
	m_models[modelInd].wrldMtx[FXMTX_NORMAL]._42 = m_models[modelInd].wrldTrans.y;
	m_models[modelInd].wrldMtx[FXMTX_NORMAL]._43 = m_models[modelInd].wrldTrans.z;

	//transform the planes
	_ModelTransformPlanes(&m_models[modelInd]);
}

/////////////////////////////////////
// Purpose:	rotate the model, axis style!
// Output:	model rotated
// Return:	none
/////////////////////////////////////
void IgfxQBSP::ModelRotateAxis(u32 modelInd, const Vec3D & axis, f32 rot)
{
	MtxAxisRotation(&axis, rot, &m_models[modelInd].wrldMtx[FXMTX_NORMAL]);

	m_models[modelInd].wrldMtx[FXMTX_NORMAL]._41 = m_models[modelInd].wrldTrans.x;
	m_models[modelInd].wrldMtx[FXMTX_NORMAL]._42 = m_models[modelInd].wrldTrans.y;
	m_models[modelInd].wrldMtx[FXMTX_NORMAL]._43 = m_models[modelInd].wrldTrans.z;

	//transform the planes
	_ModelTransformPlanes(&m_models[modelInd]);
}

/////////////////////////////////////
// Purpose:	enable/disable model
//			If disabled, model will
//			not perform collision
//			and will not be displayed
// Output:	model status set
// Return:	none
/////////////////////////////////////
void IgfxQBSP::ModelEnable(u32 modelInd, u8 bEnable)
{
	if(bEnable)
		CLEARFLAG(m_models[modelInd].status, QBSP_MODEL_FLAG_DISABLE);
	else
		SETFLAG(m_models[modelInd].status, QBSP_MODEL_FLAG_DISABLE);
}


//transform the plane model space
static inline void _BrushRawTransormPlanes(ConvexPolyhedron *pBrush, Matrix *pMtx)
{
	//go through the sides
	for(int i = 0; i < pBrush->numPlane; i++)
	{
		pBrush->planes[i].d *= -1; //convert d's sign, for some reason, PlaneTransform doesn't
								   //like id software's plane format...
		PlaneTransform(pMtx, &pBrush->planes[i], &pBrush->planes[i]);
		PlaneNormalize(&pBrush->planes[i], &pBrush->planes[i]);

		pBrush->tPlanes[i] = pBrush->planes[i];
	}
}

//translate the actual raw vertex data and plane
void IgfxQBSP::_ModelTranslate(gfxVtxQBSP *pVtx, gfxBSPModel *mdl, const Vec3D & trans)
{
	if(!pVtx)
		return;

	s32 i;

	///////////////////////////////////
	// Translate the Vertices
	s32 maxFace = mdl->faceIndex + mdl->numOfFaces;
	for(i = mdl->faceIndex; i < maxFace; i++)
	{
		s32 j, maxVert;

		//assume face is a polygon...
		switch(m_faces[i].type)
		{
		case eFacePolygon:
			maxVert = m_faces[i].startVertIndex+m_faces[i].numOfVerts;
			for(j = m_faces[i].startVertIndex; j < maxVert; j++)
				pVtx[j].pt += trans;
			break;

		case eFacePatch:
			break;

		case eFaceMesh:
			break;

		case eFaceBillboard:
			pVtx[m_faces[i].startVertIndex].pt += trans;
			break;
		}
	}
	///////////////////////////////////

	///////////////////////////////////
	// Translate the Planes
	Matrix  InvTMtx;
	Vec3D scale(0.995f, 0.995f, 0.995f);

	MtxScale(&scale, &InvTMtx);
	InvTMtx._14 = -trans.x;
	InvTMtx._24 = -trans.y;
	InvTMtx._34 = -trans.z;

	//D3DXMatrixTranslation(&InvTMtx, trans[eX], trans[eY], trans[eZ]);
	//D3DXMatrixInverse(&InvTMtx, NULL, &InvTMtx);
	//D3DXMatrixTranspose(&InvTMtx, &InvTMtx);

	// Loop through and tranform the brush side planes
	for(i = 0; i < mdl->numOfBrushes; i++)
		_BrushRawTransormPlanes(&mdl->brushes[i], &InvTMtx);
	///////////////////////////////////
}

/////////////////////////////////////
// Purpose:	get the translate of 
//			the model
// Output:	ptOut
// Return:	none
/////////////////////////////////////
void IgfxQBSP::ModelGetTranslate(s32 modelInd, Vec3D *ptOut)
{
	(*ptOut) = m_models[modelInd].wrldTrans;
}

/////////////////////////////////////
// Purpose:	get the rotation of 
//			the model
// Output:	ptOut
// Return:	none
/////////////////////////////////////
void IgfxQBSP::ModelGetRotate(s32 modelInd, Vec3D *rotOut)
{
	(*rotOut) = m_models[modelInd].wrldRot;
}

void QBSPMDLShadowCallback(void *usrDat, u32 usrParam, u8 bLock, gfxShadowInfo *pSIOut)
{
	hIQBSP qbsp = (hIQBSP)usrDat;
	s32   ind  = (s32)usrParam;

	if(qbsp)
	{
		if(bLock)
		{
			Vec3D *shVtxBuff = qbsp->GetSHVtxBuff();
			u16	  *shIndBuff = qbsp->GetSHIndBuff();

			gfxVtxQBSP *qbspVtx;
			u16		   *qbspInd;

			//combine all the faces into one geometry
			VtxLock(qbsp->GetVtxBuff(), 0, 0,0, (void**)&qbspVtx, D3DLOCK_READONLY);
			IndLock(qbsp->GetIndBuff(), 0,0, (void**)&qbspInd, D3DLOCK_READONLY);

			pSIOut->numVtx = 0;
			pSIOut->numFace = 0;

			const gfxBSPModel & model = qbsp->GetModel(ind);

			s32 faceInd;
			for(s32 f = 0; f < model.numOfFaces; f++)
			{
				faceInd = f+model.faceIndex;

				const gfxBSPFace & face = qbsp->GetFace(faceInd);

				switch(face.type)
				{
				case eFaceMesh:
				case eFacePolygon:
					{
						s32 i;

						s32 numInd = face.numMeshVerts;
						for(i = 0; i < numInd; i++)
						{
							shVtxBuff[pSIOut->numVtx + i] = qbspVtx[qbsp->GetFace(faceInd).startVertIndex+qbspInd[i+face.meshVertIndex]].pt;
							shIndBuff[pSIOut->numVtx + i] = pSIOut->numVtx + i;
						}
					}
					break;

				case eFacePatch: //not worth it...
					break;
				}

				pSIOut->numVtx += face.numMeshVerts;
				pSIOut->numFace = pSIOut->numVtx/3;
			}

			VtxUnlock(qbsp->GetVtxBuff(), 0);
			IndUnlock(qbsp->GetIndBuff());

			pSIOut->vOut = shVtxBuff;
			pSIOut->iOut = shIndBuff;
		}
		else
		{
		}
	}
}

/////////////////////////////////////
// Purpose:	render the given model
// Output:	scene object added
// Return:	true
/////////////////////////////////////
u32 IgfxQBSP::ModelRender(s32 modelInd, const Vec3D & camPos, u8 bShadow)
{
	// model has to be enabled
	if(m_models[modelInd].numOfFaces > 0 && !TESTFLAGS(m_models[modelInd].status, QBSP_MODEL_FLAG_DISABLE))
	{
		g_fragmentFrame++;

		Vec3D bbMin, bbMax;

		s32 f;

		RayModelGetBBoxMin(modelInd, &bbMin);
		RayModelGetBBoxMax(modelInd, &bbMax);

		u8 bOBJCulled = !FrustrumCheckBox(bbMin, bbMax);
		u8 bLightCulled = FALSE;

		//set the lights up.
		LightGetAllNearest(bbMin, bbMax, m_models[modelInd].lights);

		//our shadow light
		hLIGHT shL = _LightGetShadow(m_models[modelInd].lights);
	
		if(shL)
			bLightCulled = !FrustrumCheckPoint(shL->lDat.Position.x,
			shL->lDat.Position.y, shL->lDat.Position.z);
		
		// not within frustrum?
		if(bOBJCulled)
		{
			for(f = 0; f < m_models[modelInd].numOfFaces; f++)
				m_faces[f+m_models[modelInd].faceIndex].fragmentframe = g_fragmentFrame;
		}
		else
		{
			//Vec3D vCamObj(m_models[modelInd].wrldTrans.x-camPos.x, m_models[modelInd].wrldTrans.y-camPos.y, m_models[modelInd].wrldTrans.z-camPos.z);
			f32 lod_lvl = CameraGetBoundFarDistSq(m_models[modelInd].min, 
				m_models[modelInd].max, &m_models[modelInd].wrldMtx[FXMTX_NORMAL]); //Vec3DDot(&vCamObj,&vCamObj);

			//calculate the matrices
			MtxInverse(&m_models[modelInd].wrldMtx[FXMTX_NORMAL], 
				&m_models[modelInd].wrldMtx[FXMTX_INVERSE]);
			MtxTranspose(&m_models[modelInd].wrldMtx[FXMTX_NORMAL], 
				&m_models[modelInd].wrldMtx[FXMTX_TRANSPOSE]);
			MtxTranspose(&m_models[modelInd].wrldMtx[FXMTX_INVERSE], 
				&m_models[modelInd].wrldMtx[FXMTX_INVTRANSPOSE]);

			s32 faceInd;

			for(f = 0; f < m_models[modelInd].numOfFaces; f++)
			{
				faceInd = f+m_models[modelInd].faceIndex;

				if(m_textures[m_faces[faceInd].textureID].fx == m_dfltFX)
					//if we are using qbsp's default FX, then use the model technique
					_RenderFace(faceInd, m_models[modelInd].wrldMtx, 0,
						m_mdlFXTech, m_models[modelInd].lights, lod_lvl);
				else
					_RenderFace(faceInd, m_models[modelInd].wrldMtx, 0,
						0, m_models[modelInd].lights, lod_lvl);
			}
		}

		//add shadowing
		if(shL && bShadow)
		{
			//FIXME!: check to see if vector: obj-light is intersecting frustrum!!!!
			//        if bLightCulled is true
			if((!bLightCulled && !bOBJCulled)
				|| (bLightCulled))
			{
				ShadowDat shadow;

				//get the light position depending on type, for directional we only want direction.
				if(shL->lDat.Type == GFXLIGHT_DIRECTIONAL)
				{
					shadow.lPos = shL->lDat.Direction;
					shadow.flag = SHADOW_USE_DIR;
				}
				else
				{
					//Vec3DTransformCoord(&shL->lDat.Position, 
					//	&obj->wrldMtx[FXMTX_INVERSE], &shadow.lPos);
					shadow.lPos = shL->lDat.Position;
					shadow.flag = 0;
				}

				if(bOBJCulled)
					SETFLAG(shadow.flag, SHADOW_OCC_CULLED);

				//calculate the scissor area rect in 2d

				shadow.pWrldMtx = &m_models[modelInd].wrldMtx[FXMTX_NORMAL];
				shadow.extrude = SHADOW_EXTRUDE;//shL->lDat.Range; //

				shadow.cb = QBSPMDLShadowCallback;
				shadow.usrDat = (void*)this;
				shadow.usrParam = modelInd;

				GFXShadowAdd(&shadow);
			}
		}
	}

	return TRUE;
}