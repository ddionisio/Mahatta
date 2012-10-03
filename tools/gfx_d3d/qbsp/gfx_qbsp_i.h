#ifndef _gfx_qbsp_i_h
#define _gfx_qbsp_i_h

//internal use of QBSP stuff

//globals
extern u8			 g_nullrow[MAX_NULLROW];

extern Vec3D		 g_traceExt;	//trace extension
extern const Vec3D  *g_p;

extern s32					g_areabytes;
extern u8					g_areabits[MAX_MAP_AREAS/8];		// portalarea visibility bits

extern s32					g_clientarea;
extern s32					g_cluster;
extern s32					g_leafnum;

extern s32					g_fragmentFrame;

#define	DIST_EPSILON	0.03125f	//stuff

#define	PLANE_NORMAL_EPSILON	0.00001f
#define	PLANE_DIST_EPSILON		0.01f
#define ON_EPSILON				0.1f

//used for FX variables
#define QBSP_BASE_TXT		L"BaseTex"
#define QBSP_SHADOW_TXT		L"ShadowTex"
#define QBSP_MDL_TECH		L"Model"

#define VIS_SET(vis, i) ((vis)[(i) >> 5] |= (1 << ((i) & 31)))
#define VIS_IS_ON(vis, i) ((vis)[(i) >> 5] & (1 << ((i) & 31 )))
#define VIS_CLEAR(vis, i) ((vis)[(i) >> 5] &= ~(1 << ((i) & 31)))

inline void QBSPVisBitClearAll(u32 *vis, u32 size)
{
	memset(vis, 0, sizeof(u32) * size);
}

u32 * QBSPVisBitCreate(u32 count, u32 & sOut);

//effect stuff
void QBSPFXCB(hIEFFECT effect, u32 wParam, s32 lParam);

/*
=================
Patch
=================
*/

/*
===============
Patch_GetFlatness
===============
*/
void PatchGetFlatness ( f32 maxflat, const gfxVtxQBSP *points, s32 *patch_cp, s32 *flat);

/*
===============
Patch_Evaluate
===============
*/
void PatchEvaluate(const gfxVtxQBSP *p, s32 *numcp, s32 *tess, gfxVtxQBSP *dest);

//create a patch from 9 control points of given vtx and its offset index
//as well as the given subdivision level
gfxPatch *QBSPPatchCreate(const gfxVtxQBSP *vtx, s32 offset, s32 patch_cp[2], f32 level);

void QBSPPatchDestroy(gfxPatch *patch);

s32 QBSPPatchRender(const gfxPatch *patch, SceneGeometry &g);

#endif