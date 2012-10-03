#include "gfx_i.h"

#include "GFX\gfx_math.h"

//Shadow Volume Interface

#define SHADOWVTXFLAG (GFXFVF_XYZ)

#define SHADOW_VTXBUFF_USAGE	(GFXUSAGE_DYNAMIC | GFXUSAGE_WRITEONLY)
#define SHADOW_VTXBUFF_POOL		GFXPOOL_DEFAULT

#define SHADOWQUADVTXFLAG (GFXFVF_XYZRHW | GFXFVF_DIFFUSE)

#define SHADOWQUAD_VTXBUFF_USAGE	GFXUSAGE_WRITEONLY
#define SHADOWQUAD_VTXBUFF_POOL		GFXPOOL_MANAGED

struct swQuadVtx {
	Vec4D	vtx;
	u32		clr;
};

//globals
GFXVTXBUFF		*g_shadowScnQuad;	//the shadow screen quad
u32				 g_shadeClr = 0x7f000000;

GFXVTXBUFF		*g_shadowVtx;		//the vertex pool for all shadows
u32				 g_maxVtx;

s16				*g_edges;			//the edge pool for all shadows
u32				 g_maxEdge;

ShadowDat		*g_shadows;			//the shadow pool
u32				 g_maxShadow;
u32				 g_numShadow;		//current number of shadows

void GFXShadowDestroyShade()
{
	GFXDestroyVtxBuff(g_shadowScnQuad);
	g_shadowScnQuad = 0;
}

//call this on graphics init and whenever we change screen size
u32 GFXShadowInitShade()
{
	if(TESTFLAGS(g_FLAGS, GFX_NOSTENCIL))
		return TRUE;

	GFXShadowDestroyShade();

	g_shadowScnQuad = GFXCreateVtxBuff(sizeof(swQuadVtx), 4, SHADOWQUAD_VTXBUFF_USAGE, 
		SHADOWQUADVTXFLAG, SHADOWQUAD_VTXBUFF_POOL);

	if(!g_shadowScnQuad)
	{ LogMsg(LOG_FILE, L"GFXShadowInitShade", L"Unable to create vertex buffer"); return FALSE; }

	f32 h = (f32)g_mode.height;
	f32 w = (f32)g_mode.width;

	swQuadVtx *pV;

	if(VtxLock(g_shadowScnQuad, 0, 0,0, (void**)&pV, 0))
	{
		pV[0].vtx = Vec4D(0.0f, h,    0.0f, 1.0f);
		pV[0].clr = g_shadeClr;

		pV[1].vtx = Vec4D(0.0f, 0.0f, 0.0f, 1.0f);
		pV[1].clr = g_shadeClr;

		pV[2].vtx = Vec4D(w,    h,    0.0f, 1.0f);
		pV[2].clr = g_shadeClr;

		pV[3].vtx = Vec4D(w,    0.0f, 0.0f, 1.0f);
		pV[3].clr = g_shadeClr;

		VtxUnlock(g_shadowScnQuad, 0);
	}

	return TRUE;
}

/////////////////////////////////////
// Name:	GFXShadowSetShade
// Purpose:	set the shadow shading
// Output:	shading color set
// Return:	TRUE if success
/////////////////////////////////////
u32 F_API GFXShadowSetShade(u32 color)
{
	g_shadeClr = color;

	if(g_shadowScnQuad)
	{
		swQuadVtx *pV;

		if(VtxLock(g_shadowScnQuad, 0, 0,0, (void**)&pV, 0))
		{
			pV[0].clr = g_shadeClr;
			pV[1].clr = g_shadeClr;
			pV[2].clr = g_shadeClr;
			pV[3].clr = g_shadeClr;

			VtxUnlock(g_shadowScnQuad, 0);

			return TRUE;
		}
	}

	return FALSE;
}

void GFXShadowDestroy()
{
	GFXDestroyVtxBuff(g_shadowVtx);

	if(g_edges) { MemFree(g_edges); g_edges = 0; }

	if(g_shadows) { MemFree(g_shadows); g_shadows = 0; }
}

