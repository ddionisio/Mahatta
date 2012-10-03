#include "game_local.h"
#include "trajectory.h"

#define PLYR_DELAY_IDLE 6.0f

tCHAR *g_animStr[PLYR_STATE_MAX]={L"idle0", L"idle1", L"idle2", L"walk", L"run", L"bwalk", L"strafeL", L"strafeR"};
tCHAR *g_spdStr[SPEED_MAX]={L"walk", L"run", L"strafe", L"bwalk"};
tCHAR *g_animActStr[PLYR_ACTION_MAX]={L"shoot", L"ball"};

#define ANIM_SEC		(L"anim")

#define SOUND_SEC		(L"sound")

#define SPEED_SEC		(L"speed")

#define VIEW_SEC		(L"view")

struct PlyrDat {
	s32			m_movAnim[PLYR_STATE_MAX];	//the animation IDs for each state

	f32			m_speed[SPEED_MAX];		//various speeds

	s32			m_animAction[PLYR_ACTION_MAX];	//the animation IDs for each action

	Vec3D		m_lookatOfs;		//the look-at offset
	f32			m_lookDist;			//how far do we look away

	PlyrMove	m_move;				//current movement
	PlyrMove	m_moveNext;			//next movement (m_move is set to this on move update)

	PlyrMovAnim m_curMovAnim;		//current movement animation (used only by idle)

	f32			m_runDelay;			//delay from walk<->run
	f32			m_runCurDelay;		//current counter delay from walk<->run

	f32			m_idleCurDelay;
	f32			m_idleDelay;

	s32			m_soundArray[PLYR_STATE_MAX]; //array that contains all the ids of the sounds loaded
	s32			m_soundCur;	///< Sound that is being played

	void StopSound (void);
	void PlaySound (u32 index);

	s32			 m_bulletFireSnd;
	hENTITY		*m_bullets;
	s32			 m_curBullet;		//current available bullet
	s32			 m_numBullets;

	hENTITY mBalls[5];
};

static PlyrDat *plyr_pDat;

void PlyrDat::StopSound (void)
{
	if (-1 == m_soundCur) return;
	
	g_gi.SoundStop(m_soundCur);

	m_soundCur = -1;
}

void PlyrDat::PlaySound (u32 index)
{
	if (m_soundCur != -1) g_gi.SoundStop(m_soundCur);

	m_soundCur = g_gi.SoundPlay(m_soundArray[index], Vec3D(0.0f, 0.0f, 0.0f));
}

