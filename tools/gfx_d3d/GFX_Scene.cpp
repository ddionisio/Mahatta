#include "gfx_i.h"

typedef enum {
	SCENE_SKYBOXES,		//the skybox realm
	SCENE_NORMAL,		//the normal scene geometries, each receives shadows
	SCENE_NOSHADOWS,	//does not receive shadows
	SCENE_TRANSPARENT,	//transparent objects will not receive shadows and is sorted differently

	SCENE_MAX
} SCENE_TYPE;

typedef struct _sceneDat {
	SceneGeometry	g;			//the scene geometry from user

	//our stuff:
	s32				type;		//sorted from lowest to highest value

	Matrix			wrldS;		//the world matrix to set (if wrld is NULL, we use this)

	gfxMaterial		mat;		//material to set
} sceneDat;

sceneDat *g_sceneBuff=0;	//the scenes to apply
u32		  g_sceneSize;		//size of scene buffer
u32		  g_sceneNum;		//number of scenes in scenebuff

u32		  g_sceneTNum[SCENE_MAX]; //number of scene based on type

gfxMaterial g_DefMat;

Matrix g_defMtx[FXMTX_MAX];

/////////////////////////////////////
// Name:	GFXSceneDestroyBuffer
// Purpose:	destroy the scene buffer
// Output:	scene buffer destroyed
// Return:	none
/////////////////////////////////////
void GFXSceneDestroyBuffer()
{
	if(g_sceneBuff) { MemFree(g_sceneBuff); g_sceneBuff = 0; }
}

/////////////////////////////////////
// Name:	GFXSceneSetBufferSize
// Purpose:	set the scene buffer size
// Output:	scene buffer set
// Return:	TRUE if successful
/////////////////////////////////////
u32 GFXSceneSetBufferSize(u32 size)
{
	GFXSceneDestroyBuffer();

	u8 i;

	g_sceneBuff = (sceneDat *)MemAlloc(sizeof(sceneDat)*size);

	if(!g_sceneBuff)
	{ LogMsg(LOG_FILE, L"GFXSceneSetBufferSize", L"Failed to allocate scene buffer"); return FALSE; }

	memset(g_sceneTNum, 0, sizeof(g_sceneTNum));

	g_sceneNum = 0;
	g_sceneSize = size;

	//set the default material
	memset(&g_DefMat, 0, sizeof(g_DefMat));
	g_DefMat.Diffuse.x = g_DefMat.Diffuse.y = g_DefMat.Diffuse.z = g_DefMat.Diffuse.w = 1;
	g_DefMat.Ambient.x = g_DefMat.Ambient.y = g_DefMat.Ambient.z = g_DefMat.Ambient.w = 1;

	//set default matrices
	for(i = 0; i < FXMTX_MAX; i++)
		MtxIdentity(&g_defMtx[i]);

	return TRUE;
}

static u8 _GFXSceneExpandBuffer(u32 num)
{
	g_sceneBuff = (sceneDat *)realloc(g_sceneBuff, sizeof(sceneDat)*(g_sceneSize+num));

	if(g_sceneBuff)
	{
		g_sceneSize += num;
		return TRUE;
	}

	return FALSE;
}