/////////////////////////////////////
// Name:	GFXShadowInit
// Purpose:	this will create the shadow
//			pool data.  if each count 
//			is greater than the current, 
//			then we re-allocate
//			the pool to the new size
// Output:	shadow data initialized
// Return:	TRUE if success
/////////////////////////////////////
u32 GFXShadowInit(u32 vtxCount, u32 edgeCount, u32 shadowCount)
{
	if(TESTFLAGS(g_FLAGS, GFX_NOSTENCIL))
		return TRUE;

	//initialize vertex
	if(g_maxVtx < vtxCount)
	{
		GFXDestroyVtxBuff(g_shadowVtx);
		g_shadowVtx = GFXCreateVtxBuff(sizeof(Vec3D), vtxCount, SHADOW_VTXBUFF_USAGE, SHADOWVTXFLAG, SHADOW_VTXBUFF_POOL);

		if(!g_shadowVtx)
		{ LogMsg(LOG_FILE, L"GFXShadowInit", L"Unable to create vertex buffer"); return FALSE; }

		g_maxVtx = vtxCount;
	}

	//initialize edge
	if(g_maxEdge < edgeCount)
	{
		if(g_edges) MemFree(g_edges);

		g_edges = (s16*)MemAlloc(sizeof(s16)*edgeCount);

		if(!g_edges)
		{ LogMsg(LOG_FILE, L"GFXShadowInit", L"Unable to create edge buffer"); return FALSE; }

		g_maxEdge = edgeCount;
	}

	//initialize shadow data
	if(g_maxShadow < shadowCount)
	{
		if(g_shadows)
			g_shadows = (ShadowDat*)realloc(g_shadows, sizeof(ShadowDat)*shadowCount);
		else
			g_shadows = (ShadowDat*)MemAlloc(sizeof(ShadowDat)*shadowCount);

		if(!g_shadows)
		{ LogMsg(LOG_FILE, L"GFXShadowInit", L"Unable to create shadow data buffer"); return FALSE; }

		g_maxShadow = shadowCount;
	}

	return TRUE;
}

void GFXShadowInvalidate()
{
	//just release the vertex buffer
	GFXDestroyVtxBuff(g_shadowVtx);
}

void GFXShadowRestore()
{
	if(TESTFLAGS(g_FLAGS, GFX_NOSTENCIL))
		return;

	g_shadowVtx = GFXCreateVtxBuff(sizeof(Vec3D), g_maxVtx, SHADOW_VTXBUFF_USAGE, 
		SHADOWVTXFLAG, SHADOW_VTXBUFF_POOL);
}

/////////////////////////////////////
// Name:	GFXShadowAdd
// Purpose:	add an occluder to cast
//			shadow.
//			lPos is light in obj space
//			callback is for retrieving
//			vertex/index buffer when
//			building shadow vol.
//			for SHADOW_USE_DIR, 
//			we assume that the dir is 
//			normalized
// Output:	occluder added
// Return:	none
/////////////////////////////////////
void GFXShadowAdd(const ShadowDat *pShad)
{
	if(TESTFLAGS(g_FLAGS, GFX_NOSTENCIL))
		return;

	//expand the shadow occluder pool?
	if(g_numShadow == g_maxShadow)
		GFXShadowInit(0, 0, g_numShadow+1);

	g_shadows[g_numShadow] = *pShad;

	g_numShadow++;
}

/////////////////////////////////////
// Name:	GFXShadowClear
// Purpose:	clear out occluders
//			used only by GFXSceneClear()
// Output:	numShadow set to 0
// Return:	none
/////////////////////////////////////
void GFXShadowClear()
{
	g_numShadow = 0;
}

