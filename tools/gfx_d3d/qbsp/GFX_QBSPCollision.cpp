#include "..//gfx_i.h"

#include "GFX\gfx_math.h"

#include "gfx_qbsp.h"
#include "gfx_qbsp_i.h"

Vec3D		 g_traceExt;	//trace extension
u8			 g_bIsPt;	//is it a pt?
const Vec3D	*g_mins, *g_maxs;
Vec3D		 g_trace_absmins, g_trace_absmaxs;
const Vec3D	*g_p;
Vec3D		 g_dir;
gfxTrace	*g_pTrace;
f32			 g_tMax;

/////////////////////////////////////
// Purpose:	collide with brush
//			NOTE: pTrace->t must have
//			a value (usu. 1)!
// Output:	pTrace, if t < 1, then
//			you better do something
//			about it!
// Return:	none
/////////////////////////////////////
void IgfxQBSP::_CollBrush(s32 brushIndex)
{
	if(VIS_IS_ON(m_brushVis, brushIndex))
	{ return; }

	VIS_SET(m_brushVis, brushIndex);

	//go through each half-space and determine the interval t[min,max]
	f32 tMin=0, tMax=1;

	f32 vDotn, pDotn, tInt;

	s32 plInd;

	f32 *pPlane, dist, ofs[3];
	
	s32 indMin=0, indMax=0;

	//go through the sides
	s32 max =m_brushes[brushIndex].brushSide+m_brushes[brushIndex].numOfBrushSides;

	for(s32 i = m_brushes[brushIndex].brushSide; i < max; i++)
	{
		if(tMin > tMax) //no intersect!
			break;

		pPlane = (f32*)m_planes[m_brushSides[i].plane];

		if(!g_bIsPt)
		{	// general box case
			// push the plane out apropriately for mins/maxs
			ofs[0] = pPlane[0] < 0 ? g_maxs->x : g_mins->x;
			ofs[1] = pPlane[1] < 0 ? g_maxs->y : g_mins->y;
			ofs[2] = pPlane[2] < 0 ? g_maxs->z : g_mins->z;

			dist = pPlane[0]*ofs[0] + pPlane[1]*ofs[1] + pPlane[2]*ofs[2];
			dist -= pPlane[3];//-((-pPlane[ePD]) - dist);
		}
		else
		{	// special point case
			dist = -pPlane[3];
		}

		//normal dot ray vector
		vDotn = pPlane[0]*g_dir.x + pPlane[1]*g_dir.y + pPlane[2]*g_dir.z;

		pDotn = pPlane[0]*g_p->x + pPlane[1]*g_p->y + pPlane[2]*g_p->z;

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
	if(tMin < tMax && tMin < g_pTrace->t)
	{
		g_pTrace->t = tMin;

		plInd = m_brushSides[indMin].plane;

		g_pTrace->norm.x = m_planes[plInd].a;
		g_pTrace->norm.y = m_planes[plInd].b;
		g_pTrace->norm.z = m_planes[plInd].c;
		g_pTrace->d		 = m_planes[plInd].d;

		g_pTrace->txtInd   = m_brushSides[indMin].textureID;
		
		g_pTrace->bStartSolid = tMin < 0 ? TRUE : FALSE;
		g_pTrace->bAllSolid = (tMax < 0 && g_pTrace->bStartSolid) ? TRUE : FALSE;

		g_pTrace->brushIndex = brushIndex;
	}
	else
	{
		if(tMin < 0)
		{
			g_pTrace->bStartSolid = TRUE;

			if(tMax < 0)
				g_pTrace->bAllSolid = TRUE;

			g_pTrace->brushIndex = brushIndex;
		}
	}
}

void IgfxQBSP::_CollRecursive(s32 node, f32 p1f, f32 p2f, const Vec3D &p1, const Vec3D &p2)
{
	s8		side = 1;
	f32		t1, t2, d = 0.0f;
	f32		frac, frac2;
	f32		idist;
	Vec3D	mid;
	f32		midf;
	s32		brushIndex;

	gfxBSPLeaf *pLeaf;

	if(g_pTrace->t <= p1f) //got something better already
		return;

	if(node < 0) //are we there yet?
	{
		pLeaf = &m_leaves[~node];

		if(!TESTFLAGS(pLeaf->contents, CONTENTS_SOLID))
		{
			if(!TESTFLAGS(pLeaf->contents, CONTENTS_PLAYERCLIP))
				return;
		}

		//now go through brushes
		s32 brushCount = pLeaf->numOfLeafBrushes;

		if(brushCount > 0)
		{
			// If the current leaf is not hit by ray
			if(!BoundsIntersect(pLeaf->min, pLeaf->max, g_trace_absmins, g_trace_absmaxs))
				return;

			// If we get here, check for brushes
			// Loop through and check all of the brushes in this leaf
			while(brushCount--)
			{
				// Grab the current brush index from our leaf brushes array
				brushIndex = m_leafBrushes[pLeaf->leafBrush + brushCount];

				_CollBrush(brushIndex);
			}
		}

		//trace line against all patches in the leaf
		//FIXME!
		s32 k=0;
		gfxBSPFace *face=0;

		for(s32 j = 0; j < pLeaf->numOfLeafFaces; j++ ) 
		{
			k = m_leafFaces[pLeaf->leafface + j];
			face = &m_faces[k];

			//m_leaves[i].contents |= m_textures[face->textureID].contents;

			if(face->numbrushes
				&& BoundsIntersect(face->mins, face->maxs, g_trace_absmins, g_trace_absmaxs)) 
			{
				for(brushIndex = 0; brushIndex < face->numbrushes; brushIndex++)
					_CollBrush(face->firstbrush+brushIndex);
			}
		}

		return;
	}

	/*f32 *plane = (f32*)m_planes[m_nodes[node].plane];

	t1 = p1.x*plane[0] + p1.y*plane[1] + p1.z*plane[2] - plane[3];
	t2 = p2.x*plane[0] + p2.y*plane[1] + p2.z*plane[2] - plane[3];

	if(!g_bIsPt)
		d = fabsf(g_traceExt.x*plane[0]) +
			fabsf(g_traceExt.y*plane[1]) +
			fabsf(g_traceExt.z*plane[2]);*/

	Plane *plane = &m_planes[m_nodes[node].plane];
	
	if (plane->type < 3)
	{
		t1 = ((f32*)p1)[plane->type] - plane->d;
		t2 = ((f32*)p2)[plane->type] - plane->d;
		d = ((f32*)g_traceExt)[plane->type];
	}
	else
	{
		t1 = p1.x*plane->a + p1.y*plane->b + p1.z*plane->c - plane->d;
		t2 = p2.x*plane->a + p2.y*plane->b + p2.z*plane->c - plane->d;

		if(!g_bIsPt)
			d = fabs(g_traceExt.x*plane->a) +
				fabs(g_traceExt.y*plane->b) +
				fabs(g_traceExt.z*plane->c);
	}

	//see which sides we need to go
	if(t1 >= d && t2 >= d) //front
	{
		_CollRecursive(m_nodes[node].child[0], p1f, p2f, p1, p2);
		return;
	}
	if(t1 < -d && t2 < -d) //back
	{
		_CollRecursive(m_nodes[node].child[1], p1f, p2f, p1, p2);
		return;
	}

	//damn, that means we need to go through two path
	if (t1 < t2)
	{
		idist = 1.0f/(t1-t2);
		side = 1;
		frac2 = (t1 + d + DIST_EPSILON)*idist;
		frac = (t1 - d + DIST_EPSILON)*idist;
	}
	else if (t1 > t2)
	{
		idist = 1.0f/(t1-t2);
		side = 0;
		frac2 = (t1 - d - DIST_EPSILON)*idist;
		frac = (t1 + d + DIST_EPSILON)*idist;
	}
	else
	{
		side = 0;
		frac = 1;
		frac2 = 0;
	}

	if(frac < 0) frac = 0;
	else if(frac > 1) frac = 1;

	if(frac2 < 0) frac2 = 0;
	else if(frac2 > 1) frac2 = 1;

	//lerp to first fraction
	midf = p1f + (p2f - p1f)*frac;

	Vec3DLerp(&p1, &p2, frac, &mid);
	
	_CollRecursive(m_nodes[node].child[side], p1f, midf, p1, mid);

	//lerp to second fraction
	midf = p1f + (p2f - p1f)*frac2;

	Vec3DLerp(&p1, &p2, frac2, &mid);
	
	_CollRecursive(m_nodes[node].child[!side], midf, p2f, mid, p2);
}

/////////////////////////////////////
// Purpose:	find collision with the given
//			map.  Will collide if 't' is
//			within [0,1)
//			NOTE: testT is the starting
//				  't' value (starting minimum)
//				  usu. set as 't = 1'
// Output:	pTrace filled
// Return:	true if collide
/////////////////////////////////////
void IgfxQBSP::Collision(const Vec3D *mins, const Vec3D *maxs, const Vec3D &pt1, const Vec3D &pt2, 
						 gfxTrace *pTrace, f32 testT)
{
	QBSPVisBitClearAll(m_brushVis, m_numBrushVis);

	g_pTrace = pTrace;

	memset(g_pTrace, 0, sizeof(gfxTrace));
	g_pTrace->t = testT;

	g_tMax = testT;

	g_dir = pt2-pt1;
	g_p   = &pt1;

	//determine if we are colliding with bbox or pt.
	g_mins = mins;
	g_maxs = maxs;

	g_bIsPt = (mins && maxs) ? FALSE : TRUE;

	// build a bounding box of the entire move
	Vec3D point;

	BoundsClear(g_trace_absmins, g_trace_absmaxs);

	point = g_mins ? pt1 + (*g_mins) : pt1;
	BoundsAddPoint(point, g_trace_absmins, g_trace_absmaxs);
	point = g_maxs ? pt1 + (*g_maxs) : pt1;
	BoundsAddPoint(point, g_trace_absmins, g_trace_absmaxs);
	point = g_mins ? pt2 + (*g_mins) : pt2;
	BoundsAddPoint(point, g_trace_absmins, g_trace_absmaxs);
	point = g_maxs ? pt2 + (*g_maxs) : pt2;
	BoundsAddPoint(point, g_trace_absmins, g_trace_absmaxs);
	
	//create trace extend
	if(!g_bIsPt)
	{
		g_traceExt.x = -mins->x > maxs->x ? -mins->x : maxs->x;
		g_traceExt.y = -mins->y > maxs->y ? -mins->y : maxs->y;
		g_traceExt.z = -mins->z > maxs->z ? -mins->z : maxs->z;
	}

	//go through the bsp tree
	_CollRecursive(0, 0, testT, pt1, pt2);
}