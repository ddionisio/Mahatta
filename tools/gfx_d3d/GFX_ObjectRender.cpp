#include "cal3d/cal3d.h"
#include "gfx_i.h"

#include "GFX\gfx_math.h"

void OBJFXMeshCB(hIEFFECT effect, u32 wParam, s32 lParam)
{
	//set texture
	hTXT txt = (hTXT)wParam;

	//if(txt)
		effect->SetBaseTexture(0, txt);
}

void OBJFXCB(hIEFFECT effect, u32 wParam, s32 lParam)
{
	static u32 meshID; meshID = ((wParam&0xffff0000)>>16);
	static u32 meshSubID; meshSubID = (wParam&0x0000ffff);

	static hIOBJ obj; obj = (hIOBJ)lParam;

	//assuming that the vertex of this model has been set prior to this call
	//the same for indices
	static hIMDL mdl; mdl = (hIMDL)obj->GetModel();

	static CalRenderer *pCalRenderer; pCalRenderer = obj->GetCalModel()->getRenderer();

	pCalRenderer->selectMeshSubmesh(meshID, meshSubID);

	f32 *pVerts, *pTangents, *pBinorms, cV[3];
	CalIndex *pInds;

	u32 numMap = pCalRenderer->getMapCount();

	static GFXVTXBUFF *vtxBuff; vtxBuff = mdl->GetVtxBuff();
	static GFXINDBUFF *indBuff; indBuff = mdl->GetIndBuff();

	if(VtxLock(vtxBuff, 0, 0, pCalRenderer->getVertexCount()*VtxGetStride(vtxBuff, 0), (void**)&pVerts, D3DLOCK_DISCARD))
	{
		if(pCalRenderer->getVerticesNormalsAndTexCoords(pVerts) == -1) //this shouldn't happen!
		{ LogMsg(LOG_FILE, L"OBJFXCB", L"Error filling vertex/normals/texture coords"); }

		VtxUnlock(vtxBuff, 0);
	}

	//fill up tangent/binormal stuff
	if(pCalRenderer->isTangentsEnabled(0))
	{
		if(TESTFLAGS(mdl->GetFlag(), MODEL_TANGENT))
		{
			if(VtxLock(vtxBuff, 1, 0, pCalRenderer->getVertexCount()*VtxGetStride(vtxBuff, 1), (void**)&pTangents, D3DLOCK_DISCARD))
			{
				pCalRenderer->getTangentSpaces(0, pTangents);
				
				if(TESTFLAGS(mdl->GetFlag(), MODEL_BINORMAL))
				{
					if(VtxLock(vtxBuff, 2, 0, pCalRenderer->getVertexCount()*VtxGetStride(vtxBuff, 2), (void**)&pBinorms, D3DLOCK_DISCARD))
					{
						pCalRenderer->getNormals(pBinorms);
						
						for(u32 i = 0; i < pCalRenderer->getVertexCount(); i++)
						{
							cV[0] = pBinorms[1]*pTangents[2] - pBinorms[2]*pTangents[1];
							cV[1] = pBinorms[2]*pTangents[0] - pBinorms[0]*pTangents[2];
							cV[2] = pBinorms[0]*pTangents[1] - pBinorms[1]*pTangents[0];

							pBinorms[0] = cV[0];
							pBinorms[1] = cV[1];
							pBinorms[2] = cV[2];
							pBinorms[3] = 0;
							
							pBinorms += 4;
							pTangents += 4;
						}

						VtxUnlock(vtxBuff, 2);
					}
				}

				VtxUnlock(vtxBuff, 1);
			}
		}
	}

	if(IndLock(indBuff, 0, pCalRenderer->getFaceCount()*3*sizeof(CalIndex), (void**)&pInds, D3DLOCK_DISCARD))
	{
		int numInd = pCalRenderer->getFaces(pInds);
		IndUnlock(indBuff);
	}

	//set textures
	hTXT txt;

	if(numMap > 0)
	{
		for(u32 i = 0; i < numMap; i++)
		{
			txt = (hTXT)pCalRenderer->getMapUserData(i);

			if(txt)
				effect->SetBaseTexture(i, txt);
		}
	}
	else
		effect->SetBaseTexture(0, 0);
}

