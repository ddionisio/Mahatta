#include "game_local.h"

//flags
#define DOODAD_FLAG_WPCURVE		1 //we follow the path, curve style!
#define DOODAD_FLAG_OBJSHADOW	2 //object casts shadow

struct doodadDat {
	hWPNODE		wp;			//the waypoint node
	s32			parID;		//particle ID (0 == none)

	f32			rot;		//accumulated rotation
	f32			rotSpd;		//rotation spd along y-axis (obj) or dir axis (q3bsp mdl)

	f32			delay;		//delay from one waypoint to next

	Vec3D		axis;		//rotation axis (for qbsp models)

	s32			snd;		//looping sound

	FLAGS		flag;		//doodad flags
};

static doodadDat *doodad_pDat;

s32 Doodad_Init(hENTITY ent, void *initDat)
{
	doodad_pDat = (doodadDat *)ent->m_usrDat;

	//allocate door data
	doodad_pDat = (doodadDat *)g_gi.MemAlloc(sizeof(doodadDat));
	ent->m_usrDat = doodad_pDat;

	if(!doodad_pDat)
	{ g_gi.LogMsg(LOG_FILE, L"Doodad_Init", L"Unable to allocate doodad data"); return FALSE; }

	const tCHAR *val, *animStr;

	EntityParse *pEntDat = (EntityParse *)initDat;

	animStr = pEntDat->GetVal(L"anim");

	//get point
	val = pEntDat->GetVal(L"origin");
	if(val)
	{
		//swap at the same time, the y and z
		swscanf(val, L"%f %f %f", 
			&ent->m_loc.x, &ent->m_loc.z, &ent->m_loc.y);

		ent->m_loc.z *= -1;
	}

	//set direction
	val = pEntDat->GetVal(L"angle");
	if(val)
	{
		Matrix mtx;
		Vec3D edir(1,0,0);
		f32 angle;
		swscanf(val, L"%f", &angle);
		
		angle = TO_RAD(angle);

		g_gi.MtxRotateY(angle, &mtx);
		g_gi.Vec3DTransformNormal(&edir, &mtx, &ent->m_dir);
	}

	//set axis
	val = pEntDat->GetVal(L"axis");
	if(val)
	{
		Vec3D aDir;
		g_gi.WorldTargetGet(val, &aDir);
		doodad_pDat->axis = aDir - ent->m_loc;
		g_gi.Vec3DNormal(&doodad_pDat->axis, &doodad_pDat->axis);
	}

	//set rotation speed
	val = pEntDat->GetVal(L"rotSpd");
	if(val)
		swscanf(val, L"%f", &doodad_pDat->rotSpd);
	else
		doodad_pDat->rotSpd = -1; //no silly rotation

	//set waypoint travel delay
	val = pEntDat->GetVal(L"delay");
	if(val)
		swscanf(val, L"%f", &doodad_pDat->delay);

	//set waypoint node
	val = pEntDat->GetVal(L"target");
	if(val)
		doodad_pDat->wp = g_gi.WPNodeCreate(val);

	//get particle
	val = pEntDat->GetVal(L"particle");
	if(val)
		doodad_pDat->parID = g_gi.ParSysStart(val);

	//get sound
	doodad_pDat->snd = -1;
	val = pEntDat->GetVal(L"sound");
	if(val)
	{
		s32 sysID = -1;
		
		g_gi.SoundSysGetID(val, &sysID);

		doodad_pDat->snd = g_gi.SoundPlay(sysID, ent->m_loc);
	}

	//get flags
	val = pEntDat->GetVal(L"flags");
	if(val)
		swscanf(val, L"%d", &doodad_pDat->flag);

	if(ent->m_obj)
	{
		ent->m_obj->GetAABBWorld(&ent->m_boundMin, &ent->m_boundMax);

		if(animStr)
		{
			s32 animID;

			if(ent->m_obj && ent->m_obj->GetModel())
				animID = ent->m_obj->GetModel()->GetAnimID(animStr);
			else
				animID = -1;

			ent->m_obj->MixerBlendCycle(animID, 1.0f, 0);
		}
	}

	//cast shadow?
	f32 shadT=2;
	g_gi.ValueGetFloat(VALUE_SHADOW_TYPE, &shadT);

	if(TESTFLAGS(doodad_pDat->flag, DOODAD_FLAG_OBJSHADOW))
		ent->SetShadowType((eEntShadow)((u32)shadT));

	ent->m_flag = ENTITY_FLAG_IGNORE_COLLIDE | ENTITY_FLAG_NOGRAVITY;

	return TRUE;
}