//set the next movement
static inline void _PlayerMoveNext(hENTITY ent)
{
	switch(plyr_pDat->m_moveNext)
	{
	case PLYR_MOVE_IDLE:
		if(plyr_pDat->m_move != plyr_pDat->m_moveNext)
		{
			//clear out all animations
			g_game.player->ClearMovStates();

			//initialize to the first idle
			plyr_pDat->m_curMovAnim = PLYR_STATE_IDLE_0;
			plyr_pDat->m_move = plyr_pDat->m_moveNext = PLYR_MOVE_IDLE;
			ent->m_obj->MixerBlendCycle(plyr_pDat->m_movAnim[plyr_pDat->m_curMovAnim], 1.0f, 0.0f);

			plyr_pDat->m_idleCurDelay = 0;

			plyr_pDat->StopSound();

		}
		break;

	case PLYR_MOVE_WALK:
		if(plyr_pDat->m_move == PLYR_MOVE_RUNTOWALK)
			return;



		if(plyr_pDat->m_move == PLYR_MOVE_RUN)
		{
			plyr_pDat->m_moveNext = PLYR_MOVE_RUNTOWALK;

			//set the walk<->run delay counter
			plyr_pDat->m_runCurDelay = 0;

			//clear out all animations
			g_game.player->ClearMovStates();

			plyr_pDat->PlaySound(PLYR_STATE_WALK);

		}
		else if(plyr_pDat->m_move != PLYR_MOVE_WALK)
		{
			//clear out all animations
			g_game.player->ClearMovStates();

			ent->m_obj->MixerBlendCycle(plyr_pDat->m_movAnim[PLYR_STATE_WALK], 1.0f, 0);
			
			plyr_pDat->PlaySound(PLYR_STATE_WALK);
		}
		else
			return;

		break;

	case PLYR_MOVE_RUN:
		if(plyr_pDat->m_move != PLYR_MOVE_RUN && plyr_pDat->m_move != PLYR_MOVE_WALKTORUN)
		{
			plyr_pDat->m_moveNext = PLYR_MOVE_WALKTORUN;

			//set the walk<->run delay counter
			plyr_pDat->m_runCurDelay = 0;

			//clear out all animations
			g_game.player->ClearMovStates();

			plyr_pDat->PlaySound(PLYR_STATE_RUN);
		}
		else
			return;

		break;

	case PLYR_MOVE_STRAFE_L:
		/*if(plyr_pDat->m_move == PLYR_MOVE_RUN 
			|| plyr_pDat->m_move == PLYR_MOVE_WALK
			|| plyr_pDat->m_move == PLYR_MOVE_WALKTORUN
			|| plyr_pDat->m_move == PLYR_MOVE_RUNTOWALK
			|| plyr_pDat->m_move == PLYR_MOVE_BWALK)
			return;*/

		if(plyr_pDat->m_move != plyr_pDat->m_moveNext)
		{
			//clear out all animations
			g_game.player->ClearMovStates();

			ent->m_obj->MixerBlendCycle(plyr_pDat->m_movAnim[PLYR_STATE_STRAFE_L], 1.0f, 0);

			plyr_pDat->PlaySound(PLYR_STATE_STRAFE_L);
		}
		break;

	case PLYR_MOVE_STRAFE_R:
		if(plyr_pDat->m_move != plyr_pDat->m_moveNext)
		{
			//clear out all animations
			g_game.player->ClearMovStates();

			ent->m_obj->MixerBlendCycle(plyr_pDat->m_movAnim[PLYR_STATE_STRAFE_R], 1.0f, 0);

			plyr_pDat->PlaySound(PLYR_STATE_STRAFE_R);
		}
		break;

	case PLYR_MOVE_BWALK:
		if(plyr_pDat->m_move != plyr_pDat->m_moveNext)
		{
			//clear out all animations
			g_game.player->ClearMovStates();

			ent->m_obj->MixerBlendCycle(plyr_pDat->m_movAnim[PLYR_STATE_BWALK], 1.0f, 0);

			plyr_pDat->PlaySound(PLYR_STATE_BWALK);
		}
		break;

	case PLYR_MOVE_WALKTORUN:
	case PLYR_MOVE_RUNTOWALK:
		return;
	}

	plyr_pDat->m_move = plyr_pDat->m_moveNext;
}

