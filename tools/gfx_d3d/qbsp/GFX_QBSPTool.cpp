#include "..//gfx_i.h"

#include "GFX\gfx_math.h"

#include "gfx_qbsp.h"
#include "gfx_qbsp_i.h"

/////////////////////////////////////
// Purpose:	find the leaf index where
//			the given pt resides.
// Output:	none
// Return:	leaf index >= 0 if found
/////////////////////////////////////
s32 IgfxQBSP::FindLeaf(const Vec3D & pt)
{
	s32 i = 0, pI;
	f32 distance = 0.0f;

	while(i >= 0)
	{
		pI = m_nodes[i].plane;

		distance = PlaneDiff(&pt, &m_planes[pI]);/*m_planes[pI].a*pt.x + m_planes[pI].b*pt.y
			      +m_planes[pI].c*pt.z - m_planes[pI].d;*/

		if(distance >= 0) //front
			i = m_nodes[i].child[0];
		else
			i = m_nodes[i].child[1];
	}

	return ~i; //== -(i+1)
}

/////////////////////////////////////
// Purpose:	get the effect
// Output:	none
// Return:	the effect
/////////////////////////////////////
hEFFECT IgfxQBSP::GetEffect(u32 txtInd)
{
	return m_textures[txtInd].fx;
}

/////////////////////////////////////
// Purpose:	get the texture
// Output:	none
// Return:	the texture
/////////////////////////////////////
hTXT IgfxQBSP::GetTexture(u32 txtInd)
{
	return m_textures[txtInd].texture;
}

/////////////////////////////////////
// Purpose:	grab the flags of texture
//			(based on QBSP texture flags)
// Output:	none
// Return:	flags
/////////////////////////////////////
u32 IgfxQBSP::GetTextureFlags(u32 txtInd)
{
	return m_textures[txtInd].flags;
}

/////////////////////////////////////
// Purpose:	grab the contents of texture
//			(based on QBSP texture contents)
// Output:	none
// Return:	contents
/////////////////////////////////////
u32 IgfxQBSP::GetTextureContents(u32 txtInd)
{
	return m_textures[txtInd].contents;
}

//
// Useful BSP data for culling and such
//

u32 IgfxQBSP::GetNumLeaves()
{
	return m_numLeaves;
}

s32	IgfxQBSP::ClusterSize()
{
	return m_clusters.bytesPerCluster;
}

s32	IgfxQBSP::NumClusters()
{
	return m_clusters.numOfClusters;
}

s32	IgfxQBSP::LeafContents(s32 leafnum)
{
	if (leafnum < 0 || leafnum >= m_numLeaves)
		LogMsg(LOG_FILE, L"QBSPLeafContents", L"bad number");
	return m_leaves[leafnum].contents;
}

s32	IgfxQBSP::LeafCluster(s32 leafnum)
{
	if (leafnum < 0 || leafnum >= m_numLeaves)
		LogMsg(LOG_FILE, L"QBSPLeafCluster", L"bad number");
	return m_leaves[leafnum].cluster;
}

s32	IgfxQBSP::LeafArea(s32 leafnum)
{
	if (leafnum < 0 || leafnum >= m_numLeaves)
		LogMsg(LOG_FILE, L"QBSPLeafArea", L"bad number");
	return m_leaves[leafnum].area;
}

/*
=============
CM_BoxLeafnums

Fills in a list of all the leafs touched
=============
*/
s32		leaf_count, leaf_maxcount;
s32		*leaf_list;
const Vec3D	*leaf_mins, *leaf_maxs;
s32		leaf_topnode;

void IgfxQBSP::_BoxLeafnums_r(s32 nodenum)
{
	gfxBSPNode	*node;
	s32	s;

	while (1)
	{
		if (nodenum < 0)
		{
			if (leaf_count >= leaf_maxcount)
			{
				return;
			}

			leaf_list[leaf_count++] = ~nodenum;//-1 - nodenum;//
			return;
		}

		node = &m_nodes[nodenum];
		s = BoxOnPlaneSide(leaf_mins, leaf_maxs, &m_planes[node->plane]);

		/*Plane *plane = &m_planes[node->plane];

		f32 d = fabsf(g_traceExt.x*plane->a) +
			fabsf(g_traceExt.y*plane->b) +
			fabsf(g_traceExt.z*plane->c);*/

		if (s == 1)
		//if(plane->d >= d) //front
			nodenum = node->child[0];
		else if (s == 2)
		//else if(plane->d < -d) //back
			nodenum = node->child[1];
		else
		{	// go down both
			if (leaf_topnode == -1)
				leaf_topnode = nodenum;
			_BoxLeafnums_r (node->child[0]);
			nodenum = node->child[1];
		}
	}
}

