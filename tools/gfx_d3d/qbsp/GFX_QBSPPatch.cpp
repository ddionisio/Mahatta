#include "..//gfx_i.h"

#include "GFX\gfx_math.h"

#include "gfx_qbsp.h"
#include "gfx_qbsp_i.h"

#define PATCH_VTXBUFF_USAGE		GFXUSAGE_WRITEONLY//D3DUSAGE_DYNAMIC//D3DUSAGE_SOFTWAREPROCESSING//D3DUSAGE_DYNAMIC//
#define PATCH_VTXBUFF_POOL		GFXPOOL_MANAGED//D3DPOOL_DEFAULT//D3DPOOL_DEFAULT//

#define PATCH_INDBUFF_USAGE		GFXUSAGE_WRITEONLY
#define PATCH_INDBUFF_POOL		GFXPOOL_MANAGED

//FIXME!: Tri-Strip this bitch!

/*
===============
Patch_FlatnessTest
===============
*/
static s32 _PatchFlatnessTest(f32 maxflat, const Vec3D & point0, const Vec3D & point1, 
							  const Vec3D & point2)
{
	Vec3D v1, v2, v3;
	Vec3D t, n;
	f32 dist, d, l;
	s32 ft0, ft1;

	n = point2-point0;
	l = Vec3DNormal(&n,&n);

	if ( !l ) {
		return 0;
	}

	t = point1-point0;
	d = -Vec3DDot(&t,&n);
	t = t + d*n;
	dist = Vec3DLen(&t);

	if ( fabsf(dist) <= maxflat ) {
		return 0;
	}

	v1 = (point1+point0)*0.5f;
	v2 = (point2+point1)*0.5f;
	v3 = (v1+v2)*0.5f;

	ft0 = _PatchFlatnessTest ( maxflat, point0, v1, v3 );
	ft1 = _PatchFlatnessTest ( maxflat, v3, v2, point2 );

	return 1 + (s32)floorf( max ( ft0, ft1 ) + 0.5f );
}

/*
===============
Patch_GetFlatness
===============
*/
void PatchGetFlatness ( f32 maxflat, const gfxVtxQBSP *points, s32 *patch_cp, s32 *flat )
{
	s32 i, p, u, v;

	flat[0] = flat[1] = 0;
	for (v = 0; v < patch_cp[1] - 1; v += 2)
	{
		for (u = 0; u < patch_cp[0] - 1; u += 2)
		{
			p = v * patch_cp[0] + u;

			i = _PatchFlatnessTest(maxflat, points[p].pt, points[p+1].pt, points[p+2].pt);
			flat[0] = max(flat[0], i);
			i = _PatchFlatnessTest(maxflat, points[p+patch_cp[0]].pt, points[p+patch_cp[0]+1].pt, points[p+patch_cp[0]+2].pt);
			flat[0] = max(flat[0], i);
			i = _PatchFlatnessTest(maxflat, points[p+2*patch_cp[0]].pt, points[p+2*patch_cp[0]+1].pt, points[p+2*patch_cp[0]+2].pt);
			flat[0] = max(flat[0], i);

			i = _PatchFlatnessTest(maxflat, points[p].pt, points[p+patch_cp[0]].pt, points[p+2*patch_cp[0]].pt);
			flat[1] = max(flat[1], i);
			i = _PatchFlatnessTest(maxflat, points[p+1].pt, points[p+patch_cp[0]+1].pt, points[p+2*patch_cp[0]+1].pt);
			flat[1] = max(flat[1], i);
			i = _PatchFlatnessTest(maxflat, points[p+2].pt, points[p+patch_cp[0]+2].pt, points[p+2*patch_cp[0]+2].pt);
			flat[1] = max(flat[1], i);
		}
	}
}

/*
vOut.pt = (v1.pt*(b*b)) + (v2.pt*(2*b*a)) + (v3.pt*(a*a));
	vOut.n  = (v1.n*(b*b)) + (v2.n*(2*b*a)) + (v3.n*(a*a));
	
	vOut.sTex  = (v1.sTex*(b*b)) + (v2.sTex*(2*b*a)) + (v3.sTex*(a*a));
	vOut.lTex  = (v1.lTex*(b*b)) + (v2.lTex*(2*b*a)) + (v3.lTex*(a*a));
	vOut.dlTex = (v1.dlTex*(b*b)) + (v2.dlTex*(2*b*a)) + (v3.dlTex*(a*a));

	vOut.color = 0xffffffff; //forget about this crap FIXME!
*/

