#include "..//gfx_i.h"

#include "gfx_qbsp.h"
#include "gfx_qbsp_i.h"

void IgfxQBSP::_RenderFace(s32 i, const Matrix *pWrldMtx, FLAGS flag,
						   GFXHANDLE fxTech, const hLIGHT *pLights,
						   f32 lodDistSq)
{
	if(m_faces[i].fragmentframe != g_fragmentFrame)// !VIS_IS_ON(m_faceVis, i))
	{
		if(!TESTFLAGS(m_textures[m_faces[i].textureID].flags, SURF_NODRAW))
		{
			SceneGeometry g={0};

			g.camDist = lodDistSq;
			g.fx = (hIEFFECT)m_textures[m_faces[i].textureID].fx;
			g.tech = fxTech ? fxTech : ((hIEFFECT)m_textures[m_faces[i].textureID].fx)->GetTechLOD(lodDistSq);
			g.wrld = (Matrix*)pWrldMtx;
			g.flag = flag;
			g.pLights = pLights;
			g.fxCB = QBSPFXCB;
			g.wParam = (u32)this;
			g.lParam = i;

			/*if(m_textures[m_faces[i].textureID].texture)
				{
					GFXSceneAdd(dfltFX, 0, m_vtx, 0, 
						m_faces[i].startVertIndex, 0, GFXPT_TRIANGLEFAN, 
						m_faces[i].numOfVerts, m_faces[i].numOfVerts-2, 
						0, pWrldMtx, flag, pLights, dfltFXCB, (u32)qbsp, i, 0);
				}*/

			switch(m_faces[i].type)
			{
			case eFaceMesh:
			case eFacePolygon:
				g.pVtx = m_vtx; 
				g.pInd = m_meshIndBuff; 
				g.startVtx = m_faces[i].startVertIndex; 
				g.startInd = m_faces[i].meshVertIndex;
				g.primType = GFXPT_TRIANGLELIST;
				g.numVtx = m_faces[i].numMeshVerts;
				g.numFaces = m_faces[i].numMeshVerts/3;

				GFXSceneAdd(g);
				break;

			case eFacePatch:
				if(m_faces[i].patch)
				{
					QBSPPatchRender(m_faces[i].patch, g);
				}
				break;

			case eFaceBillboard:
				{
					if(m_textures[m_faces[i].textureID].texture)
					{
						gfxVtxQBSP *pVtx;//=m_vtx;

						if(VtxLock(m_vtx, 0, 0,0, (void**)&pVtx, D3DLOCK_READONLY))
						{
							m_textures[m_faces[i].textureID].texture->BltBillboard(pVtx[m_faces[i].startVertIndex].pt, 1, 0, 0);

							VtxUnlock(m_vtx, 0);
						}
					}
				}
				break;
			}
		}

		//VIS_SET(m_faceVis, i);
		m_faces[i].fragmentframe = g_fragmentFrame;
	}
}

void IgfxQBSP::_RenderLeaf(const Vec3D &pos, gfxBSPLeaf *pLeaf, u8 bCheckFrustum, const Matrix *pWrldMtx)
{
	// If we get here, the leaf we are testing must be visible in our camera's view.
	// Get the number of faces that this leaf is in charge of.
	s32 faceCount = pLeaf->numOfLeafFaces;
	s32 faceIndex;

	//Vec3D vCamObj((pLeaf->loc.x+pWrldMtx->_41)-pos.x, (pLeaf->loc.y+pWrldMtx->_42)-pos.y, (pLeaf->loc.z+pWrldMtx->_43)-pos.z);
	f32 lod_lvl = CameraGetBoundFarDistSq(pLeaf->min, pLeaf->max, 0);//Vec3DDot(&vCamObj,&vCamObj);

	// Loop through and render all of the faces in this leaf
	while(faceCount--)
	{
		// Grab the current face index from our leaf faces array
		faceIndex = m_leafFaces[pLeaf->leafface + faceCount];

		_RenderFace(faceIndex, pWrldMtx, SCENE_SINGLEMTX, 0, 0, lod_lvl);
	}
}

void IgfxQBSP::_RecursiveRender(s32 node, const Vec3D &pos, u8 bCheckFrustum)
{
	s32 ind=0;
	s32 side = 1;
	f32 distance = 0.0f;

	gfxBSPLeaf *pLeaf;

	if(node < 0)
	{
		ind = ~node;
		
		pLeaf = &m_leaves[ind];

		// Check at PVS as well
		// check for door connected areas
		// If the current leaf is not in the camera's frustum, go to the next leaf
		if(!ClusterIsVisible(g_cluster, pLeaf->cluster)
			|| !(g_areabits[pLeaf->area>>3] & (1<<(pLeaf->area&7)))
			|| (bCheckFrustum && !FrustrumCheckBox(pLeaf->min, pLeaf->max)))
		{
			return;
		}

		//render stuff
		_RenderLeaf(pos, pLeaf, bCheckFrustum, &m_wrldMtx);

		return;
	}
	else
		ind = node;

	if(bCheckFrustum)
	{
		switch(FrustrumCheckBoxEX(m_nodes[ind].min, m_nodes[ind].max))
		{
		case BBOX_IN:
			bCheckFrustum = FALSE;
			break;
		case BBOX_OUT:
			return;
		}
	}

	//find where we are, front or back
	s32 pI = m_nodes[ind].plane;
	distance = PlaneDiff(&pos, &m_planes[pI]); /*m_planes[pI].a*pos.x + m_planes[pI].b*pos.y
			      +m_planes[pI].c*pos.z - m_planes[pI].d;*/

	if(distance >= 0)
		side = 0;

	//recurse down from front
	_RecursiveRender(m_nodes[ind].child[side], pos, bCheckFrustum);

	//recurse down from back
	_RecursiveRender(m_nodes[ind].child[!side], pos, bCheckFrustum);
}

/////////////////////////////////////
// Purpose:	render given qbsp with
//			camera position.
//			Things will be added
//			to the GFX scene.
//			Remember to call
//			QBSPPrepareFrame(...)
//			first
// Output:	scene added
// Return:	TRUE if success
/////////////////////////////////////
u32 IgfxQBSP::Render()
{
	//assume that the frustum is calculated!
	g_fragmentFrame++;

	////////////////////////////////////////////////////////////////////
	// render the static polygons

	_RecursiveRender(0, g_eyePt, TRUE);
	////////////////////////////////////////////////////////////////////

	//Render all decals
	RenderDecals();

	return TRUE;
}