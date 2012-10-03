#include "game_teamplayer.h"
#include "game_match.h"
#include "game_steering.h"
#include "game_team.h"
#include "game_local.h"
#include "game_lua.h"

/// @brief Structure used to represent team player scout entity data
struct tpSD {
	hENTITY mSelf;	///< Team player scout entity

	tpSD (hENTITY ent) : mSelf(ent) {}

	void Scout (hQBSP qbsp, float time);
};

tCHAR *g_TPanimStr[TP_STATE_MAX]={L"idle0", L"idle1", L"idle2", L"idle3", L"idle4",
								L"walkF", L"walkstrafeR", L"walkstrafeRF", L"walkstrafeRB",
								L"walkstrafeL", L"walkstrafeLF",L"walkstrafeLB", L"walkB",
								L"runF", L"runstrafeR", L"runstrafeRF", L"runstrafeRB",
								L"runstrafeL", L"runstrafeLF", L"runstrafeLB", L"runB",
								L"downed", L"scareU", L"scareD"};
tCHAR *g_spdStr[SPEED_MAX]={L"walkF", L"walkstrafe", L"walkB",
							L"runF", L"runstrafe", L"runB"};
tCHAR *g_animActStr[TP_ACTION_MAX]={L"serve",
									L"shootJL", L"shootUL", L"shootDL",
									L"shootJR", L"shootUR", L"shootDR",
									L"hitU", L"hitM", L"hitD",
									L"dropdown", L"rise",
									L"winpoint0", L"winpoint1", L"winpoint2",
									L"winmatch0", L"winmatch1", L"winmatch2",
									L"taunt0", L"taunt1", L"taunt2" };

#define ANIM_SEC		(L"anim")
#define SPEED_SEC		(L"speed")
#define CONV_SEC		(L"conv")
#define CAMERA_SEC		(L"camera")
#define SOUND_SEC		(L"sound")

#define TP_DELAY_IDLE 6.0f

#define TPIDLE_DELAY_MIN	5.000f
#define TPIDLE_DELAY_MAX	10.000f

TPDat::TPDat (void) : mPlayer(0), mProc(LUA_REFNIL), mMotion(0.0f, 0.0f, 0.0f)
{
	Lua::class_New(g_game.mLua, "Steering", 0);	// steering

	mSteering = luaL_ref(g_game.mLua, LUA_REGISTRYINDEX);
}

TPDat::~TPDat (void)
{
	luaL_unref(g_game.mLua, LUA_REGISTRYINDEX, mSteering);
	luaL_unref(g_game.mLua, LUA_REGISTRYINDEX, mProc);
}

void TPDat::StopSound (void)
{
	if (m_soundCur != -1) g_gi.SoundStop(m_soundCur);

	m_soundCur = -1;
}

void TPDat::PlaySound (u32 index)
{
	if (m_soundCur != -1) g_gi.SoundStop(m_soundCur);

	m_soundCur = g_gi.SoundPlay(m_soundArray[index], Vec3D(0.0f, 0.0f, 0.0f));
}

// utility function for removing any cycle animation on an entity
void TP_ClearMovStates(hENTITY ent)
{
	TPDat *TP_pDat = (TPDat *)ent->m_usrDat;

	if(ent->m_obj)
	{
		for(u8 i = 0; i < TP_STATE_MAX; i++)
		{
			ent->m_obj->MixerClearCycle(TP_pDat->m_movAnim[i], 0.2f);
		}
	}
}

s32 TP_Init(hENTITY ent, void *initDat)
{
	ent->m_usrDat = new TPDat;

	ent->m_obj->GetAABBWorld(&ent->m_boundMin, &ent->m_boundMax);

	Vec3D diff = ent->m_boundMax - ent->m_boundMin;

	for (int index = 0; index < 3; ++index) ent->m_boundRadius = diff[index] > ent->m_boundRadius ? diff[index] : ent->m_boundRadius;

	return TRUE;
}

