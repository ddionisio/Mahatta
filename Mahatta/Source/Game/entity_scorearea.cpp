#include "game_match.h"
#include "game_local.h"

struct ScoreareaDat {
	wstring entityname;	// name of this entity
	wstring side_id;	// id of a team side
};

static ScoreareaDat *scorearea_pDat;

s32 Scorearea_Init(hENTITY ent, void *initDat)
{
	scorearea_pDat = (ScoreareaDat *)ent->m_usrDat;

	scorearea_pDat = new ScoreareaDat;
	ent->m_usrDat = scorearea_pDat;

	if(!scorearea_pDat)
	{ g_gi.LogMsg(LOG_FILE, L"Scorearea_Init", L"Unable to allocate score area data"); return FALSE; }

	//* BEGIN Extract data from QuArk form
	const tCHAR *val;
	EntityParse *pEntDat = (EntityParse *)initDat;

	val = pEntDat->GetVal(L"classname");
	scorearea_pDat->entityname = val ? val : L"\0";

	val = pEntDat->GetVal(L"side_id");
	scorearea_pDat->side_id = val ? val : L"\0";
	//* END Extract //

	ent->m_flag     = ENTITY_FLAG_NOWORLDCOLLIDE | ENTITY_FLAG_NONSOLID | ENTITY_FLAG_NOGRAVITY | ENTITY_FLAG_NONSOLID_COLLIDE;

	return TRUE;
}

s32 Scorearea_Destroy(hENTITY ent)
{
	scorearea_pDat = (ScoreareaDat *)ent->m_usrDat;

	if(scorearea_pDat)
		delete scorearea_pDat;

	return TRUE;

}

//an entity is colliding to us
s32 Scorearea_EntityCollide(hENTITY ent, hENTITY otherEnt, gfxTrace *pTrace)
{
	if(otherEnt->GetType() == ENTITY_BALL){
		scorearea_pDat = (ScoreareaDat *)ent->m_usrDat;
		Match::getMe()->setBallCollision(scorearea_pDat->entityname, scorearea_pDat->side_id);
		return TRUE;
	}
	return FALSE;
}

s32 Scorearea_AllowCollide(hENTITY ent, hENTITY otherEnt){
	if(otherEnt != 0 && otherEnt->GetType() == ENTITY_BALL)
		return TRUE;
	return FALSE;
}

static EntityVFTable scorearea_vftbl = {Scorearea_Init, 0,0, Scorearea_Destroy, 0, Scorearea_EntityCollide, Scorearea_AllowCollide, 0};

void Spawn_Scorearea(const EntityParse & entDat)
{
	const tCHAR *val=0, *mdlStr;

	hENTITY newEnt;

	mdlStr = entDat.GetVal(L"model");

	//set the bound to be just like the model ID
	newEnt = g_gi.EntCreate(val, ENTITY_SCOREAREA, mdlStr, scorearea_vftbl, (void*)&entDat);

	if(newEnt)
	{
		g_gi.WorldAddEntity(newEnt);
		newEnt->Release(); //let the world take care of its death
	}

}