static inline void _PlayerUpdateMove(hENTITY ent)
{
	f32 tick = g_gi.EngineGetTick();
	f32 t=0;

	switch(plyr_pDat->m_move)
	{
	case PLYR_MOVE_IDLE:
		if(plyr_pDat->m_idleCurDelay < plyr_pDat->m_idleDelay)
		{
			plyr_pDat->m_idleCurDelay += tick;
			if(plyr_pDat->m_idleCurDelay >= plyr_pDat->m_idleDelay)
			{
				ent->m_obj->MixerClearCycle(plyr_pDat->m_movAnim[plyr_pDat->m_curMovAnim], 0.3f);

				//set to another random idle
				plyr_pDat->m_curMovAnim = (PlyrMovAnim)g_gi.RandRngInt(1, PLYR_STATE_IDLE_MAX-1);

				plyr_pDat->PlaySound(plyr_pDat->m_curMovAnim);				

				ent->m_obj->MixerBlendCycle(plyr_pDat->m_movAnim[plyr_pDat->m_curMovAnim], 1.0f, 0.0f);
			}
		}
		else
		{
			f32 animTLeft = ent->m_obj->MixerGetDuration()-ent->m_obj->MixerGetTime();
			if(animTLeft <= 0.300f)
			{
				ent->m_obj->MixerClearCycle(plyr_pDat->m_movAnim[plyr_pDat->m_curMovAnim], 0.3f);
				plyr_pDat->m_curMovAnim = PLYR_STATE_IDLE_0;
				ent->m_obj->MixerBlendCycle(plyr_pDat->m_movAnim[plyr_pDat->m_curMovAnim], 1.0f, 0.0f);
				plyr_pDat->m_idleCurDelay = 0;
			}
		}
		break;

	case PLYR_MOVE_WALK:
		ent->m_spd = plyr_pDat->m_speed[SPEED_WALK];
		break;

	case PLYR_MOVE_RUN:
		ent->m_spd = plyr_pDat->m_speed[SPEED_RUN];
		break;

	case PLYR_MOVE_STRAFE_L:
		ent->m_spd = plyr_pDat->m_speed[SPEED_STRAFE];
		break;

	case PLYR_MOVE_STRAFE_R:
		ent->m_spd = plyr_pDat->m_speed[SPEED_STRAFE];
		break;

	case PLYR_MOVE_BWALK:
		ent->m_spd = plyr_pDat->m_speed[SPEED_BWALK];
		break;

	case PLYR_MOVE_WALKTORUN:
		plyr_pDat->m_runCurDelay += tick;

		t = plyr_pDat->m_runCurDelay/plyr_pDat->m_runDelay;

		if(t >= 1)
		{
			t = 1;
			plyr_pDat->m_move = PLYR_MOVE_RUN;

			ent->m_obj->MixerClearCycle(plyr_pDat->m_movAnim[PLYR_STATE_WALK], 0.3f);
			ent->m_obj->MixerBlendCycle(plyr_pDat->m_movAnim[PLYR_STATE_RUN], 1.0f, 0);
		}
		else
		{
			ent->m_obj->MixerBlendCycle(plyr_pDat->m_movAnim[PLYR_STATE_WALK], 1.0f - t, 0);
			ent->m_obj->MixerBlendCycle(plyr_pDat->m_movAnim[PLYR_STATE_RUN], t, 0);
		}

		ent->m_spd = plyr_pDat->m_speed[SPEED_WALK] + t*(plyr_pDat->m_speed[SPEED_RUN]-plyr_pDat->m_speed[SPEED_WALK]);
		break;

	case PLYR_MOVE_RUNTOWALK:
		plyr_pDat->m_runCurDelay += tick;

		t = plyr_pDat->m_runCurDelay/plyr_pDat->m_runDelay;

		if(t >= 1)
		{
			t = 1;
			plyr_pDat->m_move = PLYR_MOVE_WALK;

			ent->m_obj->MixerClearCycle(plyr_pDat->m_movAnim[PLYR_STATE_RUN], 0.3f);
			ent->m_obj->MixerBlendCycle(plyr_pDat->m_movAnim[PLYR_STATE_WALK], 1.0f, 0);
		}
		else
		{
			ent->m_obj->MixerBlendCycle(plyr_pDat->m_movAnim[PLYR_STATE_RUN], 1.0f - t, 0);
			ent->m_obj->MixerBlendCycle(plyr_pDat->m_movAnim[PLYR_STATE_WALK], t, 0);
		}

		ent->m_spd = plyr_pDat->m_speed[SPEED_RUN] + t*(plyr_pDat->m_speed[SPEED_WALK]-plyr_pDat->m_speed[SPEED_RUN]);
		break;
	}
}