/////////////////////////////////////
// Name:	GFXSceneAdd
// Purpose:	append a scene to render
//			(pWrldMtx must be an array 
//			of size FXMTX_MAX)
//			flags are: SCENE_*
// Output:	scene buffer updated
// Return:	TRUE if success
/////////////////////////////////////
u32 GFXSceneAdd(const SceneGeometry & geo)
{
	u32 ind, ret=TRUE;
	SCENE_TYPE type = SCENE_NORMAL;

	if(geo.pMat && geo.pMat->Diffuse.w < 1.0f || (geo.fx && TESTFLAGS(geo.fx->GetFlag(), EFFECT_TRANSPARENT)))
		type = SCENE_TRANSPARENT;
	else if(TESTFLAGS(geo.flag, SCENE_NOSHADOW))
		type = SCENE_NOSHADOWS;
	else if(TESTFLAGS(geo.flag, SCENE_SKYBOX))
		type = SCENE_SKYBOXES;

	//set index and increment total scene object
	ind = g_sceneNum;
	g_sceneNum++;

	//set relative number based on type
	g_sceneTNum[type]++;

	//in case we ran out of buffer
	if(g_sceneNum > g_sceneSize)
	{
		if(!_GFXSceneExpandBuffer(1))
		{ LogMsg(LOG_FILE, L"GFXSceneAdd", L"Ran out of Memory"); return FALSE; }
	}

	g_sceneBuff[ind].type = type;
	g_sceneBuff[ind].g = geo;

	//did user provide FX technique? if not, then get the default from hEFFECT
	if(g_sceneBuff[ind].g.fx && g_sceneBuff[ind].g.tech == 0)
		g_sceneBuff[ind].g.tech = g_sceneBuff[ind].g.fx->GetDfltTechnique();

	//did user not provide an array of matrix?
	if(!g_sceneBuff[ind].g.wrld)
		g_sceneBuff[ind].g.wrld = g_defMtx;
	
	//if user provided a matrix pointer and requests it to be a single matrix,
	//set our single matrix data and set pointer to 0
	if(TESTFLAGS(g_sceneBuff[ind].g.flag, SCENE_SINGLEMTX) && g_sceneBuff[ind].g.wrld)
	{
		g_sceneBuff[ind].wrldS = *g_sceneBuff[ind].g.wrld;
		g_sceneBuff[ind].g.wrld = 0;
	}

	//did user provide material, otherwise use default
	if(g_sceneBuff[ind].g.pMat)
	{ g_sceneBuff[ind].mat = *g_sceneBuff[ind].g.pMat; g_sceneBuff[ind].g.pMat = 0; }
	else
		g_sceneBuff[ind].mat = g_DefMat;

	g_numTri += g_sceneBuff[ind].g.numFaces;

	return TRUE;
}

/////////////////////////////////////
// Name:	GFXSceneGetNumTri
// Purpose:	get the number of triangles
//			that will be rendered, or
//			has rendered.  Call this
//			before GFXSceneClear().
// Output:	none
// Return:	number of tris.
/////////////////////////////////////
u32 F_API GFXSceneGetNumTri()
{
	return g_numTri;
}

/////////////////////////////////////
// Name:	GFXSceneClear
// Purpose:	clear the scene
// Output:	scene cleared
// Return:	none
/////////////////////////////////////
void F_API GFXSceneClear()
{
	g_sceneNum = 0;

	memset(g_sceneTNum, 0, sizeof(g_sceneTNum));

	//clear out the occluders as well
	GFXShadowClear();

	g_numTri = 0;
}

/*
< 0 elem1 less than elem2 
0 elem1 equivalent to elem2 
> 0 elem1 greater than elem2 
*/

int sceneCompare(const void *itm1, const void *itm2)
{
	sceneDat *pS1 = (sceneDat *)itm1, *pS2 = (sceneDat *)itm2;

	if(pS1->type == pS2->type)
	{
		//back-to-front order for transparent geometries
		if(TESTFLAGS(pS1->g.fx->GetFlag(), EFFECT_TRANSPARENT))
		{
			if(pS1->g.camDist > pS2->g.camDist) //things farther away should be rendered first
				return -1;
			else if(pS1->g.camDist < pS2->g.camDist) //vice versa
				return 1;
		}

		if(pS1->g.fx == pS2->g.fx && pS1->g.tech == pS2->g.tech)
		{
			if(pS1->g.pVtx == pS2->g.pVtx
				&& TESTFLAGS(pS1->g.flag, SCENE_FORCEFVF) == TESTFLAGS(pS2->g.flag, SCENE_FORCEFVF))
			{
				if(pS1->g.pInd == pS2->g.pInd)
					return 0;
			}
		}
	}
	else if(pS1->type > pS2->type)
		return 1;

	return -1;
}