s32	IgfxQBSP::BoxLeafnums(const Vec3D & mins, const Vec3D & maxs, s32 *list, s32 listsize, s32 *topnode)
{
	leaf_list = list;
	leaf_count = 0;
	leaf_maxcount = listsize;
	leaf_mins = &mins;
	leaf_maxs = &maxs;

	leaf_topnode = -1;

	//g_traceExt.x = -mins.x > maxs.x ? -mins.x : maxs.x;
	//g_traceExt.y = -mins.y > maxs.y ? -mins.y : maxs.y;
	//g_traceExt.z = -mins.z > maxs.z ? -mins.z : maxs.z;

	_BoxLeafnums_r(0);

	if (topnode)
		*topnode = leaf_topnode;

	return leaf_count;
}

/*
===============================================================================

PVS / PHS

===============================================================================
*/

u8	*IgfxQBSP::ClusterPVS (s32 cluster)
{
	if (cluster != -1 && m_clusters.numOfClusters)
		return (u8 *)m_clusters.pBitsets + cluster * m_clusters.bytesPerCluster;

	return g_nullrow;
}

u8	*IgfxQBSP::ClusterPHS (s32 cluster)
{
	if (cluster != -1 && m_clustersPHS.numOfClusters)
		return (u8 *)m_clustersPHS.pBitsets + cluster * m_clustersPHS.bytesPerCluster;

	return g_nullrow;
}

/*
=============
CM_HeadnodeVisible

Returns true if any leaf under headnode has a cluster that
is potentially visible
=============
*/
u8 IgfxQBSP::HeadnodeVisible(s32 nodenum, u8 *visbits)
{
	s32			leafnum;
	s32			cluster;
	gfxBSPNode	*node;

	if (nodenum < 0)
	{
		leafnum = ~nodenum;//-1-nodenum;
		cluster = m_leaves[leafnum].cluster;
		if (cluster == -1)
			return FALSE;
		if (visbits[cluster>>3] & (1<<(cluster&7)))
			return TRUE;
		return FALSE;
	}

	node = &m_nodes[nodenum];
	if (HeadnodeVisible(node->child[0], visbits))
		return TRUE;
	return HeadnodeVisible(node->child[1], visbits);
}

/////////////////////////////////////
// Purpose:	checks to see if p2 is
//			visible from p1 based on
//			given map
// Output:	none
// Return:	TRUE if p2 is visible
/////////////////////////////////////
u8 IgfxQBSP::InPVS(const Vec3D & p1, const Vec3D & p2)
{
	s32		leafnum;
	s32		cluster;
	s32		area1, area2;
	u8		*mask;

	leafnum = FindLeaf(p1);
	cluster = LeafCluster(leafnum);
	area1 = LeafArea(leafnum);
	mask = ClusterPVS(cluster);

	leafnum = FindLeaf(p2);
	cluster = LeafCluster(leafnum);
	area2 = LeafArea(leafnum);
	if(mask && (!(mask[cluster>>3] & (1<<(cluster&7)))))
		return FALSE;
	if(!AreasConnected(area1, area2))
		return FALSE;		// a door blocks sight
	return TRUE;
}