static inline void ShadowInsertEdge( s16* pEdges, u32 & dwNumEdges, s16 v0, s16 v1 )
{
	for (u32 i=0; i < dwNumEdges; i++)
	{
		if( ( pEdges[2*i+0] == v0 && pEdges[2*i+1] == v1 ) || 
			( pEdges[2*i+0] == v1 && pEdges[2*i+1] == v0 ) )
		{
			if( dwNumEdges > 1 )
			{
				pEdges[2*i+0] = pEdges[2*( dwNumEdges-1 )+0];
				pEdges[2*i+1] = pEdges[2*( dwNumEdges-1 )+1];	
			}
			dwNumEdges--;
			return;
		}
	}

	pEdges[2*dwNumEdges+0] = v0;
	pEdges[2*dwNumEdges+1] = v1;
	dwNumEdges++;
}

//build a shadow volume from given occluder
//this will set the shadow pool
//returns the number of vertices for the shadow volume
static u32 ShadowBuild(const ShadowDat & sh)
{
	u32 numSHVtx = 0;

	gfxShadowInfo si;

	//get the vertex data
	sh.cb(sh.usrDat, sh.usrParam, TRUE, &si);

	u32 dwNumEdges = 0;

	u16 wIndex0,wIndex1,wIndex2;

	Vec3D *pVtx;

	Vec3D v0,v1,v2,v3,v4;

	if(!VtxLock(g_shadowVtx, 0, 0,0, (void**)&pVtx, D3DLOCK_DISCARD))
		return 0;

	// For each face, check all 3 edges
	for(u32 i=0; i<si.numFace; i++)
	{
		wIndex0 = si.iOut[3*i+0];
		wIndex1 = si.iOut[3*i+1];
		wIndex2 = si.iOut[3*i+2];

		//FIXME!: prescale models so we don't have to do this
		Vec3DTransformCoord(&si.vOut[wIndex0], sh.pWrldMtx, &v0);
		Vec3DTransformCoord(&si.vOut[wIndex1], sh.pWrldMtx, &v1);
		Vec3DTransformCoord(&si.vOut[wIndex2], sh.pWrldMtx, &v2);

		//v0 = si.vOut[wIndex0];
		//v1 = si.vOut[wIndex1];
		//v2 = si.vOut[wIndex2];

		// Note that vLight had already been transformed to object space. This saves some computations work
		// Cosine value larger than 0.0 meant light facing since the angle between the light vector vLight
		// and the face normal is within -90 to 90 degrees
		// Face normal is computed in order to use welded models
		Vec3D vCrossValue1(v2-v1);
		Vec3D vCrossValue2(v1-v0);
		Vec3D vFaceNormal;
		Vec3DCross(&vCrossValue1, &vCrossValue2, &vFaceNormal);

		// Take note that we are doing a recalculation of vLightDir, or direction vector of incoming light ray
		// by using the first vertex of a face (3 vertices) to represent that face. The dot product test is also
		// only done once per face.
		Vec3D vLightDir = TESTFLAGS(sh.flag, SHADOW_USE_DIR) ? sh.lPos : v0 - sh.lPos;  // Direction vector of incoming light rays
		if( Vec3DDot( &vFaceNormal, &vLightDir ) >= 0.0f )
		{
			// Front cap using occluder's front facing tri
			// Add cappping by reusing object's triangles
			// if occluder is culled, we don't need this
			if(!TESTFLAGS(sh.flag, SHADOW_OCC_CULLED))
			{
				pVtx[numSHVtx++] = v0;
				pVtx[numSHVtx++] = v1;
				pVtx[numSHVtx++] = v2;
			}

			ShadowInsertEdge( g_edges, dwNumEdges, wIndex0, wIndex1 );
			ShadowInsertEdge( g_edges, dwNumEdges, wIndex1, wIndex2 );
			ShadowInsertEdge( g_edges, dwNumEdges, wIndex2, wIndex0 );
		}
	}

	// Using a tri fan to form the back cap
	Vec3D vExtrusionDir; // Direction vector from light to vertex, or rather the direction to extrude

	Vec3D vTriFanStart;// = si.vOut[g_edges[(dwNumEdges-1)*2+1]];  // Use last vertex as origin for back capping: tri fan

	Vec3DTransformCoord(&si.vOut[g_edges[(dwNumEdges-1)*2+1]], sh.pWrldMtx, &vTriFanStart);
	
	if(TESTFLAGS(sh.flag, SHADOW_USE_DIR))
		vExtrusionDir = sh.lPos;
	else
	{ vExtrusionDir = vTriFanStart - sh.lPos; Vec3DNormal(&vExtrusionDir, &vExtrusionDir); }

	vTriFanStart.x = vTriFanStart.x + vExtrusionDir.x * sh.extrude;
	vTriFanStart.y = vTriFanStart.y + vExtrusionDir.y * sh.extrude;
	vTriFanStart.z = vTriFanStart.z + vExtrusionDir.z * sh.extrude;


	// For each silhouette edge, duplicate it, 
	for( i=0; i<dwNumEdges; i++ )
	{
		v1 = si.vOut[g_edges[2*i+0]];
		v2 = si.vOut[g_edges[2*i+1]];

		Vec3DTransformCoord(&si.vOut[g_edges[2*i+0]], sh.pWrldMtx, &v1);
		Vec3DTransformCoord(&si.vOut[g_edges[2*i+1]], sh.pWrldMtx, &v2);

		// Extrusion can be tricky. It is not advisable to extrude vertices by a multiple of the magnitude of 
		// vExtrusionDir. This is because the magnitude may be so small that even a large multiple would not extrude
		// the vertices far enough. Results will be unpredictable if either light source of occluders are dynamic 
		// objects. Hence, we normalize the vExtrusionDir vector before multiply by the ABSOLUTE distance
		// that we want to extrude the vertex to.

		if(TESTFLAGS(sh.flag, SHADOW_USE_DIR))
			vExtrusionDir = sh.lPos;
		else
		{
			vExtrusionDir = v1 - sh.lPos;  // Compute a new extrusion direction for new vertex
			Vec3DNormal(&vExtrusionDir, &vExtrusionDir);
		}

		v3.x = v1.x + vExtrusionDir.x * sh.extrude;
		v3.y = v1.y + vExtrusionDir.y * sh.extrude;
		v3.z = v1.z + vExtrusionDir.z * sh.extrude;

		if(TESTFLAGS(sh.flag, SHADOW_USE_DIR))
			vExtrusionDir = sh.lPos;
		else
		{
			vExtrusionDir = v2 - sh.lPos;  // Compute a new extrusion direction for new vertex
			Vec3DNormal( &vExtrusionDir, &vExtrusionDir );
		}

		v4.x = v2.x + vExtrusionDir.x * sh.extrude;
		v4.y = v2.y + vExtrusionDir.y * sh.extrude;
		v4.z = v2.z + vExtrusionDir.z * sh.extrude;

		// Add a quad (two triangles) to the vertex list
		pVtx[numSHVtx++] = v1;
		pVtx[numSHVtx++] = v4;
		pVtx[numSHVtx++] = v2;

		pVtx[numSHVtx++] = v1;
		pVtx[numSHVtx++] = v3;
		pVtx[numSHVtx++] = v4;

		// Add a triangle to back cap tri fan, make sure it faces outwards at well!
		pVtx[numSHVtx++] = vTriFanStart;
		pVtx[numSHVtx++] = v4;
		pVtx[numSHVtx++] = v3;
	}

	//done with vertex data
	sh.cb(sh.usrDat, sh.usrParam, FALSE, 0);

	VtxUnlock(g_shadowVtx, 0);

	//numSHVtx
	//dwNumEdges
	assert(g_maxVtx > numSHVtx);
	assert(g_maxEdge > dwNumEdges);

	return numSHVtx;
}

