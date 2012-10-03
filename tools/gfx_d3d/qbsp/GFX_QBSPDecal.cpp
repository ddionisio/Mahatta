#include "..//gfx_i.h"

#include "GFX\gfx_math.h"

#include "gfx_qbsp.h"
#include "gfx_qbsp_i.h"

typedef struct _decalVtx {
    Vec3D pt;		 //world space vertex position
	f32 s, t;		 //texture coord
} decalVtx;

#define DECALVERTEXFLAG (GFXFVF_XYZ | GFXFVF_TEX1) //xyz and texture

#define DECAL_VTXBUFF_USAGE	GFXUSAGE_WRITEONLY//D3DUSAGE_DYNAMIC//D3DUSAGE_SOFTWAREPROCESSING//
#define DECAL_VTXBUFF_POOL	GFXPOOL_MANAGED//D3DPOOL_DEFAULT

#define MAX_POLY_VERTS	3000

/////////////////////////////////////
// Purpose:	clear out all decals
// Output:	all decals cleared
// Return:	none
/////////////////////////////////////
void IgfxQBSP::ClearDecals()
{
	u32 i;

	for(i = 0; i < MAX_DECALS; i++)
	{
		GFXDestroyVtxBuff(m_decals[i].vtx);
		SAFE_RELEASE(m_decals[i].fx);
		SAFE_RELEASE(m_decals[i].txt);
	}

	memset(m_decals, 0, sizeof(m_decals));

	// link decals
	m_decals_free = m_decals;

	m_decals_headnode.prev = &m_decals_headnode;
	m_decals_headnode.next = &m_decals_headnode;

	for(i = 0; i < MAX_DECALS - 1; i++) {
		m_decals[i].next = &m_decals[i+1];
	}
}

gfxBSPDecal * IgfxQBSP::_DecalAlloc()
{
	gfxBSPDecal *dl;

	if (m_decals_free) {	// take a free decal if possible
		dl = m_decals_free;
		m_decals_free = dl->next;
	} else {				// grab the oldest one otherwise
		dl = m_decals_headnode.prev;
		dl->prev->next = dl->next;
		dl->next->prev = dl->prev;
	}

	// put the decal at the start of the list
	dl->prev = &m_decals_headnode;
	dl->next = m_decals_headnode.next;
	dl->next->prev = dl;
	dl->prev->next = dl;

	return dl;
}

void IgfxQBSP::_DecalFree(gfxBSPDecal *dl)
{
	// remove from linked active list
	dl->prev->next = dl->next;
	dl->next->prev = dl->prev;

	// insert into linked free list
	dl->next = m_decals_free;
	m_decals_free = dl;
	
	//free up some stuff
	GFXDestroyVtxBuff(dl->vtx); dl->vtx=0;

	SAFE_RELEASE(dl->fx);
	SAFE_RELEASE(dl->txt);
}