void MDLXShadowCallback(void *usrDat, u32 usrParam, u8 bLock, gfxShadowInfo *pSIOut)
{
	gfxMesh *pMesh = (gfxMesh *)usrDat;

	if(pMesh)
	{
		if(bLock)
		{
			pMesh->shMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pSIOut->vOut);
			pMesh->shMesh->LockIndexBuffer(D3DLOCK_READONLY, (void**)&pSIOut->iOut);

			pSIOut->numVtx = pMesh->shMesh->GetNumVertices();
			pSIOut->numFace = pMesh->shMesh->GetNumFaces();
		}
		else
		{
			pMesh->shMesh->UnlockVertexBuffer();
			pMesh->shMesh->UnlockIndexBuffer();
		}
	}
}

void MDLCal3DShadowCallback(void *usrDat, u32 usrParam, u8 bLock, gfxShadowInfo *pSIOut)
{
	if(!bLock) return;

	hIOBJ obj = (hIOBJ)usrDat;

	// get the renderer of the model
	CalRenderer *pCalRenderer;
	pCalRenderer = obj->GetCalModel()->getRenderer();
	gfxCal3D *pSH = (gfxCal3D *)((hIMDL)obj->GetModel())->GetCalCoreModel()->getUserData();

	// begin the rendering loop
	if(pCalRenderer->beginRendering())
	{
		u32 vCount = 0;
		u32 iCount = 0;
		u32 fCount = 0;

		CalIndex *pInds;

		//get sub meshes
		for(s32 meshSubID = 0; meshSubID < pCalRenderer->getSubmeshCount(usrParam); meshSubID++)
		{
			if(pCalRenderer->selectMeshSubmesh(usrParam, meshSubID))
			{
				pInds = (CalIndex *)&pSH->shadowInd[iCount];

				int b = pCalRenderer->getFaces(pInds);

				vCount += pCalRenderer->getVertices((f32*)&pSH->shadowVtx[vCount]);
				fCount += b;
				iCount += b*3;
			}
		}

		pSIOut->vOut = pSH->shadowVtx;
		pSIOut->iOut = (u16*)pSH->shadowInd;
		pSIOut->numFace = fCount;
		pSIOut->numVtx = vCount;

		pCalRenderer->endRendering();
	}
}

static inline void _BoundToRect(const Vec3D & bMin, const Vec3D & bMax, iRect & rect)
{
	//the 8 corners in image space
	Vec3D c[8];

	c[0] = bMin;
	c[1].x = bMin.x; c[1].y = bMin.y; c[1].z = bMax.z;
	c[2].x = bMax.x; c[2].y = bMin.y; c[2].z = bMax.z;
	c[3].x = bMax.x; c[3].y = bMin.y; c[3].z = bMin.z;
	c[4] = bMax;
	c[5].x = bMin.x; c[5].y = bMax.y; c[5].z = bMax.z;
	c[6].x = bMin.x; c[6].y = bMax.y; c[6].z = bMin.z;
	c[7].x = bMax.x; c[7].y = bMax.y; c[7].z = bMin.z;

	GFXProjectPt(&c[0], &c[0]);
	GFXProjectPt(&c[1], &c[1]);
	GFXProjectPt(&c[2], &c[2]);
	GFXProjectPt(&c[3], &c[3]);
	GFXProjectPt(&c[4], &c[4]);
	GFXProjectPt(&c[5], &c[5]);
	GFXProjectPt(&c[6], &c[6]);
	GFXProjectPt(&c[7], &c[7]);

	rect.left = (s32)c[0].x;
	rect.right = (s32)c[0].x;
	rect.top = (s32)c[0].y;
	rect.bottom = (s32)c[0].y;

	for(u8 i = 1; i < 8; i++)
	{
		if((s32)c[i].x < rect.left)
			rect.left = (s32)c[i].x;
		else if((s32)c[i].x > rect.right)
			rect.right = (s32)c[i].x;

		if((s32)c[i].y < rect.top)
			rect.top = (s32)c[i].y;
		else if((s32)c[i].y > rect.bottom)
			rect.bottom = (s32)c[i].y;
	}
}

static gfxCal3D *pSH=0;