/*
===============
Patch_Evaluate_QuadricBezier
===============
*/
static inline void _PatchEvaluateQuadricBezier(f32 t, 
					const gfxVtxQBSP & point0, const gfxVtxQBSP & point1, 
					const gfxVtxQBSP & point2, gfxVtxQBSP & out)
{
	f32 qt = t * t;
	f32 dt = 2.0f * t, tt;
	gfxVtxQBSP tvec4;

	tt = 1.0f - dt + qt;

	out.pt    = point0.pt*tt;
	out.n     = point0.n*tt;
	out.sTex  = point0.sTex*tt;
	out.lTex  = point0.lTex*tt;
	//out.dlTex = point0.dlTex*tt;

	tt = dt - 2.0f * qt;

	tvec4.pt    = point1.pt*tt;
	tvec4.n     = point1.n*tt;
	tvec4.sTex  = point1.sTex*tt;
	tvec4.lTex  = point1.lTex*tt;
	//tvec4.dlTex = point1.dlTex*tt;

	out.pt    += tvec4.pt;
	out.n     += tvec4.n;
	out.sTex  += tvec4.sTex;
	out.lTex  += tvec4.lTex;
	//out.dlTex += tvec4.dlTex;

	tvec4.pt    = point2.pt*qt;
	tvec4.n     = point2.n*qt;
	tvec4.sTex  = point2.sTex*qt;
	tvec4.lTex  = point2.lTex*qt;
	//tvec4.dlTex = point2.dlTex*qt;

	out.pt    += tvec4.pt;
	out.n     += tvec4.n;
	out.sTex  += tvec4.sTex;
	out.lTex  += tvec4.lTex;
	//out.dlTex += tvec4.dlTex;

	//out.color = 0xffffffff;
}

/*
===============
Patch_Evaluate
===============
*/
void PatchEvaluate(const gfxVtxQBSP *p, s32 *numcp, s32 *tess, gfxVtxQBSP *dest)
{
	s32 num_patches[2], num_tess[2];
	s32 index[3], dstpitch, i, u, v, x, y;
	f32 s, t, step[2];
	gfxVtxQBSP *tvec, pv[3][3], v1, v2, v3;

	num_patches[0] = numcp[0] / 2;
	num_patches[1] = numcp[1] / 2;
	dstpitch = num_patches[0] * tess[0] + 1;

	step[0] = 1.0f / (f32)tess[0];
	step[1] = 1.0f / (f32)tess[1];

	for ( v = 0; v < num_patches[1]; v++ )
	{
		// last patch has one more row 
		if ( v < num_patches[1] - 1 ) {
			num_tess[1] = tess[1];
		} else {
			num_tess[1] = tess[1] + 1;
		}

		for ( u = 0; u < num_patches[0]; u++ )
		{
			// last patch has one more column
			if ( u < num_patches[0] - 1 ) {
				num_tess[0] = tess[0];
			} else {
				num_tess[0] = tess[0] + 1;
			}

			index[0] = (v * numcp[0] + u) * 2;
			index[1] = index[0] + numcp[0];
			index[2] = index[1] + numcp[0];

			// current 3x3 patch control points
			for ( i = 0; i < 3; i++ ) 
			{
				pv[i][0] = p[index[0]+i];
				pv[i][1] = p[index[1]+i];
				pv[i][2] = p[index[2]+i];
			}
			
			t = 0.0f;
			tvec = dest + v * tess[1] * dstpitch + u * tess[0];

			for ( y = 0; y < num_tess[1]; y++, t += step[1] )
			{
				_PatchEvaluateQuadricBezier(t, pv[0][0], pv[0][1], pv[0][2], v1);
				_PatchEvaluateQuadricBezier(t, pv[1][0], pv[1][1], pv[1][2], v2);
				_PatchEvaluateQuadricBezier(t, pv[2][0], pv[2][1], pv[2][2], v3);

				s = 0.0f;
				for ( x = 0; x < num_tess[0]; x++, s += step[0] )
				{
					_PatchEvaluateQuadricBezier(s, v1, v2, v3, tvec[x]);
				}

				tvec += dstpitch;
			}
		}
	}
}

