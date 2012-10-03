#include "game_teamplayer.h"
#include "game_local.h"
/*
NPC_STATE_CONV_S,	//starting conversation
	NPC_STATE_CONV_M,	//middle conversation
	NPC_STATE_CONV_E,	//ending conversation
*/
tCHAR *g_NPCanimStr[NPC_STATE_MAX]={L"idle0", L"idle1", L"idle2", L"walk"};

#define ANIM_SEC		(L"anim")
#define CONV_SEC		(L"conv")

#define NPCIDLE_DELAY_MIN	5.000f
#define NPCIDLE_DELAY_MAX	10.000f

struct NPCDat {
	f32			radius;		//the radius(squared) for detecting player

	hWPNODE		wp;			//the waypoint node (if this is NULL, we don't move)

	s32			m_movAnim[NPC_STATE_MAX];	//the animation IDs for each state

	NPCMovAnim  m_curMovAnim;		//current movement animation (used only by idle)

	//when player gets near, delay for a bit once the player is away
	f32			m_pauseCurDelay;
	f32			m_pauseDelay;

	//slowly get to the next direction
	Vec3D		m_dirCur, m_dirNext;			//the next direction
	f32			m_dirCurDelay;
	f32			m_dirDelay;

	f32			m_idleCurDelay;
	f32			m_idleDelay;
};

static NPCDat *npc_pDat;

//move the NPC on the waypoints
void _NPCUpdateWP(hENTITY ent)
{
	if(npc_pDat->wp)
	{
		Vec3D wpt;

		npc_pDat->wp->GetLoc(&wpt);

		Vec3D dir(wpt - ent->m_loc);
		dir.y = 0;

		f32 len = Vec3DLen(&dir);

		if(len > ent->m_spd*g_gi.EngineGetTick())
		{
			ent->m_dir = dir / len;

			ent->MoveDir(1.0f);
		}
		//we are close enough
		//get to next waypoint node
		else
		{
			ent->m_vel += dir;

			npc_pDat->wp->GotoNext();

			if(npc_pDat->wp->GetWait() > 0)
			{
				npc_pDat->m_dirCur = ent->m_dir;
				npc_pDat->wp->GetDir(&npc_pDat->m_dirNext);
				npc_pDat->m_dirCurDelay = 0;
			}
		}
		
		if(npc_pDat->wp->IsDone()) //erase?
		{ npc_pDat->wp->Release(); npc_pDat->wp = 0; }
	}
}

s32 NPC_Init(hENTITY ent, void *initDat)
{
	npc_pDat = (NPCDat *)ent->m_usrDat;

	npc_pDat = (NPCDat *)g_gi.MemAlloc(sizeof(NPCDat));
	ent->m_usrDat = npc_pDat;

	hCFG cfg = (hCFG)initDat;

	if(cfg)
	{
		tCHAR buff[MAXCHARBUFF];

		//get animations
		for(u8 i = 0; i < NPC_STATE_MAX; i++)
		{
			if(cfg->GetItemStr(ANIM_SEC, g_NPCanimStr[i], buff, MAXCHARBUFF))
			{
				if(ent->m_obj && ent->m_obj->GetModel())
					npc_pDat->m_movAnim[i] = ent->m_obj->GetModel()->GetAnimID(buff);
			}
			else
				npc_pDat->m_movAnim[i] = -1;
		}
	}

	npc_pDat->m_pauseCurDelay = npc_pDat->m_pauseDelay = 1.000f;
	npc_pDat->m_dirCurDelay = npc_pDat->m_dirDelay = 0.250f;

	npc_pDat->m_idleDelay = g_gi.RandRngFloat(NPCIDLE_DELAY_MIN, NPCIDLE_DELAY_MAX);

	//set initial animations
	npc_pDat->m_curMovAnim = NPC_STATE_IDLE_0;
	ent->m_obj->MixerBlendCycle(npc_pDat->m_movAnim[npc_pDat->m_curMovAnim], 1.0f, 0);

	return TRUE;
}