static inline void _GFXScenePrepareFX(const sceneDat & scn, u32 *numPass)
{
	if(scn.g.fx)
	{
		if(scn.g.tech)
			scn.g.fx->GetFX()->SetTechnique(scn.g.tech);

		scn.g.fx->SetGlobalParams();

		scn.g.fx->GetFX()->Begin(numPass, 0);
	}
}

static inline void _GFXSceneUnprepareFX(const sceneDat & scn)
{
	if(scn.g.fx)
	{
		scn.g.fx->GetFX()->End();
	}
}

static inline void _GFXScenePrepareVtx(const sceneDat & scn)
{
	if(scn.g.pVtx)
	{
		if(TESTFLAGS(scn.g.flag, SCENE_FORCEFVF))
			VtxSetFVF(scn.g.pVtx);
		else
			VtxSet(scn.g.pVtx);
	}
	else if(scn.g.pMesh)
		g_p3DDevice->SetFVF(scn.g.pMesh->GetFVF());
}

static inline void _GFXSceneUnprepareVtx(const sceneDat & scn)
{
	if(scn.g.pVtx)
		VtxUnset(scn.g.pVtx);
}

static inline void _GFXScenePrepareInd(const sceneDat & scn)
{
	if(scn.g.pInd)
		g_p3DDevice->SetIndices(scn.g.pInd);
}

static inline void _GFXSceneDraw(const sceneDat & scn)
{
	if(scn.g.pMesh)
	{
		scn.g.pMesh->DrawSubset(scn.g.meshSub);
	}
	else if(scn.g.pInd)
		g_p3DDevice->DrawIndexedPrimitive((D3DPRIMITIVETYPE)scn.g.primType, scn.g.startVtx, 0, 
		scn.g.numVtx, scn.g.startInd, scn.g.numFaces);
	else
		g_p3DDevice->DrawPrimitive((D3DPRIMITIVETYPE)scn.g.primType, scn.g.startVtx, scn.g.numFaces);
}

static inline void _GFXSceneRender(const sceneDat & scn, u32 numPass)
{	
	if(scn.g.fx)
	{
		if(scn.g.wrld)
			scn.g.fx->SetParams(scn.g.wrld, FALSE, scn.mat, scn.g.pLights, scn.g.userDat, 
			scn.g.fxCB, scn.g.wParam, scn.g.lParam);
		else
			scn.g.fx->SetParams(&scn.wrldS, TRUE, scn.mat, scn.g.pLights, scn.g.userDat, 
			scn.g.fxCB, scn.g.wParam, scn.g.lParam);
		
		for(u32 i = 0; i < numPass; i++)
		{
			scn.g.fx->GetFX()->Pass( i );

			_GFXSceneDraw(scn);
		}
	}
	else
	{
		g_p3DDevice->SetMaterial((D3DMATERIAL9*)&scn.mat);
		g_p3DDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&scn.g.wrld);

		if(scn.g.fxCB)
			scn.g.fxCB(0, scn.g.wParam, scn.g.lParam);

		_GFXSceneDraw(scn);
	}
}