s32 Player_Init(hENTITY ent, void *initDat)
{
	plyr_pDat = (PlyrDat *)ent->m_usrDat;

	plyr_pDat = (PlyrDat *)g_gi.MemAlloc(sizeof(PlyrDat));
	ent->m_usrDat = plyr_pDat;

	hCFG cfg = (hCFG)initDat;

	if(cfg)
	{
		u32 i;
		tCHAR buff[MAXCHARBUFF];

		//get animations
		for(i = 0; i < PLYR_STATE_MAX; i++)
		{
			if(cfg->GetItemStr(ANIM_SEC, g_animStr[i], buff, MAXCHARBUFF))
			{
				if(ent->m_obj && ent->m_obj->GetModel())
					plyr_pDat->m_movAnim[i] = ent->m_obj->GetModel()->GetAnimID(buff);
			}
			else
				plyr_pDat->m_movAnim[i] = -1;
		}

		//get sounds
		for(i = 0; i < PLYR_STATE_MAX; i++)
		{
			plyr_pDat->m_soundArray[i] = -1;

			if(cfg->GetItemStr(SOUND_SEC, g_animStr[i], buff, MAXCHARBUFF))
				g_gi.SoundSysGetID(buff, &plyr_pDat->m_soundArray[i]);
		}

		plyr_pDat->m_soundCur = -1;

		//START PLAYING SOME BACKGROUND MUSIC THIS IS HARDCODED FIX ME!
		g_gi.MusicPlay(L"sounds\\music.ogg");

		//speeds
		for(i = 0; i < SPEED_MAX; i++)
		{
			plyr_pDat->m_speed[i] = cfg->GetItemFloat(SPEED_SEC, g_spdStr[i]);

			if(plyr_pDat->m_speed[i] == -1)
				plyr_pDat->m_speed[i] = 100;
		}

		//get walk<->run delay
		plyr_pDat->m_runDelay = cfg->GetItemFloat(SPEED_SEC, L"runDelay");

		//action animations
		for(i = 0; i < PLYR_ACTION_MAX; i++)
		{
			if(cfg->GetItemStr(ANIM_SEC, g_animActStr[i], buff, MAXCHARBUFF))
			{
				if(ent->m_obj && ent->m_obj->GetModel())
					plyr_pDat->m_animAction[i] = ent->m_obj->GetModel()->GetAnimID(buff);
			}
			else
				plyr_pDat->m_animAction[i] = -1;
		}

		//get look-at offset
		if(cfg->GetItemStr(VIEW_SEC, L"lookatofs", buff, MAXCHARBUFF))
			swscanf(buff, L"%f,%f,%f", &plyr_pDat->m_lookatOfs.x, &plyr_pDat->m_lookatOfs.y, &plyr_pDat->m_lookatOfs.z);

		//get view distance
		plyr_pDat->m_lookDist = cfg->GetItemFloat(VIEW_SEC, L"lookdist");

		//hardcoded values :))
		plyr_pDat->m_idleDelay = PLYR_DELAY_IDLE;

		//get bullets
		plyr_pDat->m_numBullets = cfg->GetItemInt(L"bullet", L"maxBullet");

		if(plyr_pDat->m_numBullets > 0)
		{
			plyr_pDat->m_bullets = (hENTITY*)g_gi.MemAlloc(sizeof(hENTITY)*plyr_pDat->m_numBullets);
		}

		tCHAR bulletMDLStr[MAXCHARBUFF];
		bulletInit bi;
		
		bi.maxBounce = cfg->GetItemInt(L"bullet", L"maxBounce");
		cfg->GetItemStr(L"bullet", L"mdl", bulletMDLStr, MAXCHARBUFF);
		bi.spd = cfg->GetItemFloat(L"bullet", L"spd");

		tCHAR sndStr[MAXCHARBUFF];

		cfg->GetItemStr(L"bullet", L"sndFire", sndStr, MAXCHARBUFF);
		g_gi.SoundSysGetID(sndStr, &plyr_pDat->m_bulletFireSnd);

		cfg->GetItemStr(L"bullet", L"sndBounce", sndStr, MAXCHARBUFF);
		g_gi.SoundSysGetID(sndStr, &bi.sndBounce);

		cfg->GetItemStr(L"bullet", L"sndExplode", sndStr, MAXCHARBUFF);
		g_gi.SoundSysGetID(sndStr, &bi.sndExplode);

		for(s32 b = 0; b < plyr_pDat->m_numBullets; b++)
		{
			plyr_pDat->m_bullets[b] = BulletCreate(bulletMDLStr, bi);
		}

		ballInit ballI;

		ballI.mSndBounce = bi.sndBounce;
		ballI.mSndExplode = bi.sndExplode;

		for (Uint32 index = 0; index < sizeof(plyr_pDat->mBalls) / sizeof(plyr_pDat->mBalls[0]); ++index)
		{
			plyr_pDat->mBalls[index] = BallCreate(bulletMDLStr, ballI);
		}
	}

	return TRUE;
}

