#include "game_match.h"
#include "game_local.h"

struct RingareaDat {
	wstring entityname;	// name of this entity
	wstring side_id;	// id of a team side
};

static RingareaDat *ringarea_pDat;

s32 Ringarea_Init(hENTITY ent, void *initDat)
{
	ringarea_pDat = (RingareaDat *)ent->m_usrDat;

	ringarea_pDat = new RingareaDat;
	ent->m_usrDat = ringarea_pDat;

	if(!ringarea_pDat)
	{ g_gi.LogMsg(LOG_FILE, L"Ringarea_Init", L"Unable to allocate ring area data"); return FALSE; }

	//* BEGIN Extract data from QuArk form
	const tCHAR *val;
	EntityParse *pEntDat = (EntityParse *)initDat;

	val = pEntDat->GetVal(L"classname");
	ringarea_pDat->entityname = val ? val : L"\0";

	val = pEntDat->GetVal(L"side_id");
	ringarea_pDat->side_id = val ? val : L"\0";
	//* END Extract //

	ent->m_flag     = ENTITY_FLAG_NOWORLDCOLLIDE | ENTITY_FLAG_NONSOLID | ENTITY_FLAG_NOGRAVITY | ENTITY_FLAG_NONSOLID_COLLIDE;

	return TRUE;
}

s32 Ringarea_Destroy(hENTITY ent)
{
	ringarea_pDat = (RingareaDat *)ent->m_usrDat;

	if(ringarea_pDat)
		delete ringarea_pDat;

	return TRUE;

}

//an entity is colliding to us
s32 Ringarea_EntityCollide(hENTITY ent, hENTITY otherEnt, gfxTrace *pTrace)
{
	if(otherEnt->GetType() == ENTITY_BALL){
		ringarea_pDat = (RingareaDat *)ent->m_usrDat;
		Match::getMe()->setBallCollision(ringarea_pDat->entityname, ringarea_pDat->side_id);
		return TRUE;
	}
	return FALSE;
}

s32 Ringarea_AllowCollide(hENTITY ent, hENTITY otherEnt){
	if(otherEnt != 0 && otherEnt->GetType() == ENTITY_BALL)
		return TRUE;
	return FALSE;
}

static EntityVFTable ringarea_vftbl = {Ringarea_Init, 0,0, Ringarea_Destroy, 0, Ringarea_EntityCollide, Ringarea_AllowCollide,0};

void Spawn_Ringarea(const EntityParse & entDat)
{
	const tCHAR *val=0, *mdlStr;

	hENTITY newEnt;

	mdlStr = entDat.GetVal(L"model");

	//set the bound to be just like the model ID
	newEnt = g_gi.EntCreate(val, ENTITY_RINGAREA, mdlStr, ringarea_vftbl, (void*)&entDat);

	if(newEnt)
	{
		g_gi.WorldAddEntity(newEnt);
		newEnt->Release(); //let the world take care of its death
	}

}