/////////////////////////////////////
// Purpose:	checks to see if p2 is
//			hearable from p1 based on
//			given map
// Output:	none
// Return:	TRUE if p2 is hearable
/////////////////////////////////////
u8 IgfxQBSP::InPHS(const Vec3D & p1, const Vec3D & p2)
{
	s32		leafnum;
	s32		cluster;
	s32		area1, area2;
	u8		*mask;

	leafnum = FindLeaf(p1);
	cluster = LeafCluster(leafnum);
	area1 = LeafArea(leafnum);
	mask = ClusterPHS(cluster);

	leafnum = FindLeaf(p2);
	cluster = LeafCluster(leafnum);
	area2 = LeafArea(leafnum);
	if(mask && (!(mask[cluster>>3] & (1<<(cluster&7)))))
		return FALSE;
	if(!AreasConnected(area1, area2))
		return FALSE;		// a door blocks hearing
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// Clipper
static gfxVtxQBSP *qbspVtx;
static u16		  *qbspInd;

static s32		numFragmentVerts;
static s32		maxFragmentVerts;
static Vec3D   *fragmentVerts;

static s32		numClippedFragments;
static s32		maxClippedFragments;
static gfxBSPFragment *clippedFragments;

static Plane	fragmentPlanes[6];

#define	MAX_FRAGMENT_VERTS	128

#define	SIDE_FRONT			0
#define	SIDE_BACK			1
#define	SIDE_ON				2

/*
=================
R_ClipPoly
=================
*/
void R_ClipPoly(s32 nump, Vec3D *vecs, s32 stage, gfxBSPFragment *fr)
{
	Plane *plane;
	u8	   front;
	Vec3D *v;
	f32	   d;
	f32	   dists[MAX_FRAGMENT_VERTS];
	u8     sides[MAX_FRAGMENT_VERTS];
	Vec3D  newv[MAX_FRAGMENT_VERTS];
	s32		newc;
	s32		i;

	if (nump > MAX_FRAGMENT_VERTS-2)
	{ LogMsg(LOG_FILE, L"R_ClipPoly", L"MAX_FRAGMENT_VERTS"); return; }

	if (stage == 6)
	{	// fully clipped
		if ( nump > 2 ) {
			fr->numverts = nump;
			fr->firstvert = numFragmentVerts;

			if ( numFragmentVerts+nump > maxFragmentVerts ) {
				nump = maxFragmentVerts - numFragmentVerts;
			}

			for ( i = 0, v = vecs; i < nump; i++, v++) {
				fragmentVerts[numFragmentVerts+i] = *v;
			}

			numFragmentVerts += nump;
		}

		return;
	}

	front = FALSE;
	plane = &fragmentPlanes[stage];
	for ( i = 0, v = vecs; i < nump; i++, v++)
	{
		d = PlaneDiff( v, plane);
		if (d > ON_EPSILON)
		{
			front = TRUE;
			sides[i] = SIDE_FRONT;
		}
		else if (d < -ON_EPSILON)
		{
			sides[i] = SIDE_BACK;
		}
		else
		{
			sides[i] = SIDE_ON;
		}

		dists[i] = d;
	}

	if ( !front ) {
		return;
	}

	// clip it
	sides[i] = sides[0];
	dists[i] = dists[0];

	vecs[i] = *vecs;//VectorCopy (vecs, (vecs+(i*4)) );
	
	newc = 0;

	for (i = 0, v = vecs; i < nump; i++, v++)
	{
		switch (sides[i])
		{
		case SIDE_FRONT:
		case SIDE_ON:
			newv[newc] = *v;
			newc++;
			break;
		case SIDE_BACK:
			break;
		}

		if (sides[i] == SIDE_ON || sides[i+1] == SIDE_ON || sides[i+1] == sides[i])
			continue;

		d = dists[i] / (dists[i] - dists[i+1]);

		//newv[newc][j] = v[j] + d * (v[j+4] - v[j]);
		newv[newc].x = v->x + d * ((v+1)->x - v->x);
		newv[newc].y = v->y + d * ((v+1)->y - v->y);
		newv[newc].z = v->z + d * ((v+1)->z - v->z);
			
		newc++;
	}

	// continue
	R_ClipPoly ( newc, newv, stage+1, fr);
}

/*
=================
R_PlanarSurfClipFragment
=================
*/
void QBSPPlanarSurfClipFragment (gfxBSPFace *surf, const Vec3D & normal)
{
	s32			i;
	gfxBSPFragment *fr;
	gfxVtxQBSP	*vert;
	u16			*index;
	Vec3D		verts[MAX_FRAGMENT_VERTS];

	// greater than 60 degrees
	if (Vec3DDot(&surf->vNormal, &normal) < 0.5f) {
		return;
	}

	// copy vertex data and clip to each triangle
	index = &qbspInd[surf->meshVertIndex];
	vert = &qbspVtx[surf->startVertIndex];
	for ( i = 0; i < surf->numMeshVerts; i += 3, index += 3 ) {
		fr = &clippedFragments[numClippedFragments];
		fr->numverts = 0;

		verts[0] = vert[index[0]].pt;
		verts[1] = vert[index[1]].pt;
		verts[2] = vert[index[2]].pt;

		verts[0] += 0.1f*vert[index[0]].n;
		verts[1] += 0.1f*vert[index[1]].n;
		verts[2] += 0.1f*vert[index[2]].n;

		R_ClipPoly(3, verts, 0, fr);

		if ( fr->numverts ) {
			numClippedFragments++;

			if ( numFragmentVerts >= maxFragmentVerts ||
				numClippedFragments >= maxClippedFragments ) {
				return;
			}
		}
	}
}

/*
=================
R_RecursiveFragmentNode
=================
*/
void IgfxQBSP::_RecursiveFragmentNode(s32 node, const Vec3D & origin, f32 radius, const Vec3D & normal)
{
	f32	dist;
	Plane *plane;

mark0:
	if ( numFragmentVerts >= maxFragmentVerts ||
		numClippedFragments >= maxClippedFragments) {
		return;			// already reached the limit somewhere else
	}

	if (node < 0) {	// leaf
		s32 c;
		gfxBSPLeaf *leaf;
		gfxBSPFace *surf;
		gfxTxtQBSP *shaderref;

		leaf = &m_leaves[~node];
		c = leaf->numOfLeafFaces;
		
		if(!c)
			return;

		do
		{
			if ( numFragmentVerts >= maxFragmentVerts ||
				numClippedFragments >= maxClippedFragments ) {
				return;
			}

			if(leaf->leafface + c >= m_numLeafFaces)
				return;

			surf = &m_faces[m_leafFaces[leaf->leafface + c]];

			if ( surf->fragmentframe == g_fragmentFrame ) {
				continue;
			}

			surf->fragmentframe = g_fragmentFrame;

			if (surf->textureID < 0) {
				continue;
			}

			shaderref = &m_textures[surf->textureID];
			
			if ( shaderref->flags & (SURF_NOMARKS|SURF_NOIMPACT) ) {
				continue;
			}

			if(surf->type == 1)
			{
				if(shaderref->contents & CONTENTS_SOLID) 
				{
					QBSPPlanarSurfClipFragment(surf, normal);
				}
			} 
			else if(surf->type == 2) 
			{
				//R_PatchSurfClipFragment ( surf, normal );
			}
		} while (--c);

		return;
	}

#if 1
	plane = &m_planes[m_nodes[node].plane];

	dist = origin.x*plane->a + origin.y*plane->b + origin.z*plane->c - plane->d;//PlaneDiff(&origin, plane);

	if ( dist > radius )
	{
		node = m_nodes[node].child[0];
		goto mark0;
	}
	if ( dist < -radius )
	{
		node = m_nodes[node].child[1];
		goto mark0;
	}
#endif
	
	_RecursiveFragmentNode(m_nodes[node].child[0], origin, radius, normal );
	_RecursiveFragmentNode(m_nodes[node].child[1], origin, radius, normal );
}

s32 IgfxQBSP::GetClippedFragments(const Vec3D & origin, f32 radius, const Vec3D axis[3], 
							s32 maxfverts, Vec3D *fverts, s32 maxfragments, 
							gfxBSPFragment *fragments)
{
	//lock the vertex and index buffer
	VtxLock(m_vtx, 0, 0,0, (void**)&qbspVtx, D3DLOCK_READONLY);
	IndLock(m_meshIndBuff, 0,0, (void**)&qbspInd, D3DLOCK_READONLY);

	s32 i;
	f32 d;

	g_fragmentFrame++;

	// initialize fragments
	numFragmentVerts = 0;
	maxFragmentVerts = maxfverts;
	fragmentVerts = fverts;

	numClippedFragments = 0;
	maxClippedFragments = maxfragments;
	clippedFragments = fragments;

	// calculate clipping planes
	for ( i = 0; i < 3; i++ ) {
		d = Vec3DDot(&origin, &axis[i]);

		fragmentPlanes[i*2].a = axis[i].x;
		fragmentPlanes[i*2].b = axis[i].y;
		fragmentPlanes[i*2].c = axis[i].z;
		fragmentPlanes[i*2].d = d - radius;

		fragmentPlanes[i*2].type = PlaneTypeForNormal(&fragmentPlanes[i*2]);

		fragmentPlanes[i*2+1].a = -axis[i].x;
		fragmentPlanes[i*2+1].b = -axis[i].y;
		fragmentPlanes[i*2+1].c = -axis[i].z;
		fragmentPlanes[i*2+1].d = -d - radius;

		fragmentPlanes[i*2+1].type = PlaneTypeForNormal(&fragmentPlanes[i*2+1]);
	}

	_RecursiveFragmentNode(0, origin, radius, axis[0]);

	VtxUnlock(m_vtx, 0);
	IndUnlock(m_meshIndBuff);

	return numClippedFragments;
}