s32 Player_Update(hENTITY ent)
{
	plyr_pDat = (PlyrDat *)ent->m_usrDat;

	Vec3D zeroV(0,0,0);

	//update the movement
	_PlayerMoveNext(ent);
	_PlayerUpdateMove(ent);

	if(ent->m_vel == zeroV)
	{
		g_game.player->Move(PLYR_MOVE_IDLE);
	}

	return TRUE;
}

s32 Player_Destroy(hENTITY ent)
{
	plyr_pDat = (PlyrDat *)ent->m_usrDat;

	if(plyr_pDat)
	{
		if(plyr_pDat->m_bullets)
		{
			for(s32 b = 0; b < plyr_pDat->m_numBullets; b++)
			{
				if(plyr_pDat->m_bullets[b]) plyr_pDat->m_bullets[b]->Release();
			}

			g_gi.MemFree(plyr_pDat->m_bullets);
		}

		for (Uint32 index = 0; index < sizeof(plyr_pDat->mBalls) / sizeof(plyr_pDat->mBalls[0]); ++index)
		{
			if (plyr_pDat->mBalls[index] != 0) plyr_pDat->mBalls[index]->Release();
		}

		g_gi.MemFree(plyr_pDat);
	}

	return TRUE;
}

static EntityVFTable player_vftbl = { Player_Init, Player_Update, 0, Player_Destroy, 0,0,0,0};

Player::Player(const tCHAR *name, const tCHAR *datName) : m_tgtSpawnStr(L"\0")
{
	m_ent = GameEntityLoad(name, ENTITY_PLAYER, datName, player_vftbl);

	PlyrDat *plyr_pDat = (PlyrDat *)m_ent->m_usrDat;

	f32 shadT=2;
	g_gi.ValueGetFloat(VALUE_SHADOW_TYPE, &shadT);
	m_ent->SetShadowType((eEntShadow)((u32)shadT));

	if(m_ent->m_obj)
	{
		m_ent->m_obj->ReceiveShadow(FALSE);

		//set to idle
		plyr_pDat->m_move = PLYR_MOVE_WALK;
		Move(PLYR_MOVE_IDLE);
	}
}

Player::~Player()
{
	if(m_ent) m_ent->Release();
}

//set the movement
void Player::Move(PlyrMove move)
{
	PlyrDat *plyr_pDat = (PlyrDat *)m_ent->m_usrDat;

	plyr_pDat->m_moveNext = move;
}

void Player::PerformAction(PlyrAction a)
{
	PlyrDat *plyr_pDat = (PlyrDat *)m_ent->m_usrDat;

	if(m_ent->m_obj)
		m_ent->m_obj->MixerExecuteAction(plyr_pDat->m_animAction[a], 0.3f, 0.3f, 1.0f, FALSE);

	switch(a)
	{
	case PLYR_ACTION_SHOOT:
		if(plyr_pDat->m_bullets && plyr_pDat->m_bullets[plyr_pDat->m_curBullet])
		{
			//incase this bullet is already in the world
			if(!g_gi.WorldEntityExist(plyr_pDat->m_bullets[plyr_pDat->m_curBullet]))
				g_gi.WorldAddEntity(plyr_pDat->m_bullets[plyr_pDat->m_curBullet]);

			if(g_game.view)
				plyr_pDat->m_bullets[plyr_pDat->m_curBullet]->m_loc = g_game.view->GetTgt();

			plyr_pDat->m_bullets[plyr_pDat->m_curBullet]->m_dir = m_ent->m_dir;

			//play fire sound
			g_gi.SoundPlay(plyr_pDat->m_bulletFireSnd, plyr_pDat->m_bullets[plyr_pDat->m_curBullet]->m_loc);

			plyr_pDat->m_curBullet++; if(plyr_pDat->m_curBullet == plyr_pDat->m_numBullets) plyr_pDat->m_curBullet = 0;
		}
		
		break;

	case PLYR_ACTION_BALL:
		if (g_game.view != 0)
		{
			hENTITY pBall = 0;
			
			for (Uint32 index = 0; index < sizeof(plyr_pDat->mBalls) / sizeof(plyr_pDat->mBalls[0]); ++index)
			{
				if (g_gi.WorldEntityExist(plyr_pDat->mBalls[index])) continue;
				
				pBall = plyr_pDat->mBalls[index];

				break;
			}

			if (0 == pBall) break;

			g_gi.WorldAddEntity(pBall);

			pBall->m_loc = g_game.view->GetTgt();

			g_gi.SoundPlay(plyr_pDat->m_bulletFireSnd, pBall->m_loc);

			Vec3D end = g_game.view->GetTgt() + 150.0f * m_ent->m_dir;

			f32 fGrav = -g_gi.WorldGetGravity().y;

			f32 fAngle[2];	TrajectGetAngle(g_game.view->GetTgt(), end, 200.0f, fGrav, fAngle);

			TrajectGetVelocity(g_game.view->GetTgt(), end, 200.0f, fAngle[0], fGrav, 0.0f, pBall->m_dir);

			pBall->m_spd = Vec3DLen(&pBall->m_dir);
			g_gi.Vec3DNormal(&pBall->m_dir, &pBall->m_dir);

			pBall->m_vel = Vec3D(0.0f, 0.0f, 0.0f);
		}

		break;
	}
}

