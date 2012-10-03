#include "game_local.h"

struct LoadMapDat {
	wstring	mapStr;
	wstring targetSpawn;
};

//we are already loading a map
u8 g_bMapLoading = FALSE;

static LoadMapDat *loadmap_pDat;

s32 LoadMap_Init(hENTITY ent, void *initDat)
{
	loadmap_pDat = (LoadMapDat *)ent->m_usrDat;

	loadmap_pDat = new LoadMapDat;
	ent->m_usrDat = loadmap_pDat;

	if(!loadmap_pDat)
	{ g_gi.LogMsg(LOG_FILE, L"LoadMap_Init", L"Unable to allocate LoadMap data"); return FALSE; }

	const tCHAR *val;
	EntityParse *pEntDat = (EntityParse *)initDat;

	val = pEntDat->GetVal(L"map");

	loadmap_pDat->mapStr = val ? val : L"\0";

	val = pEntDat->GetVal(L"target");
	
	loadmap_pDat->targetSpawn = val ? val : L"\0";

	ent->m_flag     = ENTITY_FLAG_NOWORLDCOLLIDE | ENTITY_FLAG_NONSOLID | ENTITY_FLAG_NOGRAVITY;

	return TRUE;
}

s32 LoadMap_Destroy(hENTITY ent)
{
	loadmap_pDat = (LoadMapDat *)ent->m_usrDat;

	if(loadmap_pDat)
		delete loadmap_pDat;

	return TRUE;
}

//an entity is colliding to us
s32 LoadMap_EntityCollide(hENTITY ent, hENTITY otherEnt, gfxTrace *pTrace)
{
	loadmap_pDat = (LoadMapDat *)ent->m_usrDat;

	if(!g_bMapLoading)
	{
		g_gi.SoundStopAll();
		
		if(g_game.player && otherEnt == g_game.player->GetEnt())
		{
			//set entity's spawn target
			g_game.player->SetTargetStr(loadmap_pDat->targetSpawn.c_str());

			//load the map
			if(loadmap_pDat->mapStr.c_str()[0] != 0)
				g_gi.WorldLoad(loadmap_pDat->mapStr.c_str());

			g_bMapLoading = TRUE;
		}
	}

	return TRUE;
}

static EntityVFTable loadmap_vftbl = {LoadMap_Init, 0,0, LoadMap_Destroy, 0, LoadMap_EntityCollide, 0,0};

void Spawn_LoadMap(const EntityParse & entDat)
{
	const tCHAR *val=0, *mdlStr;

	hENTITY newEnt;

	mdlStr = entDat.GetVal(L"model");

	//set the bound to be just like the model ID
	newEnt = g_gi.EntCreate(val, ENTITY_LOADMAP, mdlStr, loadmap_vftbl, (void*)&entDat);

	if(newEnt)
	{
		g_gi.WorldAddEntity(newEnt);
		newEnt->Release(); //let the world take care of its death
	}

	g_bMapLoading = FALSE;
}