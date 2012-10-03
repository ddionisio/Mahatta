#include "game_local.h"

//flags
#define BLOCK_FLAG_PUSHABLE		1 //can push block
#define BLOCK_FLAG_OBJSHADOW	2 //casts shadow
#define BLOCK_FLAG_NOGRAV		4 //no gravity

struct blockDat {
	s32			parID;		//particle ID (0 == none)

	Vec3D		parOfs;		//particle offset

	FLAGS		flag;		//block flags
};

//particle, target, flags

static blockDat *block_pDat;

s32 Block_Init(hENTITY ent, void *initDat)
{
	block_pDat = (blockDat *)ent->m_usrDat;

	//allocate door data
	block_pDat = (blockDat *)g_gi.MemAlloc(sizeof(blockDat));
	ent->m_usrDat = block_pDat;

	if(!block_pDat)
	{ g_gi.LogMsg(LOG_FILE, L"EntCB_Block", L"Unable to allocate block data"); return FALSE; }

	const tCHAR *val;

	EntityParse *pEntDat = (EntityParse *)initDat;

	//set particle offset
	val = pEntDat->GetVal(L"target");
	if(val)
	{
		g_gi.WorldTargetGet(val, &block_pDat->parOfs);
		block_pDat->parOfs -= ent->m_loc;
	}

	//get particle
	val = pEntDat->GetVal(L"particle");
	if(val)
		block_pDat->parID = g_gi.ParSysStart(val);

	//get flags
	val = pEntDat->GetVal(L"flags");
	if(val)
		swscanf(val, L"%d", &block_pDat->flag);

	//cast shadow?
	f32 shadT=2;
	g_gi.ValueGetFloat(VALUE_SHADOW_TYPE, &shadT);

	if(TESTFLAGS(block_pDat->flag, BLOCK_FLAG_OBJSHADOW))
		ent->SetShadowType((eEntShadow)((u32)shadT));

	if(TESTFLAGS(block_pDat->flag, BLOCK_FLAG_PUSHABLE))
		ent->m_flag |= ENTITY_FLAG_PUSHABLE;

	if(TESTFLAGS(block_pDat->flag, BLOCK_FLAG_NOGRAV))
		ent->m_flag |= ENTITY_FLAG_NOGRAVITY;

	ent->m_boundRadius = 0;

	return TRUE;
}

s32 Block_Update(hENTITY ent)
{
	block_pDat = (blockDat *)ent->m_usrDat;

	//update particle
	if(block_pDat->parID)
	{
		//update particle
		gfxParInfo pi;
		pi.emitter_pos = ent->m_loc + block_pDat->parOfs;
		//pi.emitter_pos.y += 30;
		pi.disable = FALSE;

		g_gi.ParSysUpdate(block_pDat->parID, &pi);

		//set bound rect
		if(!ent->m_obj && !ent->m_mapMdlID)
		{
			g_gi.ParSysGetBound(block_pDat->parID, &ent->m_boundMin, &ent->m_boundMax);
		}
	}

	return TRUE;
}

s32 Block_Render(hENTITY ent, const Vec3D & eyePt)
{
	block_pDat = (blockDat *)ent->m_usrDat;

	if(block_pDat->parID)
		g_gi.ParSysRender(block_pDat->parID);

	return TRUE;
}

s32 Block_Destroy(hENTITY ent)
{
	block_pDat = (blockDat *)ent->m_usrDat;

	if(block_pDat)
	{
		g_gi.MemFree(block_pDat);
	}

	return TRUE;
}

static EntityVFTable block_vftbl = {Block_Init, Block_Update, Block_Render, Block_Destroy, 0,0,0,0};

void Spawn_Block(const EntityParse & entDat)
{
	const tCHAR *mdlStr=0;
	Vec3D loc(0,0,0), dir(0,0,0);

	hENTITY newEnt;

	mdlStr = entDat.GetVal(L"model");

	newEnt = g_gi.EntCreate(L"block", ENTITY_BLOCK, mdlStr, block_vftbl, (void*)&entDat);

	if(newEnt)
	{
		g_gi.WorldAddEntity(newEnt);
		newEnt->Release(); //let the world take care of its death
	}
}