//render the pool, numVtx is generated from ShadowBuild
static inline void ShadowRender(u32 numVtx)
{
	//u32 numF = numVtx/3;

	//g_numTri += numF;

	g_p3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, numVtx/3);
}

//build all shadows from occluders and render them to stencil
//this assumes that depth buffer has been filled already by the scene
//so this is done after scene is rendered
u32 GFXShadowExecute()
{
	if(TESTFLAGS(g_FLAGS, GFX_NOSTENCIL) || g_numShadow == 0)
		return FALSE;
	else if(!TESTFLAGS(g_FLAGS, GFX_UPDATESTENCIL))
		return TRUE;

	u32 numVtx;

	// Disable z-buffer writes, z-testing still occurs, enable stencil-buffer
	g_p3DDevice->SetRenderState( D3DRS_ZWRITEENABLE,  FALSE );
	g_p3DDevice->SetRenderState( D3DRS_STENCILENABLE, TRUE );

	// (StencilRef & StencilMask) CompFunc (StencilBufferValue & StencilMask)
	g_p3DDevice->SetRenderState( D3DRS_STENCILREF,       0x1 );
	g_p3DDevice->SetRenderState( D3DRS_STENCILMASK,      0xffffffff );
	g_p3DDevice->SetRenderState( D3DRS_STENCILWRITEMASK, 0xffffffff );

#ifdef SHADOW_SCISSOR
	g_p3DDevice->SetRenderState( D3DRS_SCISSORTESTENABLE,      TRUE );
#endif

#ifndef SHOW_FACE
	g_p3DDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0x00000000 );