s32 TP_Update(hENTITY ent)
{
	f32 tick = g_gi.EngineGetTick();

	TPDat * tp_pDat = (TPDat *)ent->m_usrDat;

	tp_pDat->mPlayer->updateTime(tick);

	// Execute update logic.
	Lua::Resume(g_game.mLua, tp_pDat->mProc);

	// Steer the player. If this is the controlled player, ignore during play.
	if (tp_pDat->mPlayer != g_game.player || !Match::getMe()->InPlay())
	{
		// Find neighboring players on team.
		float range2 = tp_pDat->mNeighborRange * tp_pDat->mNeighborRange;

		std::list<hENTITY> neighbors;

		Team * pTeam = tp_pDat->mPlayer->GetTeam();

		for (u8 player = 0; player < pTeam->GetSize(); ++player)
		{
			hENTITY nEnt = pTeam->getPlayer(player)->GetEnt();

			Vec3D diff = ent->m_loc - nEnt->m_loc;

			if (ent != nEnt && Vec3DLenSQ(&diff) < range2) neighbors.push_back(nEnt);
		}

		// Compute player motion.
		ent->m_vel = Lua::_toclassT<Steering>(g_game.mLua, tp_pDat->mSteering)->GetForce(ent, neighbors, g_gi.WorldGetMap()) * g_game.mPhysicsStep;

		tp_pDat->mMotion = ent->m_vel;

		// Orient AI player.
		if (Vec3DLenSQ(&ent->m_vel) > 1e-6f)
		{
			Vec3D vU;	g_gi.Vec3DNormal(&ent->m_vel, &vU);

			Vec3D up;	Vec3DCross(&ent->m_dir, &vU, &up);

			float angle = acosf(Vec3DDot(&vU, &ent->m_dir));

			if (up.y < 0.0f)
			{
				tp_pDat->m_moveNext = TP_STATE_RUNSTRAFEL;
				angle = -angle;
			}

			else tp_pDat->m_moveNext = TP_STATE_RUNSTRAFER;

			if (fabsf(angle) < float(PI) / 6) tp_pDat->m_moveNext = TP_STATE_RUNF;

			tp_pDat->mPlayer->Rotate(angle);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////
	// characer is not moving = idle
	if(Vec3DLenSQ(&ent->m_vel) < 1e-6f && tp_pDat->m_moveNext != TP_STATE_DOWNED)
	{

		//* BEGIN new idle control

		// entity was moving before, it has just stopped
		if(tp_pDat->m_move > TP_STATE_IDLE5 || tp_pDat->m_idleCurDelay > tp_pDat->m_idleDelay){

			// reset current idle counter and determine new delay idle
			tp_pDat->m_idleCurDelay = 0;
			tp_pDat->m_idleDelay = (f32)g_gi.RandRngInt(s32(TPIDLE_DELAY_MIN), s32(TPIDLE_DELAY_MAX));

			// generate random number to determine which idle to play
			u16 idlerandnum = g_gi.RandRngInt(1, 100);

			// stop playing previous animation
			ent->m_obj->MixerClearCycle(tp_pDat->m_movAnim[tp_pDat->m_move], 0.3f);

			// set normal idle
			if(idlerandnum <= 70){
				tp_pDat->m_move = TP_STATE_IDLE1;
			// set different idle 
			}else if(idlerandnum <= 96){
				// generate another random number to determine which different idle to set
				idlerandnum = g_gi.RandRngInt(2, 4);
				if(idlerandnum == 2)
                    tp_pDat->m_move = TP_STATE_IDLE2;
				else if(idlerandnum == 3)
                    tp_pDat->m_move = TP_STATE_IDLE3;
				else
                    tp_pDat->m_move = TP_STATE_IDLE4;
			// set rare idle
			}else{
				tp_pDat->m_move = TP_STATE_IDLE5;
			}

			// play idle
			ent->m_obj->MixerBlendCycle(tp_pDat->m_movAnim[tp_pDat->m_move], 0.5f, 0);
			
		// update idle counter
		}else{
			tp_pDat->m_idleCurDelay += tick;
		}

		// END new idle control //*/

	// character is moving
	}else{
		// check to see if current state is different than next one,
		// if so... update it and act accordingly
		if(tp_pDat->m_move != tp_pDat->m_moveNext){
			
			switch(tp_pDat->m_moveNext){
				
				//////////////
				// walk

				case TP_STATE_WALKF:
				case TP_STATE_WALKSTRAFEL:
				case TP_STATE_WALKSTRAFER:
				case TP_STATE_WALKB:
					break;

				///////////////
				// run

				case TP_STATE_RUNF:

					// clear out all animations
					TP_ClearMovStates(ent);
					// play animation
					ent->m_obj->MixerBlendCycle(tp_pDat->m_movAnim[TP_STATE_RUNF], 1.0f, 0.0f);
					// play sound
					tp_pDat->PlaySound(TP_STATE_RUNF);
					// update speed
					ent->m_spd = tp_pDat->m_speed[SPEED_RUNF];
					break;

				case TP_STATE_RUNSTRAFELF:

					TP_ClearMovStates(ent);
					// play animation
					ent->m_obj->MixerBlendCycle(tp_pDat->m_movAnim[TP_STATE_RUNSTRAFELF], 1.0f, 0.0f);
					// play sound
					tp_pDat->PlaySound(TP_STATE_RUNSTRAFEL);
					// update speed
					ent->m_spd = tp_pDat->m_speed[SPEED_RUNSTRAFE];
					break;

				case TP_STATE_RUNSTRAFELB:

					TP_ClearMovStates(ent);
					// play animation
					ent->m_obj->MixerBlendCycle(tp_pDat->m_movAnim[TP_STATE_RUNSTRAFELB], 1.0f, 0.0f);
					// play sound
					tp_pDat->PlaySound(TP_STATE_RUNSTRAFEL);
					// update speed
					ent->m_spd = tp_pDat->m_speed[SPEED_RUNB];
					break;

				case TP_STATE_RUNSTRAFEL:

					TP_ClearMovStates(ent);
					// play animation
					ent->m_obj->MixerBlendCycle(tp_pDat->m_movAnim[TP_STATE_RUNSTRAFEL], 1.0f, 0.0f);
					// play sound
					tp_pDat->PlaySound(TP_STATE_RUNSTRAFEL);
					// update speed
					ent->m_spd = tp_pDat->m_speed[SPEED_RUNSTRAFE];
					break;

				case TP_STATE_RUNSTRAFERF:

					TP_ClearMovStates(ent);
					// play animation
					ent->m_obj->MixerBlendCycle(tp_pDat->m_movAnim[TP_STATE_RUNSTRAFERF], 1.0f, 0.0f);
					// play sound
					tp_pDat->PlaySound(TP_STATE_RUNSTRAFER);
					// update speed
					ent->m_spd = tp_pDat->m_speed[SPEED_RUNSTRAFE];
					break;

				case TP_STATE_RUNSTRAFERB:

					TP_ClearMovStates(ent);
					// play animation
					ent->m_obj->MixerBlendCycle(tp_pDat->m_movAnim[TP_STATE_RUNSTRAFERB], 1.0f, 0.0f);
					// play sound
					tp_pDat->PlaySound(TP_STATE_RUNSTRAFER);
					// update speed
					ent->m_spd = tp_pDat->m_speed[SPEED_RUNB];
					break;

				case TP_STATE_RUNSTRAFER:

					TP_ClearMovStates(ent);
					// play animation
					ent->m_obj->MixerBlendCycle(tp_pDat->m_movAnim[TP_STATE_RUNSTRAFER], 1.0f, 0.0f);
					// play sound
					tp_pDat->PlaySound(TP_STATE_RUNSTRAFER);
					// update speed
					ent->m_spd = tp_pDat->m_speed[SPEED_RUNSTRAFE];
					break;

				case TP_STATE_RUNB:

					TP_ClearMovStates(ent);
					// play animation
					ent->m_obj->MixerBlendCycle(tp_pDat->m_movAnim[TP_STATE_RUNB], 1.0f, 0.0f);
					// play sound
					tp_pDat->PlaySound(TP_STATE_RUNB);
					// update speed
					ent->m_spd = tp_pDat->m_speed[SPEED_RUNB];
					break;

				case TP_STATE_DOWNED:

					TP_ClearMovStates(ent);
					// play animation
					ent->m_obj->MixerBlendCycle(tp_pDat->m_movAnim[TP_STATE_DOWNED], 1.0f, 0.0f);
					break;

				//////////
				// scares

				case TP_STATE_SCAREU:
				case TP_STATE_SCARED:
					break;

			}
			// update move
			tp_pDat->m_move = tp_pDat->m_moveNext;
		}
	}

	return TRUE;
}

s32 TP_Destroy(hENTITY ent)
{
	delete (TPDat *)ent->m_usrDat;

	return TRUE;
}

static EntityVFTable tp_vftbl = {TP_Init, TP_Update, 0, TP_Destroy, 0,0,0,0};

#define TP_SPAWNFLAG_SHADOW		1
#define TP_SPAWNFLAG_PUSH		2

void Spawn_TP(const EntityParse & entDat)
{
	hENTITY newEnt;

	const tCHAR *val;

	tCHAR name[10];

	swprintf(name, L"t%sp%s", entDat.GetVal(L"side_id"), entDat.GetVal(L"pos_id"));

	// Get corresponding filename
	tCHAR cfgfilename[100];
	const tCHAR *teamsideid = entDat.GetVal(L"side_id");
	const tCHAR *playerposid = entDat.GetVal(L"pos_id");
	// ... and add this ent to corresponding player
	if(wcsicmp(teamsideid, L"1") == 0){
		swprintf(cfgfilename, Match::getMe()->getTeam(1)->getPlayerMappedCFGFile(playerposid));
	}else if(wcsicmp(teamsideid, L"2") == 0){
		swprintf(cfgfilename, Match::getMe()->getTeam(2)->getPlayerMappedCFGFile(playerposid));
	}

	// load Entity
	newEnt = GameEntityLoad(name, ENTITY_TP, cfgfilename, tp_vftbl);

	if(newEnt)
	{
		//initialize other stuff from the entity spawn
		TPDat *tp_pDat = (TPDat *)newEnt->m_usrDat;

		// get team side and position ids
		tp_pDat->teamsideid = entDat.GetVal(L"side_id");
		tp_pDat->playerposid = entDat.GetVal(L"pos_id");

		wstring filepath = L"Data\\Entity\\";
		filepath += cfgfilename;
		filepath += L".ini";

		// getting Animations
		hCFG cfg;
		cfg = g_gi.CfgFileLoad(filepath.c_str()); // pointer to cfg file

		tCHAR buff[MAXCHARBUFF];
		//get animations
		for(u8 i = 0; i < TP_STATE_MAX; i++)
		{
			if(cfg->GetItemStr(ANIM_SEC, g_TPanimStr[i], buff, MAXCHARBUFF))
				tp_pDat->m_movAnim[i] = newEnt->MDLGetAnimID(buff);
			else
				tp_pDat->m_movAnim[i] = -1;
		}
		
		tp_pDat->m_pauseCurDelay = tp_pDat->m_pauseDelay = 1.000f;
		tp_pDat->m_dirCurDelay = tp_pDat->m_dirDelay = 0.250f;

		tp_pDat->m_idleDelay = g_gi.RandRngFloat(TPIDLE_DELAY_MIN, TPIDLE_DELAY_MAX);

		//set initial animations
		tp_pDat->m_move = TP_STATE_IDLE1;
		
		newEnt->m_obj->MixerBlendCycle(tp_pDat->m_movAnim[tp_pDat->m_move], 1.0f, 0);


		// get speeds
		for(u8 i = 0; i < SPEED_MAX; i++)
		{
			if(cfg->GetItemInt(SPEED_SEC, g_spdStr[i]))
				tp_pDat->m_speed[i] = 3 * (f32)cfg->GetItemInt(SPEED_SEC, g_spdStr[i]);
			else
				tp_pDat->m_speed[i] = 0;
		}

		//get sounds
		for(i = 0; i < TP_STATE_MAX; i++)
		{
			tp_pDat->m_soundArray[i] = -1;

			if(cfg->GetItemStr(SOUND_SEC, g_TPanimStr[i], buff, MAXCHARBUFF))
				g_gi.SoundSysGetID(buff, &tp_pDat->m_soundArray[i]);
		}

		tp_pDat->m_soundCur = -1;

		//action animations
		for(i = 0; i < TP_ACTION_MAX; i++)
		{
			if(cfg->GetItemStr(ANIM_SEC, g_animActStr[i], buff, MAXCHARBUFF))
			{
				if(newEnt->m_obj && newEnt->m_obj->GetModel())
					tp_pDat->m_animAction[i] = newEnt->m_obj->GetModel()->GetAnimID(buff);
			}
			else
				tp_pDat->m_animAction[i] = -1;
		}


		//get walk<->run delay
		tp_pDat->m_runDelay = cfg->GetItemFloat(SPEED_SEC, L"runDelay");

		//hardcoded values :))
		tp_pDat->m_idleDelay = TP_DELAY_IDLE;


		s32 flags=0;

		//get point
		val = entDat.GetVal(L"origin");
		if(val)
		{
			//swap at the same time, the y and z
			swscanf(val, L"%f %f %f", 
				&newEnt->m_loc.x, &newEnt->m_loc.z, &newEnt->m_loc.y);

			newEnt->m_loc.z *= -1;

			static_cast<TPDat*>(newEnt->m_usrDat)->mOrigin = newEnt->m_loc;
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

		//get look-at offset
		if(cfg->GetItemStr(CAMERA_SEC, L"lookatofs", buff, MAXCHARBUFF))
			swscanf(buff, L"%f,%f,%f", &tp_pDat->m_lookatOfs.x, &tp_pDat->m_lookatOfs.y, &tp_pDat->m_lookatOfs.z);
		
		//get camera distance
		tp_pDat->m_lookDist = cfg->GetItemFloat(CAMERA_SEC, L"lookdist");

		val = entDat.GetVal(L"flags");
		if(val)
			swscanf(val, L"%d", &flags);

		// set shadow and pushable flags
		f32 shadT=0;
		g_gi.ValueGetFloat(VALUE_SHADOW_TYPE, &shadT);
		newEnt->SetShadowType((eEntShadow)((u32)shadT));
		if(newEnt->m_obj)
			newEnt->m_obj->ReceiveShadow(FALSE);
		newEnt->m_flag |= ENTITY_FLAG_PUSHABLE;

		g_gi.WorldAddEntity(newEnt);

		// ... and add this ent to corresponding team player
		if(wcsicmp(teamsideid, L"1") == 0){
			Match::getMe()->getTeam(1)->setEntToPlayerMapped(playerposid, newEnt);
			// (rodo) hardcodded settings TODO: FIX THIS!!
			// set player until map is loaded, hence entity spawned
			// initialize camera (set current controllable TP to game module)
			//* comment this to select team 1
			if(wcsicmp(playerposid, L"1") == 0){
				g_game.localteam = Match::getMe()->getTeam(1);
				g_game.player = Match::getMe()->getTeam(1)->getPlayerMapped(L"1");
				Match::getMe()->getTeam(1)->getPlayerMapped(L"1")->setType(TEAMPLAYERTYPE_HUMANLOCAL);
			}//*/
		}else if(wcsicmp(teamsideid, L"2") == 0){
			Match::getMe()->getTeam(2)->setEntToPlayerMapped(playerposid, newEnt);
			/*/ comment this to select team 2
			if(wcsicmp(playerposid, L"1") == 0){
				g_game.localteam = Match::getMe()->getTeam(2);
				g_game.player = Match::getMe()->getTeam(2)->getPlayerMapped(L"1");
				Match::getMe()->getTeam(2)->getPlayerMapped(L"1")->setType(TEAMPLAYERTYPE_HUMANLOCAL);
			}//*/
		}

		cfg->Release();
		newEnt->Release(); //let the world take care of its death
	}
}