//create a patch from 9 control points of given vtx and its offset index
//as well as the given subdivision level
gfxPatch *QBSPPatchCreate(const gfxVtxQBSP *vtx, s32 offset, s32 patch_cp[2], f32 level)
{
	gfxPatch *newPatch = (gfxPatch *)MemAlloc(sizeof(gfxPatch));

	if(newPatch)
	{
		s32 numverts, step[2], flat[2], size[2];

		newPatch->level = level;

		// find the degree of subdivision in the u and v directions
		PatchGetFlatness(level, &vtx[offset], patch_cp, flat);

		// allocate space for mesh
		step[0] = (1 << flat[0]);
		step[1] = (1 << flat[1]);
		size[0] = (patch_cp[0] / 2) * step[0] + 1;
		size[1] = (patch_cp[1] / 2) * step[1] + 1;
		numverts = size[0] * size[1];

		newPatch->patchW = size[0];
		newPatch->patchH = size[1];

		//allocate vertices
		newPatch->vtx = GFXCreateVtxBuff(sizeof(gfxVtxQBSP), numverts, PATCH_VTXBUFF_USAGE, GFXVERTEXQBSPFLAG, PATCH_VTXBUFF_POOL);
		newPatch->numVtx = numverts;

		////////////////////////////////////////////////////////
		//fill up the vertices
		gfxVtxQBSP *vertex;

		if(VtxLock(newPatch->vtx, 0, 0, 0, (void**)&vertex, 0))
		{
			PatchEvaluate(&vtx[offset], patch_cp, step, vertex);

			//Get bounding box
			newPatch->max = newPatch->min = vertex[0].pt;

			for(u32 i = 1; i < numverts; i++)
			{
				if(vertex[i].pt.x < newPatch->min.x)
					newPatch->min.x = vertex[i].pt.x;
				else if(vertex[i].pt.x > newPatch->max.x)
					newPatch->max.x = vertex[i].pt.x;

				if(vertex[i].pt.y < newPatch->min.y)
					newPatch->min.y = vertex[i].pt.y;
				else if(vertex[i].pt.y > newPatch->max.y)
					newPatch->max.y = vertex[i].pt.y;

				if(vertex[i].pt.z < newPatch->min.z)
					newPatch->min.z = vertex[i].pt.z;
				else if(vertex[i].pt.z > newPatch->max.z)
					newPatch->max.z = vertex[i].pt.z;
			}

			VtxUnlock(newPatch->vtx, 0);
		}
		////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////
		//fill up the indices
		newPatch->numInd = (size[0]-1)*(size[1]-1)*6;

		GFXCreateIndBuff(sizeof(u16)*newPatch->numInd, PATCH_INDBUFF_USAGE, 16, 
			PATCH_INDBUFF_POOL, &newPatch->ind);

		// Compute the indices
		u16 *indexes;

		if(IndLock(newPatch->ind, 0, 0, (void **)&indexes, 0))
		{
			s32 v,i,u,p;

			for (v = 0, i = 0; v < size[1]-1; v++)
			{
				for (u = 0; u < size[0]-1; u++)
				{
					indexes[0] = p = v * size[0] + u;
					indexes[1] = p + size[0];
					indexes[2] = p + 1;
					indexes[3] = p + 1;
					indexes[4] = p + size[0];
					indexes[5] = p + size[0] + 1;
					indexes += 6;
				}
			}

			IndUnlock(newPatch->ind);
		}
		////////////////////////////////////////////////////////
	}

	return newPatch;
}

void QBSPPatchDestroy(gfxPatch *patch)
{
	if(patch)
	{
		GFXDestroyVtxBuff(patch->vtx);
		SAFE_RELEASE(patch->ind);

		if(patch->rowIndOfs)
			MemFree(patch->rowIndOfs);

		MemFree(patch);
	}
}

s32 QBSPPatchRender(const gfxPatch *patch, SceneGeometry &g)
{
	//GFXSceneAdd(fx, tech, patch->vtx, patch->ind, 0, 0, GFXPT_POINTLIST,
	//		patch->numVtx, patch->numVtx, pMtrl, pWrldMtx, flag, lights, fxCB, wParam, lParam, usrDat);

	g.camDist = CameraGetBoundFarDistSq(patch->min, patch->max, 0);
	g.pVtx = patch->vtx;
	g.pInd = patch->ind;
	g.primType = GFXPT_TRIANGLELIST;
	g.numVtx = patch->numVtx;
	g.numFaces = patch->numInd/3;

	GFXSceneAdd(g);
/*
	for(u32 row = 0; row < patch->numRow; row++)
	{
		//GFXSceneAdd(fx, tech, patch->vtx, patch->ind, 0, patch->rowIndOfs[row], GFXPT_LINESTRIP,
		//	patch->numVtx, patch->numTri, pMtrl, pWrldMtx, flag, lights, fxCB, wParam, lParam, usrDat);
		GFXSceneAdd(fx, tech, patch->vtx, patch->ind, 0, patch->rowIndOfs[row], GFXPT_TRIANGLESTRIP,
			patch->numVtx, patch->numTri, pMtrl, pWrldMtx, flag, lights, fxCB, wParam, lParam, usrDat);
	}
*/
	return TRUE;
}