void IgfxObject::_RenderMDL(hEFFECT fxOverride, f32 lod_lvl, SceneGeometry & g)
{
	static gfxMaterial mat;
	g.pMat = &mat;

	FLAGS sceneFlag = g.flag;

	if(TESTFLAGS(m_flag, OBJECT_NORECSHADOW))
		sceneFlag |= SCENE_NOSHADOW;

	//for Cal3D
	if(m_pCalModel)
	{
		//global geometry settings...
		g.primType = GFXPT_TRIANGLELIST;
		g.lParam = (s32)this;
		g.pVtx = m_mdl->GetVtxBuff();
		g.pInd = m_mdl->GetIndBuff();
		g.fxCB = OBJFXCB;

		//set Cal3D LOD level
		if(m_mdl->GetLODDist() > 0)
			m_pCalModel->setLodLevel(1.0f - (lod_lvl/m_mdl->GetLODDist()));

		pSH = (gfxCal3D *)m_mdl->GetCalCoreModel()->getUserData();

		u32 matID;

		// get the renderer of the model
		CalRenderer *pCalRenderer;
		pCalRenderer = m_pCalModel->getRenderer();

		// begin the rendering loop
		if(!pCalRenderer->beginRendering()) return;

		//render all the meshes
		for(s32 meshID = 0; meshID < pCalRenderer->getMeshCount(); meshID++)
		{
			//skip the shadow one...
			if(pSH && pSH->shadowID >= 0 && meshID == pSH->shadowID)
				continue;

			//get sub meshes
			for(s32 meshSubID = 0; meshSubID < pCalRenderer->getSubmeshCount(meshID); meshSubID++)
			{
				if(pCalRenderer->selectMeshSubmesh(meshID, meshSubID))
				{
					matID = pCalRenderer->m_pSelectedSubmesh->getCoreMaterialId();

					//////////////////////////////////////////////////////
					// get the core material, so we can get the FX stuff
					g.fx = (hIEFFECT)(fxOverride ? fxOverride : GetEffect(matID));

					//just use the default FX
					if(!g.fx) g.fx = (hIEFFECT)m_mdl->GetEffectDflt();
					//////////////////////////////////////////////////////

					g.tech = ((hIEFFECT)g.fx)->GetTechLOD(lod_lvl);

					GetMaterial(matID, &mat);

					if(mat.Diffuse.w == 0)
						mat.Diffuse.w = m_alpha;

					g.numVtx = (u32)pCalRenderer->getVertexCount();
					g.numFaces = (u32)pCalRenderer->getFaceCount();

					g.wParam = (meshSubID&0x0000ffff) | ((meshID&0x0000ffff)<<16);

					g.flag = sceneFlag;

					//we want to force FVF on submeshes with no bumpmap,
					//saves us a lot of trouble
					if(!pCalRenderer->isTangentsEnabled(0))
						g.flag |= SCENE_FORCEFVF;

					GFXSceneAdd(g);
				}
			}
		}

		pCalRenderer->endRendering();
	}
	//for mesh
	else
	{
		static gfxMesh *pMesh; pMesh = m_mdl->GetXMesh();

		if(!pMesh) return;

		g.numFaces = 0;

		g.pMesh = m_mdl->XGetLOD(lod_lvl);

		g_numTri += g.pMesh->GetNumFaces();

		g.fxCB = OBJFXMeshCB;

		for(g.meshSub = 0; g.meshSub < pMesh->numSubset; g.meshSub++)
		{
			g.fx = (hIEFFECT)(fxOverride ? fxOverride : GetEffect(g.meshSub));

			//just use the default FX
			if(!g.fx) g.fx = (hIEFFECT)m_mdl->GetEffectDflt();

			g.tech = ((hIEFFECT)g.fx)->GetTechLOD(lod_lvl);

			GetMaterial(g.meshSub, &mat);

			if(mat.Diffuse.w == 0)
				mat.Diffuse.w = m_alpha;

			g.wParam = (u32)GetTxt(g.meshSub);

			GFXSceneAdd(g);
		}
	}
}