s32 NPC_Update(hENTITY ent)
{
	npc_pDat = (NPCDat *)ent->m_usrDat;

	f32 tick = g_gi.EngineGetTick();

	u8 bDontMove = FALSE;

	///////////////////////////////////////////////////////////////////////////////////
	//check to see if player is nearby
	if(g_game.player)
	{
		Vec3D plyrToNPC(g_game.player->GetEnt()->m_loc-ent->m_loc);
		f32 lenSQ = Vec3DDot(&plyrToNPC, &plyrToNPC);
		
		if(lenSQ < npc_pDat->radius)
		{
			bDontMove = TRUE;

			//set dir
			ent->m_dir = plyrToNPC/lenSQ;

			npc_pDat->m_pauseCurDelay = 0;
		}
	}

	///////////////////////////////////////////////////////////////////////////////////
	//smoothly rotate the npc to the next direction
	if(npc_pDat->m_dirCurDelay < npc_pDat->m_dirDelay)
	{
		npc_pDat->m_dirCurDelay += tick;

		f32 t = npc_pDat->m_dirCurDelay/npc_pDat->m_dirDelay; if(t > 1) t = 1;
		ent->m_dir = npc_pDat->m_dirCur + t*(npc_pDat->m_dirNext-npc_pDat->m_dirCur);
		g_gi.Vec3DNormal(&ent->m_dir, &ent->m_dir);
	}

	///////////////////////////////////////////////////////////////////////////////////
	//movement
	if(!bDontMove)
	{
		if(npc_pDat->m_pauseCurDelay < npc_pDat->m_pauseDelay)
			npc_pDat->m_pauseCurDelay += tick;
		else
		{
			_NPCUpdateWP(ent);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////
	//idle
	if(ent->m_vel == Vec3D(0,0,0))
	{
		if(npc_pDat->m_curMovAnim >= NPC_STATE_IDLE_MAX)
		{
			npc_pDat->m_idleCurDelay = 0;
			npc_pDat->m_idleDelay = g_gi.RandRngInt(NPCIDLE_DELAY_MIN, NPCIDLE_DELAY_MAX);

			ent->m_obj->MixerClearCycle(npc_pDat->m_movAnim[npc_pDat->m_curMovAnim], 0.3f);
			npc_pDat->m_curMovAnim = NPC_STATE_IDLE_0;
			ent->m_obj->MixerBlendCycle(npc_pDat->m_movAnim[npc_pDat->m_curMovAnim], 1.0f, 0);
		}
		else if(npc_pDat->m_curMovAnim == NPC_STATE_IDLE_0)
		{
			npc_pDat->m_idleCurDelay += tick;
			if(npc_pDat->m_idleCurDelay > npc_pDat->m_idleDelay)
			{
				ent->m_obj->MixerClearCycle(npc_pDat->m_movAnim[npc_pDat->m_curMovAnim], 0.3f);
				npc_pDat->m_curMovAnim = NPC_STATE_IDLE_1;
				ent->m_obj->MixerBlendCycle(npc_pDat->m_movAnim[npc_pDat->m_curMovAnim], 1.0f, 0);
			}
		}
		else
		{
			f32 animTLeft = ent->m_obj->MixerGetDuration()-ent->m_obj->MixerGetTime();
			if(animTLeft <= 0.300f)
			{
				npc_pDat->m_idleCurDelay = 0;
				npc_pDat->m_idleDelay = g_gi.RandRngFloat(NPCIDLE_DELAY_MIN, NPCIDLE_DELAY_MAX);

				ent->m_obj->MixerClearCycle(npc_pDat->m_movAnim[npc_pDat->m_curMovAnim], 0.3f);
				npc_pDat->m_curMovAnim = NPC_STATE_IDLE_0;
				ent->m_obj->MixerBlendCycle(npc_pDat->m_movAnim[npc_pDat->m_curMovAnim], 1.0f, 0);
			}
		}
	}
	else if(npc_pDat->m_curMovAnim != NPC_STATE_WALK)
	{
		ent->m_obj->MixerClearCycle(npc_pDat->m_movAnim[npc_pDat->m_curMovAnim], 0.3f);
		ent->m_obj->MixerBlendCycle(npc_pDat->m_movAnim[NPC_STATE_WALK], 1.0f, 0);

		npc_pDat->m_curMovAnim = NPC_STATE_WALK;
	}

	return TRUE;
}

s32 NPC_Destroy(hENTITY ent)
{
	npc_pDat = (NPCDat *)ent->m_usrDat;
	
	if(npc_pDat)
	{
		g_gi.MemFree(npc_pDat);
	}

	return TRUE;
}

static EntityVFTable npc_vftbl = {NPC_Init, NPC_Update, 0, NPC_Destroy, 0,0,0,0};

#define NPC_SPAWNFLAG_SHADOW		1
#define NPC_SPAWNFLAG_PUSH			2

void Spawn_NPC(const EntityParse & entDat)
{
	const tCHAR *val, *name, *datName;

	name = entDat.GetVal(L"name");
	datName = entDat.GetVal(L"type");

	hENTITY newEnt = GameEntityLoad(name, ENTITY_NPC, datName, npc_vftbl);

	if(newEnt)
	{
		s32 flags=0;

		//initialize other stuff from the entity spawn
		NPCDat *npc_pDat = (NPCDat *)newEnt->m_usrDat;

		//get point
		val = entDat.GetVal(L"origin");
		if(val)
		{
			//swap at the same time, the y and z
			swscanf(val, L"%f %f %f", 
				&newEnt->m_loc.x, &newEnt->m_loc.z, &newEnt->m_loc.y);

			newEnt->m_loc.z *= -1;
		}

		//set direction
		val = entDat.GetVal(L"angle");
		if(val)
		{
			Matrix mtx;
			Vec3D edir(1,0,0);
			f32 angle;
			swscanf(val, L"%f", &angle);
			
			angle = TO_RAD(angle);

			g_gi.MtxRotateY(angle, &mtx);
			g_gi.Vec3DTransformNormal(&edir, &mtx, &newEnt->m_dir);
		}

		//set radius
		val = entDat.GetVal(L"radius");
		if(val)
		{
			swscanf(val, L"%f", &npc_pDat->radius);
			npc_pDat->radius *= npc_pDat->radius;
		}

		//set waypoint
		val = entDat.GetVal(L"target");
		if(val)
		{
			npc_pDat->wp = g_gi.WPNodeCreate(val);
		}

		val = entDat.GetVal(L"flags");
		if(val)
			swscanf(val, L"%d", &flags);

		//add shadow?
		if(TESTFLAGS(flags, NPC_SPAWNFLAG_SHADOW))
		{
			f32 shadT=2;
			g_gi.ValueGetFloat(VALUE_SHADOW_TYPE, &shadT);
			newEnt->SetShadowType((eEntShadow)((u32)shadT));

			if(newEnt->m_obj)
				newEnt->m_obj->ReceiveShadow(FALSE);
		}

		if(TESTFLAGS(flags, NPC_SPAWNFLAG_PUSH))
			newEnt->m_flag |= ENTITY_FLAG_PUSHABLE;

		g_gi.WorldAddEntity(newEnt);
		newEnt->Release(); //let the world take care of its death
	}
}