void Player::AddToWorld(const Vec3D & loc, const Vec3D & dir)
{
	m_ent->m_loc = loc;
	m_ent->m_dir = dir;

	//make sure the entity has not been added already
	if(!g_gi.WorldEntityExist(m_ent))
	{
		g_gi.WorldAddEntity(m_ent);
	}
}

void Player::Rotate(f32 amt)
{
	Vec3D dir = m_ent->m_dir;

	Matrix rotMtx;
	g_gi.MtxRotateY(amt, &rotMtx);
	
	g_gi.Vec3DTransformNormal(&dir, &rotMtx, &m_ent->m_dir);
}

void Player::UpdateView(f32 mX, f32 mY)
{
	PlyrDat *plyr_pDat = (PlyrDat *)m_ent->m_usrDat;

	//rotating via mouse
	g_game.viewRotX += (mY)/100; 

	if(g_game.viewRotX < -PI/2)
		g_game.viewRotX = -PI/2;
	else if(g_game.viewRotX > PI/3)
		g_game.viewRotX = PI/3;

	if(g_game.viewbLook)
	{
		g_game.viewRotY += (mX)/100;

		f32 dirRotY = g_gi.Vec2DGetAngle(&Vec2D(m_ent->m_dir.x, m_ent->m_dir.z));

		Matrix rotMtx;
		g_gi.MtxYawPitchRoll(dirRotY+g_game.viewRotY, g_game.viewRotX, 0, &rotMtx);
		
		g_gi.Vec3DTransformNormal(&g_game.viewDirDefault, &rotMtx, &g_game.viewDir);
	}
	else if(!g_game.viewbLook)
	{
		g_game.viewRotY = 0;

		if(m_ent->m_obj)
		{
			f32 dirRotY = g_gi.Vec2DGetAngle(&Vec2D(m_ent->m_dir.x, m_ent->m_dir.z));

			Matrix rotMtx;
			g_gi.MtxYawPitchRoll(dirRotY, g_game.viewRotX, 0, &rotMtx);
			
			g_gi.Vec3DTransformNormal(&g_game.viewDirDefault, &rotMtx, &g_game.viewDir);
		}
	}

	//set the view to back of current Ta-Ta.
	Vec3D lookat(plyr_pDat->m_lookatOfs+m_ent->m_loc);

	g_game.view->SetTgt(lookat);
	g_game.view->SetPt(lookat + (plyr_pDat->m_lookDist*g_game.viewDir));
}

void Player::ClearMovStates()
{
	PlyrDat *plyr_pDat = (PlyrDat *)m_ent->m_usrDat;

	if(m_ent->m_obj)
	{
		for(u8 i = 0; i < PLYR_STATE_MAX; i++)
		{
			m_ent->m_obj->MixerClearCycle(plyr_pDat->m_movAnim[i], 0.3f);
		}
	}
}