#include "game_ball.h"
#include "game_local.h"

struct CourtsideDat {
	wstring entityname;	// name of this entity
	wstring side_id;	// id of a team side
};

static CourtsideDat *courtside_pDat;

s32 Courtside_Init(hENTITY ent, void *initDat)
{
	courtside_pDat = (CourtsideDat *)ent->m_usrDat;

	courtside_pDat = new CourtsideDat;
	ent->m_usrDat = courtside_pDat;

	if(!courtside_pDat)
	{ g_gi.LogMsg(LOG_FILE, L"Courtside_Init", L"Unable to allocate court side data"); return FALSE; }

	//* BEGIN Extract data from QuArk form
	const tCHAR *val;
	EntityParse *pEntDat = (EntityParse *)initDat;

	val = pEntDat->GetVal(L"classname");
	courtside_pDat->entityname = val ? val : L"\0";

	val = pEntDat->GetVal(L"side_id");
	courtside_pDat->side_id = val ? val : L"\0";
	//* END Extract //

	ent->m_flag     = ENTITY_FLAG_NOWORLDCOLLIDE | ENTITY_FLAG_NONSOLID | ENTITY_FLAG_NOGRAVITY | ENTITY_FLAG_NONSOLID_COLLIDE;

	return TRUE;
}

s32 Courtside_Destroy(hENTITY ent)
{
	courtside_pDat = (CourtsideDat *)ent->m_usrDat;

	if(courtside_pDat)
		delete courtside_pDat;

	return TRUE;

}

//an entity is colliding to us
s32 Courtside_EntityCollide(hENTITY ent, hENTITY otherEnt, gfxTrace *pTrace)
{
	if(otherEnt->GetType() == ENTITY_BALL_SCOUT)
	{
		OnCourtsideHit(otherEnt, static_cast<CourtsideDat*>(ent->m_usrDat)->side_id[0] - L'0');
	}
	return FALSE;
}

static EntityVFTable courtside_vftbl = {Courtside_Init, 0,0, Courtside_Destroy, 0, Courtside_EntityCollide, 0,0};

void Spawn_Courtside(const EntityParse & entDat)
{
	const tCHAR *val=0, *mdlStr;

	hENTITY newEnt;

	mdlStr = entDat.GetVal(L"model");

	//set the bound to be just like the model ID
	newEnt = g_gi.EntCreate(val, ENTITY_COURTSIDE, mdlStr, courtside_vftbl, (void*)&entDat);

	if(newEnt)
	{
		g_gi.WorldAddEntity(newEnt);
		newEnt->Release(); //let the world take care of its death
	}

}