#include "..//gfx_i.h"

#include "gfx_qbsp.h"
#include "gfx_qbsp_i.h"

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
IgfxQBSP::~IgfxQBSP()
{
	u32 i;

	GFXDestroyVtxBuff(m_vtx);

	if(m_faces)
	{
		//destroy the patches
		for(i = 0; i < m_numFaces; i++)
			QBSPPatchDestroy(m_faces[i].patch);
		
		MemFree(m_faces);
	}

	if(m_textures)
	{
		//release all textures
		for(i = 0; i < m_numTxt; i++)
		{
			SAFE_RELEASE(m_textures[i].fx);
			SAFE_RELEASE(m_textures[i].texture);
		}

		MemFree(m_textures);
	}

	if(m_lightmaps)
	{
		//release all textures
		for(u32 i = 0; i < m_numLightMap; i++)
			SAFE_RELEASE(m_lightmaps[i]);

		MemFree(m_lightmaps);
	}

	if(m_nodes)
		MemFree(m_nodes);

	if(m_leaves)
	{
		MemFree(m_leaves);
	}

	if(m_leafFaces)
		MemFree(m_leafFaces);

	if(m_planes)
		MemFree(m_planes);

	if(m_clusters.pBitsets)
		MemFree(m_clusters.pBitsets);

	if(m_clustersPHS.pBitsets)
		MemFree(m_clustersPHS.pBitsets);

	SAFE_RELEASE(m_meshIndBuff);
	
	if(m_models)
	{
		for(u32 i = 1; i < m_numModels; i++)
		{
			if(m_models[i].brushes)
			{
				for(s32 j = 0; j < m_models[i].numOfBrushes; j++)
				{
					if(m_models[i].brushes[j].planes)
						MemFree(m_models[i].brushes[j].planes);

					if(m_models[i].brushes[j].tPlanes)
						MemFree(m_models[i].brushes[j].tPlanes);

					if(m_models[i].brushes[j].txtID)
						MemFree(m_models[i].brushes[j].txtID);
				}

				MemFree(m_models[i].brushes);
			}
		}

		MemFree(m_models);
	}

	if(m_brushes)
		MemFree(m_brushes);

	if(m_brushVis)
		MemFree(m_brushVis);

	if(m_leafBrushes)
		MemFree(m_leafBrushes);

	if(m_brushSides)
		MemFree(m_brushSides);

	//destroy default fxs
	SAFE_RELEASE(m_dfltFX);

	//destroy decals
	ClearDecals();

	//destroy shadow informations
	if(m_shVtxBuff)
		MemFree(m_shVtxBuff);

	if(m_shIndBuff)
		MemFree(m_shIndBuff);
}

s32 IgfxQBSP::Release()
{
	if(m_ref == 0)
		delete this;
	else
	{ m_ref--; return m_ref; }

	return 0;
}