#endif

	// Set a slight depth bias value for shadow volumes
	g_p3DDevice->SetRenderState( D3DRS_DEPTHBIAS, GFX_F2DW( 0.000001f));

	g_p3DDevice->SetVertexShader( NULL );
	VtxSetFVF(g_shadowVtx);

	//set world matrix
	//FIXME!: once we do prescaling, world transformation of occluder is used
	Matrix mtxIden; MtxIdentity(&mtxIden);
	g_p3DDevice->SetTransform( D3DTS_WORLD, (D3DXMATRIX*)&mtxIden);

	//check to see if we can do TWOSIDED stencil
	if(g_d3dcaps.StencilCaps & D3DSTENCILCAPS_TWOSIDED)
	{
		g_p3DDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_NONE );

		g_p3DDevice->SetRenderState( D3DRS_TWOSIDEDSTENCILMODE, TRUE);

		g_p3DDevice->SetRenderState( D3DRS_STENCILFUNC,      D3DCMP_ALWAYS );
		g_p3DDevice->SetRenderState( D3DRS_STENCILPASS,      D3DSTENCILOP_KEEP );
		g_p3DDevice->SetRenderState( D3DRS_STENCILFAIL,      D3DSTENCILOP_KEEP );

		g_p3DDevice->SetRenderState( D3DRS_CCW_STENCILFUNC,      D3DCMP_ALWAYS );
		g_p3DDevice->SetRenderState( D3DRS_CCW_STENCILPASS,      D3DSTENCILOP_KEEP );
		g_p3DDevice->SetRenderState( D3DRS_CCW_STENCILFAIL,      D3DSTENCILOP_KEEP );

		g_p3DDevice->SetRenderState( D3DRS_STENCILZFAIL,     D3DSTENCILOP_DECR );

		g_p3DDevice->SetRenderState( D3DRS_CCW_STENCILZFAIL,     D3DSTENCILOP_INCR );

		for(u32 i = 0; i < g_numShadow; i++)
		{
			numVtx = ShadowBuild(g_shadows[i]);

			/////////////////////////////////////////////////////////////////////
			//set the scissor test
#ifdef SHADOW_SCISSOR
			g_p3DDevice->SetScissorRect((RECT*)&g_shadows[i].scissorR);
#endif

#ifdef SHOW_FACE
			D3DMATERIAL9 mtrl;
			memset( &mtrl, 0, sizeof( D3DMATERIAL9 ) );
			mtrl.Ambient = D3DXCOLOR( 0.0f, 1.0f, 1.0f, 0.0f );
			g_p3DDevice->SetMaterial( &mtrl );
			g_p3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCCOLOR );
			g_p3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_DESTALPHA  );