static inline void _SceneRender(u32 start, u32 end)
{
	u32 numPass=0;

	//do the first one
	_GFXScenePrepareFX(g_sceneBuff[start], &numPass);
	_GFXScenePrepareVtx(g_sceneBuff[start]);
	_GFXScenePrepareInd(g_sceneBuff[start]);
	LightEnableList(g_sceneBuff[start].g.pLights);
	_GFXSceneRender(g_sceneBuff[start], numPass);

	for(u32 i = start+1; i <= end; i++)
	{
		if(g_sceneBuff[i-1].g.fx != g_sceneBuff[i].g.fx 
			|| g_sceneBuff[i-1].g.tech != g_sceneBuff[i].g.tech)
		{ _GFXSceneUnprepareFX(g_sceneBuff[i-1]); _GFXScenePrepareFX(g_sceneBuff[i], &numPass); }

		if(g_sceneBuff[i-1].g.pVtx != g_sceneBuff[i].g.pVtx
			|| TESTFLAGS(g_sceneBuff[i-1].g.flag, SCENE_FORCEFVF) != TESTFLAGS(g_sceneBuff[i].g.flag, SCENE_FORCEFVF))
		{ _GFXSceneUnprepareVtx(g_sceneBuff[i-1]); _GFXScenePrepareVtx(g_sceneBuff[i]); }

		if(g_sceneBuff[i-1].g.pInd != g_sceneBuff[i].g.pInd)
			_GFXScenePrepareInd(g_sceneBuff[i]);

		if(g_sceneBuff[i-1].g.pLights != g_sceneBuff[i].g.pLights)
		{ LightDisableList(); LightEnableList(g_sceneBuff[i].g.pLights); }

		_GFXSceneRender(g_sceneBuff[i], numPass);
	}

	//end for last
	_GFXSceneUnprepareFX(g_sceneBuff[end]);
	_GFXSceneUnprepareVtx(g_sceneBuff[end]);
	LightDisableList();
	g_p3DDevice->SetIndices(0);
}

//render scene with given FX override
static inline void _GFXSceneRenderOvride(hIEFFECT fx, const sceneDat & scn, u32 pass)
{
	if(scn.g.wrld)
		fx->SetParams(scn.g.wrld, FALSE, scn.mat, scn.g.pLights, scn.g.userDat, scn.g.fxCB, 
		scn.g.wParam, scn.g.lParam);
	else
		fx->SetParams(&scn.wrldS, TRUE, scn.mat, scn.g.pLights, scn.g.userDat, scn.g.fxCB, 
		scn.g.wParam, scn.g.lParam);

	fx->GetFX()->Pass( pass );

	_GFXSceneDraw(scn);
}

//scene render with FX override
static inline void _SceneRenderOvride(hIEFFECT fx, u32 start, u32 end, u32 pass)
{
	//do the first one
	_GFXScenePrepareVtx(g_sceneBuff[start]);
	_GFXScenePrepareInd(g_sceneBuff[start]);
	LightEnableList(g_sceneBuff[start].g.pLights);
	_GFXSceneRenderOvride(fx, g_sceneBuff[start], pass);

	for(u32 i = start+1; i <= end; i++)
	{
		if(g_sceneBuff[i-1].g.pVtx != g_sceneBuff[i].g.pVtx)
		{ _GFXSceneUnprepareVtx(g_sceneBuff[i-1]); _GFXScenePrepareVtx(g_sceneBuff[i]); }

		if(g_sceneBuff[i-1].g.pInd != g_sceneBuff[i].g.pInd)
			_GFXScenePrepareInd(g_sceneBuff[i]);

		if(g_sceneBuff[i-1].g.pLights != g_sceneBuff[i].g.pLights)
		{ LightDisableList(); LightEnableList(g_sceneBuff[i].g.pLights); }

		_GFXSceneRenderOvride(fx, g_sceneBuff[i], pass);
	}

	//end for last
	_GFXSceneUnprepareVtx(g_sceneBuff[end]);
	LightDisableList();
	g_p3DDevice->SetIndices(0);
}

