#include "..//gfx_i.h"

#include "gfx_qbsp.h"
#include "gfx_qbsp_i.h"

/*
=============================================================================

Build a client frame structure

This is used for culling entities from the map

=============================================================================
*/

u8		fatpvs[MAX_MAP_LEAFS/8]={0};
u8		fatphs[MAX_MAP_LEAFS/8]={0};

//u8		clientpvs[MAX_MAP_LEAFS/8];
//u8		clientphs[MAX_MAP_LEAFS/8];

/*
============
SV_FatPVS

The client will interpolate the view position,
so we can't use a single PVS point
===========
*/
void IgfxQBSP::_FatPVS(const Vec3D & org)
{
	s32		leafs[128]={0};
	s32		i, j, count;
	s32		longs;
	u8		*src;
	Vec3D	mins, maxs;

	mins.x = org.x - 8;
	mins.y = org.y - 8;
	mins.z = org.z - 8;

	maxs.x = org.x + 8;
	maxs.y = org.y + 8;
	maxs.z = org.z + 8;

	count = BoxLeafnums(mins, maxs, leafs, 128, NULL);
	if(count < 1)
	{ LogMsg(LOG_FILE, L"QBSPFatPVS", L"count < 1"); return; }

	longs = ClusterSize()>>2;

	// convert leafs to clusters
	for (i=0 ; i<count ; i++)
		leafs[i] = LeafCluster(leafs[i]);

	memcpy(fatpvs, ClusterPVS(leafs[0]), longs<<2);

	// or in all the other leaf bits
	for (i=1 ; i<count ; i++)
	{
		for (j=0 ; j<i ; j++)
			if (leafs[i] == leafs[j])
				break;
		if (j != i)
			continue;		// already have the cluster we want
		src = ClusterPVS(leafs[i]);
		for (j=0 ; j<longs ; j++)
			((s32 *)fatpvs)[j] |= ((s32 *)src)[j];
	}
}

/*
============
SV_FatPHS

===========
*/
void IgfxQBSP::_FatPHS(s32 cluster)
{
	memcpy (fatphs, ClusterPHS(cluster), ClusterSize());
}

/*
============
SV_MergePVS

Portal entities add a second PVS at old_origin to fatpvs 
===========
*/
void IgfxQBSP::_MergePVS(const Vec3D & org)
{
	s32		leafs[128];
	s32		i, j, count;
	s32		longs;
	u8		*src;
	Vec3D	mins, maxs;

	mins.x = org.x - 1;
	mins.y = org.y - 1;
	mins.z = org.z - 1;

	maxs.x = org.x + 1;
	maxs.y = org.y + 1;
	maxs.z = org.z + 1;

	count = BoxLeafnums(mins, maxs, leafs, 128, NULL);
	if (count < 1)
	{ LogMsg(LOG_FILE, L"QBSPFatPVS", L"count < 1"); return; }

	longs = ClusterSize()>>2;

	// convert leafs to clusters
	for (i=0 ; i<count ; i++)
		leafs[i] = LeafCluster(leafs[i]);

	// or in all the other leaf bits
	for (i=0 ; i<count ; i++)
	{
		for (j=0 ; j<i ; j++)
			if (leafs[i] == leafs[j])
				break;
		if (j != i)
			continue;		// already have the cluster we want
		src = ClusterPVS(leafs[i]);
		for (j=0 ; j<longs ; j++)
			((s32 *)fatpvs)[j] |= ((s32 *)src)[j];
	}
}

/*
============
SV_MergePHS

===========
*/
void IgfxQBSP::_MergePHS(s32 cluster)
{
	s32		i, longs;
	u8		*src;
	longs = ClusterSize()>>2;

	// or in all the other leaf bits
	src = ClusterPHS(cluster);
	for (i=0 ; i<longs ; i++)
		((s32 *)fatphs)[i] |= ((s32 *)src)[i];
}

/*
=============
SV_CullEntity

=============
*/
u8 IgfxQBSP::_CullEntity(gfxEnt *ent, u8 *bitvector)
{
	int i, l;

	if (ent->num_clusters == -1)
	{	// too many leafs for individual check, go by headnode
		if (!HeadnodeVisible(ent->headnode, bitvector))
			return TRUE;
	}
	else
	{	// check individual leafs
		for (i=0 ; i < ent->num_clusters ; i++)
		{
			l = ent->clusternums[i];
			if (bitvector[l >> 3] & (1 << (l&7) )) {
				break;
			}
		}
		
		if (i == ent->num_clusters)
			return TRUE;		// not visible
	}

	return FALSE;
}

//These will be used when rendering the QBSP map as well
s32					g_areabytes;
u8					g_areabits[MAX_MAP_AREAS/8];		// portalarea visibility bits

s32					g_clientarea;
s32					g_cluster;
s32					g_leafnum;

s32					g_fragmentFrame;

/////////////////////////////////////
// Purpose:	prepare qbsp for culling
//			entities and display map.
//			NOTE: You should only call
//			this once per frame and AFTER
//			CameraSetScene.
//			You need to call this before
//			rendering the qbsp and
//			culling entity
// Output:	cull info set based on
//			given pos.
// Return:	true
/////////////////////////////////////
u32 IgfxQBSP::PrepareFrame()
{
	g_leafnum = FindLeaf(g_eyePt);
	g_clientarea = LeafArea(g_leafnum);
	g_cluster = LeafCluster(g_leafnum);

	// calculate the visible areas
	g_areabytes = WriteAreaBits(g_areabits, g_clientarea);

	// save client's PVS so it can be later compared with fatpvs
	_FatPVS(g_eyePt);

	_FatPHS(g_cluster);

	return TRUE;
}

/////////////////////////////////////
// Purpose:	cull entity from qbsp
//			You must have called
//			QBSPPrepareFrame() prior
//			to this function
// Output:	none
// Return:	TRUE if entity is culled
/////////////////////////////////////
u8 IgfxQBSP::CullEnt(gfxEnt *ent)
{
	//if there are no clusters, assume entity is visible...
	if(m_clusters.numOfClusters == 0)
		return FALSE;

	// check area
	if (! (g_areabits[ent->areanum>>3] & (1<<(ent->areanum&7)) ) )
	{	// doors can legally straddle two areas, so
		// we may need to check another one
		if (!ent->areanum2
			|| !(g_areabits[ent->areanum2>>3] & (1<<(ent->areanum2&7)) ))
			return TRUE;		// blocked by a door
	}

	return _CullEntity(ent, fatpvs);
}

	/*for (e=0 ; e<numedicts ; e++)
	{
		ent = pedicts[e];

		
		
		// beams just check one point for PHS
		if (ent->s.renderfx & RF_BEAM)
		{
			l = ent->clusternums[0];
			if ( !(fatphs[l >> 3] & (1 << (l&7) )) )
				continue;
		}
		else
		{
			// FIXME: if an ent has a model and a sound, but isn't
			// in the PVS, only the PHS, clear the model
			if (ent->s.sound)
				bitvector = fatpvs;	//clientphs;
			else
				bitvector = fatpvs;
			
			if (SV_CullEntity (ent, bitvector))
				continue;
		}
		
		// add it to the circular client_entities array
		state = &svs.client_entities[svs.next_client_entities%svs.num_client_entities];
		*state = ent->s;

		// don't mark players missiles as solid
		if (ent->owner == client->edict)
			state->solid = 0;
		// don't mark entities from portals as solid
		if (portalview && !(ent->s.renderfx & RF_BEAM) && SV_CullEntity (ent, clientpvs)) {
			state->solid = 0;
		}

		svs.next_client_entities++;
		frame->num_entities++;
	}
}*/