#endif
			/////////////////////////////////////////////////////////////////////
			//render
			ShadowRender(numVtx);
			/////////////////////////////////////////////////////////////////////
		}

		g_p3DDevice->SetRenderState( D3DRS_TWOSIDEDSTENCILMODE, FALSE);
	}
	else
	{
		g_p3DDevice->SetRenderState( D3DRS_STENCILFUNC,      D3DCMP_ALWAYS );
		g_p3DDevice->SetRenderState( D3DRS_STENCILPASS,      D3DSTENCILOP_KEEP );
		g_p3DDevice->SetRenderState( D3DRS_STENCILFAIL,      D3DSTENCILOP_KEEP );

		for(u32 i = 0; i < g_numShadow; i++)
		{
			numVtx = ShadowBuild(g_shadows[i]);

			/////////////////////////////////////////////////////////////////////
			//set the scissor test
#ifdef SHADOW_SCISSOR
			g_p3DDevice->SetScissorRect((RECT*)&g_shadows[i].scissorR);
#endif

			/////////////////////////////////////////////////////////////////////
			//back-face render

			// Back face depth test fail -> Incr
			g_p3DDevice->SetRenderState( D3DRS_STENCILZFAIL,     D3DSTENCILOP_INCR );

			// Now reverse cull order so back sides of shadow volume are written.
			g_p3DDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CW );

#ifdef SHOW_FACE
			D3DMATERIAL9 mtrl;
			memset( &mtrl, 0, sizeof( D3DMATERIAL9 ) );
			mtrl.Ambient = D3DXCOLOR( 0.0f, 1.0f, 1.0f, 0.0f );
			g_p3DDevice->SetMaterial( &mtrl );
			g_p3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCCOLOR );
			g_p3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_DESTALPHA  );
#endif

			ShadowRender(numVtx);
			/////////////////////////////////////////////////////////////////////
		
			/////////////////////////////////////////////////////////////////////
			//front-face render

			// Now reverse cull order so front sides of shadow volume are written.
			g_p3DDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
			// Reverse the stencil op for front face
			g_p3DDevice->SetRenderState( D3DRS_STENCILZFAIL,     D3DSTENCILOP_DECR );

#ifdef SHOW_FACE
			memset( &mtrl, 0, sizeof( D3DMATERIAL9 ) );
			mtrl.Ambient = D3DXCOLOR( 1.0f, 1.0f, 0.0f, 0.0f );
			g_p3DDevice->SetMaterial( &mtrl );
			g_p3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCCOLOR );
			g_p3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_DESTALPHA  );
#endif

			ShadowRender(numVtx);
			/////////////////////////////////////////////////////////////////////
		}
	}

	VtxUnset(g_shadowVtx);

	// Restore render states
	g_p3DDevice->SetRenderState( D3DRS_DEPTHBIAS, GFX_F2DW( 0.0f ) );
	g_p3DDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0x0000000F ); 
	g_p3DDevice->SetRenderState( D3DRS_ZWRITEENABLE,     TRUE );

#ifdef SHADOW_SCISSOR
	g_p3DDevice->SetRenderState( D3DRS_SCISSORTESTENABLE,      FALSE );
#endif

	//g_p3DDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );

	return TRUE;
}

//darken areas that are in shadow, this is after stencil has been updated
//so you call this anytime after GFXShadowExecute, so long as stencil data
//is not changed
void GFXShadowRenderShade()
{
	if(!TESTFLAGS(g_FLAGS, GFX_UPDATESTENCIL))
	{
		g_p3DDevice->SetRenderState( D3DRS_STENCILENABLE, TRUE );
	}

	if(!g_shadowScnQuad)
		return;

#ifdef SHOW_FACE
	g_p3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	g_p3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
#endif

	g_p3DDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	//g_p3DDevice->SetRenderState( D3DRS_STENCILENABLE, TRUE );


	// Only write where stencil value greater or equal to 1
	//g_p3DDevice->SetRenderState( D3DRS_STENCILREF,  0x1 );
	g_p3DDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_LESSEQUAL );
	//g_p3DDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );

	// Draw a big, gray square
	VtxSetFVF(g_shadowScnQuad);
	g_p3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
	VtxUnset(g_shadowScnQuad);

	//set the stuff back to default
	g_p3DDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	g_p3DDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE );
}