/////////////////////////////////////
// Purpose:	this will add a decal to
//			given map
// Output:	new decal added
// Return:	none
/////////////////////////////////////
void IgfxQBSP::SpawnDecal(const Vec3D & origin, const Vec3D & dir, f32 orient, f32 radius,
								const Vec4D & clr, f32 die, f32 fadetime, u8 fadealpha, 
								hTXT txt, hEFFECT fx)
{
	s32 i, j;
	Vec3D axis[3], v;
	Vec3D verts[MAX_DECAL_VERTS];
	gfxBSPFragment *fr, fragments[MAX_DECAL_FRAGMENTS];
	s32 numfragments;

	gfxBSPDecal *dl;

	decalVtx *pVtx;

	//if ( !cl_add_decals->value ) {
	//	return;
	//}

	// invalid decal
	if (radius <= 0 || (dir.x == 0 && dir.y == 0 && dir.z == 0))
		return;

	// calculate orientation matrix
	if(Vec3DNormal(&dir, &axis[0]) == 0)
		memset(&axis[0], 0, sizeof(axis[0]));

	Vec3DPerpendicular(&axis[0], &axis[1]);

	Vec3DRotatePointAroundVector(&axis[0], &axis[1], orient, &axis[2]);

	Vec3DCross(&axis[0], &axis[2], &axis[1]);

	// the bitch part, clip it!!!
	numfragments = GetClippedFragments(origin, radius, axis, 
		MAX_DECAL_VERTS, verts, MAX_DECAL_FRAGMENTS, fragments);

	// no valid fragments
	if ( !numfragments ) {
		return;
	}

	//Assemble the decal
	axis[1] *= 0.5f / radius;
	axis[2] *= 0.5f / radius;
	
	for(i = 0, fr = fragments; i < numfragments; i++, fr++) 
	{
		if(fr->numverts > MAX_POLY_VERTS)
			fr->numverts = MAX_POLY_VERTS;
		else if(fr->numverts <= 0)
			continue;

		// allocate decal
		dl = _DecalAlloc();

		dl->time = 0;
		dl->die = die;
		dl->fadetime = (die - min(fadetime, die));
		dl->fadefreq = 1.0f/min(fadetime, die);
		dl->fadealpha = fadealpha;

		dl->fx = fx; if(dl->fx) dl->fx->AddRef();
		dl->txt = txt; if(dl->txt) dl->txt->AddRef();
		dl->color = clr; 
		dl->color_cur = clr;

		dl->vtx = GFXCreateVtxBuff(sizeof(decalVtx), fr->numverts, DECAL_VTXBUFF_USAGE, DECALVERTEXFLAG, DECAL_VTXBUFF_POOL);
		dl->vtxNum = fr->numverts;

		if(!dl->vtx) //failed to allocate vertex buffer
		{ _DecalFree(dl); continue; }

		VtxLock(dl->vtx, 0, 0,0, (void**)&pVtx, 0);

		for(j = 0; j < fr->numverts; j++) 
		{
			pVtx[j].pt = verts[fr->firstvert+j];

			v = pVtx[j].pt - origin;

			pVtx[j].s = Vec3DDot(&v, &axis[1]) + 0.5f;
			pVtx[j].t = Vec3DDot(&v, &axis[2]) + 0.5f;
		}

		VtxUnlock(dl->vtx, 0);
	}
}

/////////////////////////////////////
// Purpose:	this will render the
//			decals (clearing out
//			those that expire as well)
// Output:	decals added to scene list
// Return:	none
/////////////////////////////////////
void IgfxQBSP::RenderDecals()
{
	f32			fade;
	gfxBSPDecal	*dl, *next, *hnode;

	gfxMaterial mat={0};

	SceneGeometry g={0};

	g.pMat = &mat;
	g.primType = GFXPT_TRIANGLEFAN;
	g.wrld = &m_wrldMtx;
	g.flag = SCENE_SINGLEMTX;

	// add decals to scene in first-spawed - first-drawn order
	hnode = &m_decals_headnode;
	for (dl = hnode->prev; dl != hnode; dl = next) 
	{
		next = dl->prev;

		dl->time += g_time;

		// it's time to DIE!!!
		if(dl->die <= dl->time)
		{ _DecalFree(dl); continue; }

		// fade out
		if(dl->fadetime < dl->time)
		{
			fade = (dl->die - dl->time) * dl->fadefreq;

			if(dl->fadealpha)
				dl->color_cur.w = dl->color.w*fade;
			else
			{
				dl->color_cur.x = dl->color.x*fade;
				dl->color_cur.y = dl->color.y*fade;
				dl->color_cur.z = dl->color.z*fade;
			}
		}

		if(dl->fx)
		{
			mat.Diffuse = dl->color_cur;

			g.fx = (hIEFFECT)dl->fx;
			g.pVtx = dl->vtx;
			g.numVtx = dl->vtxNum;
			g.numFaces = dl->vtxNum-2;

			GFXSceneAdd(g);
		}
		else if(dl->txt) //use texture display's FX, it is essentially the same
		{
			((hITXT)dl->txt)->Blt3DEX(dl->vtx, GFXPT_TRIANGLEFAN, dl->vtxNum, dl->vtxNum-2,
				&m_wrldMtx, dl->color_cur);
		}
	}
}