/////////////////////////////////////
// Purpose:	add object to scene,
//			to be rendered.
//			OPTIONAL (can all be set to 0):
//			fxOverride: use this
//			to override all effects
//			of the model.
//			fxCBUserDat:
//			this is passed to effect's
//			callback for setting
//			parameters.
// Output:	scene updated
// Return:	TRUE if success
/////////////////////////////////////
u32 IgfxObject::Display(hEFFECT fxOverride, void *fxCBUserDat)
{
	//create the world matrix
	_ComputeWrldMtx();

	///////////////////////////////////////////
	//are we attached to a bone from parent?
	if(m_parent && m_parentBoneID >= 0)
	{
		Vec3D bTrans; m_parent->BoneGetTransAbs(m_parentBoneID, &bTrans);
		Quaternion bQuat; m_parent->BoneGetRotateAbs(m_parentBoneID, &bQuat);

		Matrix bMtx; MtxQuaternion(&bQuat, &bMtx); bMtx._41 = bTrans.x; bMtx._42 = bTrans.y; bMtx._43 = bTrans.z;
		m_wrldMtx[FXMTX_NORMAL] *= bMtx;//*wrldMtx;
	}
	//////////////////////////////////////////

	//////////////////////////////////////////
	//push world stack and set this object's world mtx
	g_pWrldStack->Push();
	g_pWrldStack->MultMatrixLocal((D3DXMATRIX*)&m_wrldMtx[FXMTX_NORMAL]);

	//do transformations
	m_wrldMtx[FXMTX_NORMAL] = *((Matrix *)g_pWrldStack->GetTop());

	//this is for the Effects parameters
	MtxInverse(&m_wrldMtx[FXMTX_NORMAL], &m_wrldMtx[FXMTX_INVERSE]);
	MtxTranspose(&m_wrldMtx[FXMTX_NORMAL], &m_wrldMtx[FXMTX_TRANSPOSE]);
	MtxTranspose(&m_wrldMtx[FXMTX_INVERSE], &m_wrldMtx[FXMTX_INVTRANSPOSE]);
	
	//////////////////////////////////////////

	///////////////////////////////////////////
	//Get the world bounding box
	Vec3D bMin,bMax, sMin,sMax;
	GetAABB(&bMin, &bMax);

	//set the AABB to world
	Vec3DTransformCoord(&bMin, &m_wrldMtx[FXMTX_NORMAL], &sMin);
	Vec3DTransformCoord(&bMax, &m_wrldMtx[FXMTX_NORMAL], &sMax);

	if(sMin.x < sMax.x) { bMin.x = sMin.x; bMax.x = sMax.x; }
	else { bMax.x = sMin.x; bMin.x = sMax.x; }

	if(sMin.y < sMax.y) { bMin.y = sMin.y; bMax.y = sMax.y; }
	else { bMax.y = sMin.y; bMin.y = sMax.y; }

	if(sMin.z < sMax.z) { bMin.z = sMin.z; bMax.z = sMax.z; }
	else { bMax.z = sMin.z; bMin.z = sMax.z; }
	///////////////////////////////////////////

	///////////////////////////////////////////
	//Get the lights near this object
	/*
	Vec3D(m_wrldMtx[FXMTX_NORMAL]._41,
		m_wrldMtx[FXMTX_NORMAL]._42,
		m_wrldMtx[FXMTX_NORMAL]._43)
	*/
	LightGetAllNearest(bMin, bMax, m_lights);

	//our shadow light
	hLIGHT shL = _LightGetShadow(m_lights);
	//////////////////////////////////////////

	//////////////////////////////////////////
	//check to see if visible through frustrum
	//also check to see if first light is inside the frustrum
	u8 bOBJCulled = !FrustrumCheckBox(bMin, bMax);
	u8 bLightCulled = FALSE;
	
	if(shL)
		bLightCulled = !FrustrumCheckPoint(shL->lDat.Position.x,
		shL->lDat.Position.y, shL->lDat.Position.z);
	//////////////////////////////////////////

	//////////////////////////////////////////
	//Calculate the LOD level
	Vec3D vCamObj(m_wrldMtx[FXMTX_NORMAL]._41-g_eyePt.x, 
		m_wrldMtx[FXMTX_NORMAL]._42-g_eyePt.y, 
		m_wrldMtx[FXMTX_NORMAL]._43-g_eyePt.z);

	f32 lod_lvl = Vec3DDot(&vCamObj,&vCamObj);
	//////////////////////////////////////////

	//do we want to display the object?
	if(!bOBJCulled)
	{ 
		SceneGeometry g = {0};

		g.camDist = lod_lvl;
		g.wrld = m_wrldMtx;
		g.pLights = m_lights;
		g.userDat = fxCBUserDat;

		_RenderMDL(fxOverride, lod_lvl, g);
	}

	//add shadowing
	if(shL && TESTFLAGS(m_flag, OBJECT_SHADOW))
	{
		if(m_mdl->GetShadDist() > 0 && lod_lvl/m_mdl->GetShadDist() >= 1.0f)
		{ bLightCulled = FALSE; bOBJCulled = TRUE; }

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
				//	&m_wrldMtx[FXMTX_INVERSE], &shadow.lPos);
				shadow.lPos = shL->lDat.Position;
				shadow.flag = 0;
			}

			if(bOBJCulled)
				SETFLAG(shadow.flag, SHADOW_OCC_CULLED);

			//calculate the scissor area rect in 2d
#ifdef SHADOW_SCISSOR
			if(g_d3dcaps.RasterCaps & D3DPRASTERCAPS_SCISSORTEST)
			{
				//2d bound of object
				iRect objRect;

				//boundary of shadow
				Vec3D	boundMin, boundMax;

				//get the image space bound of the light
				_BoundToRect(
					Vec3D(shadow.lPos.x-m_lights[0]->lDat.Range,
					      shadow.lPos.y-m_lights[0]->lDat.Range,
						  shadow.lPos.z-m_lights[0]->lDat.Range), 
					Vec3D(shadow.lPos.x+m_lights[0]->lDat.Range,
					      shadow.lPos.y+m_lights[0]->lDat.Range,
						  shadow.lPos.z+m_lights[0]->lDat.Range), shadow.scissorR);

				//get the image space bound of the object
				_BoundToRect(bMin, bMax, objRect);

				//put the light to image space
				Vec3D lS;
				GFXProjectPt(&shadow.lPos, &lS);

				//now we shrink the shadow bound if we have to
				if(lS.x < objRect.left) shadow.scissorR.left = objRect.left;
				else if(lS.x > objRect.right) shadow.scissorR.right = objRect.right;

				if(lS.y < objRect.top) shadow.scissorR.top = objRect.top;
				else if(lS.y > objRect.bottom) shadow.scissorR.bottom = objRect.bottom;
			}
#endif
			shadow.pWrldMtx = &m_wrldMtx[FXMTX_NORMAL];
			shadow.extrude = SHADOW_EXTRUDE;//shL->lDat.Range;//

			static gfxMesh *pMesh; pMesh = m_mdl->GetXMesh();
			static CalCoreModel *pCalCoreModel; pCalCoreModel = m_mdl->GetCalCoreModel();

			if(pMesh && pMesh->shMesh)
			{
				shadow.cb = MDLXShadowCallback;
				shadow.usrDat = pMesh;

				GFXShadowAdd(&shadow);
			}
			//cal3d?
			else if(pCalCoreModel && pSH)
			{
				// get the renderer of the model
				CalRenderer *pCalRenderer;
				pCalRenderer = m_pCalModel->getRenderer();

				// begin the rendering loop
				if(pCalRenderer->beginRendering())
				{
					shadow.usrDat = this;

					if(pSH->shadowID >= 0)
					{
						//only one mesh
						shadow.cb = MDLCal3DShadowCallback;	
						shadow.usrParam = pSH->shadowID;

						GFXShadowAdd(&shadow);
					}
					else
					{
						//send all the meshes to process shadow
						for(s32 meshID = 0; meshID < pCalRenderer->getMeshCount(); meshID++)
						{
							shadow.cb = MDLCal3DShadowCallback;	
							shadow.usrParam = meshID;

							GFXShadowAdd(&shadow);
						}
					}

					pCalRenderer->endRendering();
				}
			}
		}
	}

	//display children
	for(list<hOBJ>::iterator cObj = m_objNode.begin(); cObj != m_objNode.end(); cObj++)
		(*cObj)->Display(fxOverride, fxCBUserDat);

	//take this junk out!
	g_pWrldStack->Pop();
	
	return TRUE;
}