#if 0
static inline void _PatchEval(const gfxVtxQBSP & v1,
							  const gfxVtxQBSP & v2,
							  const gfxVtxQBSP & v3,
							  f32	a,
							  f32	b,
							  gfxVtxQBSP & vOut)
{
	/*
					v1 * (b * b) + 
					v2 * (2 * b * a) +
					v3 * (a * a);
	*/
	vOut.pt = (v1.pt*(b*b)) + (v2.pt*(2*b*a)) + (v3.pt*(a*a));
	vOut.n  = (v1.n*(b*b)) + (v2.n*(2*b*a)) + (v3.n*(a*a));
	
	vOut.sTex  = (v1.sTex*(b*b)) + (v2.sTex*(2*b*a)) + (v3.sTex*(a*a));
	vOut.lTex  = (v1.lTex*(b*b)) + (v2.lTex*(2*b*a)) + (v3.lTex*(a*a));
	vOut.dlTex = (v1.dlTex*(b*b)) + (v2.dlTex*(2*b*a)) + (v3.dlTex*(a*a));

	vOut.color = 0xffffffff; //forget about this crap FIXME!
}

//create a patch from 9 control points of given vtx and its offset index
//as well as the given subdivision level
gfxPatch *QBSPPatchCreate(const gfxVtxQBSP *vtx, s32 offset, s32 level)
{
	gfxPatch *newPatch = (gfxPatch *)MemAlloc(sizeof(gfxPatch));

	if(newPatch)
	{
		newPatch->level = level;

		// The number of vertices along a side is 1 + num edges
		const s32 L1 = level + 1;

		//allocate vertices
		newPatch->vtx = GFXCreateVtxBuff(sizeof(gfxVtxQBSP), L1*L1, PATCH_VTXBUFF_USAGE, GFXVERTEXQBSPFLAG, PATCH_VTXBUFF_POOL);
		newPatch->numVtx = L1*L1;

		////////////////////////////////////////////////////////
		//fill up the vertices
		gfxVtxQBSP *controls = (gfxVtxQBSP *)&vtx[offset];
		gfxVtxQBSP *vertex;

		if(VtxLock(newPatch->vtx, 0, 0, 0, (void**)&vertex, 0))
		{
			f32 a,b;
			s32 i;

			//do first row
			for(i = 0; i <= level; ++i)
			{
				a = (f32)i / level;
				b = 1.0f - a;

				_PatchEval(controls[0], controls[3], controls[6], a, b, vertex[i]);
			}

			//do the rest
			for(i = 1; i <= level; ++i) {
				a = (f32)i / level;
				b = 1.0f - a;

				gfxVtxQBSP temp[3];

				s32 j;
				for(j = 0; j < 3; ++j) 
				{
					s32 k = 3 * j;

					_PatchEval(controls[k + 0], controls[k + 1], controls[k + 2], a, b, temp[j]);
				}

				for(j = 0; j <= level; ++j) 
				{
					a = (f32)j / level;
					b = 1.0f - a;

					_PatchEval(temp[0], temp[1], temp[2], a, b, vertex[i * L1 + j]);
				}
			}

			VtxUnlock(newPatch->vtx, 0);
		}
		////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////
		//fill up the indices
		newPatch->numInd = level * L1 * 2;

		GFXCreateIndBuff(sizeof(u16)*newPatch->numInd, PATCH_INDBUFF_USAGE, 16, 
			PATCH_INDBUFF_POOL, &newPatch->ind);

		// Compute the indices
		u16 *indexes;

		if(IndLock(newPatch->ind, 0, 0, (void **)&indexes, 0))
		{
			s32 row;

			for(row = 0; row < level; ++row) 
			{
				for(s32 col = 0; col <= level; ++col)	
				{
					indexes[(row * L1 + col) * 2 + 1] = row       * L1 + col;
					indexes[(row * L1 + col) * 2]     = (row + 1) * L1 + col;
				}
			}

			//get the rows
			newPatch->numTri = level * 2;

			newPatch->rowIndOfs = (u16*)MemAlloc(sizeof(u16)*level);
			newPatch->numRow = level;

			for(s32 row = 0; row < level; row++)
			{
				newPatch->rowIndOfs[row] = row*L1*2;//indexes[row * 2 * L1];
			}

			IndUnlock(newPatch->ind);
		}
	}

	return newPatch;
}
#endif