s32 Doodad_Update(hENTITY ent)
{
	doodad_pDat = (doodadDat *)ent->m_usrDat;

	//update rotation thing
	if(doodad_pDat->rotSpd > 0)
	{
		doodad_pDat->rot += doodad_pDat->rotSpd*g_gi.EngineGetTick();

		if(ent->m_obj)
		{
			Matrix mtx;
			Vec3D edir(1,0,0);
			g_gi.MtxRotateY(doodad_pDat->rot, &mtx);
			g_gi.Vec3DTransformNormal(&edir, &mtx, &ent->m_dir);
		}
		else if(ent->m_mapMdlID != -1)
			g_gi.WorldGetMap()->ModelRotateAxis(ent->m_mapMdlID, doodad_pDat->axis, doodad_pDat->rot);
	}

	//follow waypoint
	if(doodad_pDat->wp && !doodad_pDat->wp->IsDone())
	{
		doodad_pDat->wp->MoveToNext(doodad_pDat->delay);

		//set location (we want curves ;) )
		Vec3D dst;

		if(TESTFLAGS(doodad_pDat->flag, DOODAD_FLAG_WPCURVE))
			doodad_pDat->wp->GetCurCurvedLoc(&dst);
		else
			doodad_pDat->wp->GetCurLinearLoc(&dst);

		//now move it!
		ent->m_vel = dst - ent->m_loc;

		//we only do this if rotSpd is not available
		if(doodad_pDat->rotSpd < 0)
		{
			if(ent->m_obj)
				doodad_pDat->wp->GetCurLinearDir(&ent->m_dir);
			else if(ent->m_mapMdlID != -1)
				g_gi.WorldGetMap()->ModelRotateAxis(ent->m_mapMdlID, doodad_pDat->axis, doodad_pDat->wp->GetCurLinearAngle());
		}
	}

	//update particle
	if(doodad_pDat->parID)
	{
		//update particle
		gfxParInfo pi;
		pi.emitter_pos = ent->m_loc;
		//pi.emitter_pos.y += 30;
		pi.disable = FALSE;

		g_gi.ParSysUpdate(doodad_pDat->parID, &pi);

		//set bound rect
		if(!ent->m_obj && !ent->m_mapMdlID)
		{
			g_gi.ParSysGetBound(doodad_pDat->parID, &ent->m_boundMin, &ent->m_boundMax);
		}
	}

	//update sound
	if(doodad_pDat->snd != -1)
		g_gi.SoundSet(doodad_pDat->snd, ent->m_loc, ent->m_vel);

	return TRUE;
}

s32 Doodad_Render(hENTITY ent, const Vec3D & eyePt)
{
	doodad_pDat = (doodadDat *)ent->m_usrDat;

	if(doodad_pDat->parID)
		g_gi.ParSysRender(doodad_pDat->parID);

	return TRUE;
}

s32 Doodad_Destroy(hENTITY ent)
{
	doodad_pDat = (doodadDat *)ent->m_usrDat;

	if(doodad_pDat)
	{
		if(doodad_pDat->wp)
			doodad_pDat->wp->Release();

		if(doodad_pDat->snd != -1) g_gi.SoundStop(doodad_pDat->snd);

		g_gi.MemFree(doodad_pDat);
	}

	return TRUE;
}

static EntityVFTable doodad_vftbl = {Doodad_Init, Doodad_Update, Doodad_Render, Doodad_Destroy, 0,0,0,0};

void Spawn_Doodad(const EntityParse & entDat)
{
	const tCHAR *mdlStr=0;
	Vec3D loc(0,0,0), dir(0,0,0);

	hENTITY newEnt;

	mdlStr = entDat.GetVal(L"model");

	newEnt = g_gi.EntCreate(L"doodad", ENTITY_DOODAD, mdlStr, doodad_vftbl, (void*)&entDat);

	if(newEnt)
	{
		g_gi.WorldAddEntity(newEnt);
		newEnt->Release(); //let the world take care of its death
	}
}