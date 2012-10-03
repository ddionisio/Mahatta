#include "game_local.h"

struct bulletDat {
	s32		curBounce;
	s32		maxBounce;
	s32		sndBounce,sndExplode;
};

static bulletDat *bullet_pDat;

s32 Bullet_Init(hENTITY ent, void *initDat)
{
	bullet_pDat = (bulletDat *)ent->m_usrDat;

	//allocate data
	bullet_pDat = (bulletDat *)g_gi.MemAlloc(sizeof(bulletDat));
	ent->m_usrDat = bullet_pDat;

	if(!bullet_pDat)
	{ g_gi.LogMsg(LOG_FILE, L"Bullet_Init", L"Unable to allocate data"); return FALSE; }

	//const tCHAR *val, *animStr;

	bulletInit *pBI = (bulletInit *)initDat;

	bullet_pDat->sndBounce = pBI->sndBounce;
	bullet_pDat->sndExplode = pBI->sndExplode;

	ent->m_spd = pBI->spd;
	ent->m_flag = ENTITY_FLAG_NONSOLID | ENTITY_FLAG_NOGRAVITY;

	return TRUE;
}

s32 Bullet_Update(hENTITY ent)
{
	ent->MoveDir(1);

	return TRUE;
}

s32 Bullet_Destroy(hENTITY ent)
{
	bulletDat *bullet_pDat = (bulletDat *)ent->m_usrDat;

	if(bullet_pDat)
	{
		g_gi.MemFree(bullet_pDat);
	}

	return TRUE;
}

//we are colliding to given map
s32 Bullet_WorldCollide(hENTITY ent, hQBSP qbsp, gfxTrace *pTrace)
{
	bulletDat *bullet_pDat = (bulletDat *)ent->m_usrDat;

	//play sound
	bullet_pDat->curBounce++;
	if(bullet_pDat->curBounce == bullet_pDat->maxBounce)
	{
		g_gi.SoundPlay(bullet_pDat->sndExplode, ent->m_loc);
		//remove
		SETFLAG(ent->m_flag, ENTITY_FLAG_POLLDEATH);
	}
	else
	{
		f32 nd = Vec3DDot(&ent->m_dir, &pTrace->norm);

		//reflect
		Vec3D refl;

		refl = ent->m_dir - (2*nd*pTrace->norm);

		ent->m_dir = refl;
		g_gi.Vec3DNormal(&ent->m_dir, &ent->m_dir);

		g_gi.SoundPlay(bullet_pDat->sndBounce, ent->m_loc);
	}

	return TRUE;
}

//an entity is colliding to us
s32 Bullet_EntityCollide(hENTITY ent, hENTITY otherEnt, gfxTrace *pTrace)
{
	bulletDat *bullet_pDat = (bulletDat *)ent->m_usrDat;
				
	if(otherEnt)
	{
		if(otherEnt->GetType() != ENTITY_TP)
		{
			g_gi.SoundPlay(bullet_pDat->sndExplode, ent->m_loc);
			SETFLAG(ent->m_flag, ENTITY_FLAG_POLLDEATH);
		}
	}

	return TRUE;
}

//allow ourself to collide with given entity?
//return TRUE if you want, FALSE o.w. (default: TRUE)
s32 Bullet_AllowCollide(hENTITY ent, hENTITY otherEnt)
{
	bulletDat *bullet_pDat = (bulletDat *)ent->m_usrDat;
			
	if(otherEnt)
	{
		//don't collide with player entities
		if(otherEnt->GetType() == ENTITY_TP)
			return FALSE;
	}

	return TRUE;
}

static EntityVFTable bullet_vftbl = {Bullet_Init, Bullet_Update, 0, Bullet_Destroy, Bullet_WorldCollide, 
Bullet_EntityCollide, Bullet_AllowCollide, 0};

hENTITY BulletCreate(const tCHAR *mdlStr, const bulletInit & bi)
{
	hENTITY newEnt = g_gi.EntCreate(L"bullet", ENTITY_BULLET, mdlStr, bullet_vftbl, (void*)&bi);

	newEnt->m_obj->GetAABBWorld(&newEnt->m_boundMin, &newEnt->m_boundMax);

	return newEnt;
}