/////////////////////////////////////
// Purpose:	add object to scene,
//			to be rendered.
//			OPTIONAL (can all be set to 0):
//			fxOverride: use this
//			to override all effects
//			of the model.
//			fxCBUserDat:
//			this is passed to effect's
//			callback for setting
//			parameters.
//			SKYBOX version
//			call this between: 
//			SkyBoxBegin()
//			and SkyBoxEnd()
// Output:	scene updated
// Return:	TRUE if success
/////////////////////////////////////
u32 IgfxObject::DisplaySkyBox(hEFFECT fxOverride, void *fxCBUserDat)
{
	gfxCal3D *pSH=0;

	//create the world matrix
	_ComputeWrldMtx();

	///////////////////////////////////////////
	//are we attached to a bone from parent?
	if(m_parent && m_parentBoneID >= 0)
	{
		Vec3D bTrans; m_parent->BoneGetTransAbs(m_parentBoneID, &bTrans);
		Quaternion bQuat; m_parent->BoneGetRotateAbs(m_parentBoneID, &bQuat);

		Matrix bMtx; MtxQuaternion(&bQuat, &bMtx); bMtx._41 = bTrans.x; bMtx._42 = bTrans.y; bMtx._43 = bTrans.z;
		m_wrldMtx[FXMTX_NORMAL] *= bMtx;//*wrldMtx;
	}
	//////////////////////////////////////////

	//////////////////////////////////////////
	//push world stack and set this object's world mtx
	g_pWrldStack->Push();
	g_pWrldStack->MultMatrixLocal((D3DXMATRIX*)&m_wrldMtx[FXMTX_NORMAL]);

	//do transformations
	m_wrldMtx[FXMTX_NORMAL] = *((Matrix *)g_pWrldStack->GetTop());

	//this is for the Effects parameters
	MtxInverse(&m_wrldMtx[FXMTX_NORMAL], &m_wrldMtx[FXMTX_INVERSE]);
	MtxTranspose(&m_wrldMtx[FXMTX_NORMAL], &m_wrldMtx[FXMTX_TRANSPOSE]);
	MtxTranspose(&m_wrldMtx[FXMTX_INVERSE], &m_wrldMtx[FXMTX_INVTRANSPOSE]);
	
	//////////////////////////////////////////
	//check to see if visible through frustrum
	//also check to see if first light is inside the frustrum
	Vec3D bMin,bMax, sMin,sMax;
	GetAABB(&bMin, &bMax);

	//set the AABB to world
	Vec3DTransformCoord(&bMin, &m_wrldMtx[FXMTX_NORMAL], &sMin);
	Vec3DTransformCoord(&bMax, &m_wrldMtx[FXMTX_NORMAL], &sMax);

	if(sMin.x < sMax.x) { bMin.x = sMin.x; bMax.x = sMax.x; }
	else { bMax.x = sMin.x; bMin.x = sMax.x; }

	if(sMin.y < sMax.y) { bMin.y = sMin.y; bMax.y = sMax.y; }
	else { bMax.y = sMin.y; bMin.y = sMax.y; }

	if(sMin.z < sMax.z) { bMin.z = sMin.z; bMax.z = sMax.z; }
	else { bMax.z = sMin.z; bMin.z = sMax.z; }

	u8 bOBJCulled = !FrustrumCheckBox(bMin, bMax);
	//////////////////////////////////////////

	//do we want to display the object?
	if(!bOBJCulled)
	{ 
		SceneGeometry g = {0};

		g.wrld = m_wrldMtx;
		g.pLights = m_lights;
		g.userDat = fxCBUserDat;
		g.flag = SCENE_SKYBOX;

		_RenderMDL(fxOverride, 0, g);
	}

	//display children
	for(list<hOBJ>::iterator cObj = m_objNode.begin(); cObj != m_objNode.end(); cObj++)
		(*cObj)->DisplaySkyBox(fxOverride, fxCBUserDat);

	//take this junk out!
	g_pWrldStack->Pop();
	
	return TRUE;
}