#include "..//gfx_i.h"

#include "gfx_qbsp.h"
#include "gfx_qbsp_i.h"

s32		g_entNum = 1;

#define MAX_TOTAL_ENT_LEAFS		128

/////////////////////////////////////
// Purpose:	this will re-evaluate
//			the entity's cluster
//			and area
//			in the given bsp.
//			mins and maxs is the
//			absolute AABB position
//			of ent.  We also use
//			this for culling entities
//			later.
// Output:	entity updated
// Return:	true
/////////////////////////////////////
u32 IgfxQBSP::UpdateEnt(gfxEnt *ent, const Vec3D & mins, const Vec3D & maxs)
{
	s32			leafs[MAX_TOTAL_ENT_LEAFS]={0};
	s32			clusters[MAX_TOTAL_ENT_LEAFS]={0};
	s32			num_leafs;
	s32			i, j;
	s32			area;
	s32			topnode;

	// link to PVS leafs
	ent->num_clusters = 0;
	ent->areanum = 0;
	ent->areanum2 = 0;

	// get all leafs, including solids
	num_leafs = BoxLeafnums(mins, maxs, leafs, MAX_TOTAL_ENT_LEAFS, &topnode);

	// set areas
	for (i=0 ; i<num_leafs ; i++)
	{
		clusters[i] = LeafCluster(leafs[i]);
		area = LeafArea(leafs[i]);

		if(area)
		{	// doors may legally straggle two areas,
			// but nothing should ever need more than that
			if (ent->areanum && ent->areanum != area)
			{
				if (ent->areanum2 && ent->areanum2 != area)// && sv.state == ss_loading)
					LogMsg(LOG_FILE, L"QBSPUpdateEnt", L"Object touching 3 areas");
					
				ent->areanum2 = area;
			}
			else
				ent->areanum = area;
		}
	}

	if (num_leafs >= MAX_TOTAL_ENT_LEAFS)
	{	// assume we missed some leafs, and mark by headnode
		ent->num_clusters = -1;
		ent->headnode = topnode;
	}
	else
	{
		ent->num_clusters = 0;
		for (i=0 ; i<num_leafs ; i++)
		{
			if (clusters[i] == -1)
				continue;		// not a visible leaf
			for (j=0 ; j<i ; j++)
				if (clusters[j] == clusters[i])
					break;
			if (j == i)
			{
				if (ent->num_clusters == MAX_ENT_CLUSTERS)
				{	// assume we missed some leafs, and mark by headnode
					ent->num_clusters = -1;
					ent->headnode = topnode;
					break;
				}

				ent->clusternums[ent->num_clusters++] = clusters[i];
			}
		}
	}

	return TRUE;
}

/*
================
SV_SetAreaPortalState

Finds an areaportal leaf entity is connected with, 
and also finds two leafs from different areas connected
with the same entity.
================
*/
void IgfxQBSP::EntSetAreaPortalState(gfxEnt *ent, const Vec3D & mins, const Vec3D & maxs, u8 open)
{
	s32			i, num_leafs;
	s32			leafs[MAX_TOTAL_ENT_LEAFS];

	// entity must touch at least two areas
	if ( !ent->areanum || !ent->areanum2 ) {
		return;
	}

	if(ent->number == 0) //number hasn't been initialized
	{ ent->number = g_entNum; g_entNum++; }

	// get all leafs, including solids
	num_leafs = BoxLeafnums(mins, maxs, leafs, MAX_TOTAL_ENT_LEAFS, NULL);

	for (i=0 ; i<num_leafs ; i++)
	{
		if (LeafCluster(leafs[i]) == -1)
			continue;
		//if (QBSPLeafContents(qbsp, leafs[i]) & CONTENTS_AREAPORTAL)
		if(!TESTFLAGS(LeafContents(leafs[i]), CONTENTS_SOLID))
			break;
	}
	if ( i == num_leafs ) {
		return;
	}

	// add new areaportal if it doesn't exist
	AddAreaPortal(ent->number, ent->areanum, ent->areanum2);

	// change areaportal's state
	SetAreaPortalState(ent->number, open);
}