inline void SceneSkyBoxRender()
{
	//////////////////////////////////////////////////
	//temporary view matrix
	Vec3D tmpLoc(g_mtxView[FXMTX_NORMAL]._41, g_mtxView[FXMTX_NORMAL]._42, g_mtxView[FXMTX_NORMAL]._43);

	g_mtxView[FXMTX_NORMAL]._41 = 0; 
	g_mtxView[FXMTX_NORMAL]._42 = 0; 
	g_mtxView[FXMTX_NORMAL]._43 = 0;

	//set the one in d3d as well
	g_p3DDevice->SetTransform(D3DTS_VIEW, (const D3DMATRIX*)&g_mtxView[FXMTX_NORMAL]);

	GFXSetViewProjMtxFX();
	//////////////////////////////////////////////////

	//don't use lights
	u8 bSkyboxTmpIsLight = TESTFLAGS(g_FLAGS, GFX_LIGHTENABLE);
	LightEnableRender(FALSE);

	g_p3DDevice->SetRenderState( D3DRS_ZWRITEENABLE,  FALSE );
	//g_p3DDevice->SetRenderState( D3DRS_ZENABLE,  D3DZB_FALSE  );

	//render the skybox geometries
	if(g_sceneTNum[SCENE_SKYBOXES] > 0)
		_SceneRender(0, g_sceneTNum[SCENE_SKYBOXES]-1);

	//////////////////////////////////////////////////
	//reset the global view matrix
	g_mtxView[FXMTX_NORMAL]._41 = tmpLoc.x; 
	g_mtxView[FXMTX_NORMAL]._42 = tmpLoc.y; 
	g_mtxView[FXMTX_NORMAL]._43 = tmpLoc.z;
	GFXSetViewProjMtxFX();

	//reset the view matrix on d3d
	g_p3DDevice->SetTransform(D3DTS_VIEW, (const D3DMATRIX*)&g_mtxView[FXMTX_NORMAL]);
	//////////////////////////////////////////////////

	LightEnableRender(bSkyboxTmpIsLight);

	g_p3DDevice->SetRenderState( D3DRS_ZWRITEENABLE,  TRUE );
	///g_p3DDevice->SetRenderState( D3DRS_ZENABLE,  D3DZB_TRUE   );
}

//just sort the scene, no rendering
void _GFXSceneSort()
{
	qsort(g_sceneBuff, g_sceneNum, sizeof(sceneDat), sceneCompare);
}

//render the scene with no sorting
void _GFXSceneRenderNoSort()
{
	SceneSkyBoxRender();

	u32 ofs;

	//render the normal geometries
	if(g_sceneTNum[SCENE_NORMAL] > 0)
	{
		ofs = g_sceneTNum[SCENE_NORMAL-1];
		_SceneRender(ofs, ofs+(g_sceneTNum[SCENE_NORMAL]-1));
	}

	//render shadows
	if(GFXShadowExecute())
		GFXShadowRenderShade();

	//render the geometries that will not receive shadows
	//and render the transparent geometries
	ofs = (g_sceneTNum[SCENE_NOSHADOWS]+g_sceneTNum[SCENE_TRANSPARENT]);
	if(ofs > 0)
		_SceneRender(g_sceneNum-ofs, g_sceneNum-1);

	//render lens flares

	//////////////////////////////////////
	//per-pixel lighting here
}

/////////////////////////////////////
// Name:	GFXSceneRender
// Purpose:	render the scene with
//			provided models and
//			what-not
// Output:	scene rendered
// Return:	TRUE if success
/////////////////////////////////////
u32 F_API GFXSceneRender()
{
	qsort(g_sceneBuff, g_sceneNum, sizeof(sceneDat), sceneCompare);

	_GFXSceneRenderNoSort();

	return TRUE;
}

//render the scene with FX override and no sorting
void _GFXSceneRenderNoSortFXOvride(hIEFFECT fx, u32 pass)
{
	SceneSkyBoxRender();

	u32 ofs;

	//render the normal geometries
	if(g_sceneTNum[SCENE_NORMAL] > 0)
	{
		ofs = g_sceneTNum[SCENE_NORMAL-1];
		_SceneRenderOvride(fx, ofs, ofs+(g_sceneTNum[SCENE_NORMAL]-1), pass);
	}

	//render shadows
	if(GFXShadowExecute())
		GFXShadowRenderShade();

	//render the geometries that will not receive shadows
	//and render the transparent geometries
	ofs = (g_sceneTNum[SCENE_NOSHADOWS]+g_sceneTNum[SCENE_TRANSPARENT]);
	if(ofs > 0)
		_SceneRenderOvride(fx, g_sceneNum-ofs, g_sceneNum-1, pass);

	//render lens flares
}