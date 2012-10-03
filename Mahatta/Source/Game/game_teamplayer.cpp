#include "game_teamplayer.h"
#include "game_ball.h"
#include "game_match.h"
#include "game_team.h"
#include "game_lua.h"

TeamPlayer::TeamPlayer() : m_ent(0) {
}

TeamPlayer::TeamPlayer(tCHAR const oname[], tCHAR const ofile[]) : m_ent(0) {
	wcscpy(name, oname);
	wcscpy(file, ofile);
	type = TEAMPLAYERTYPE_AI;

	// TODO: fix this hardcoded parameters?
	hitAngle = 45 * PI / 180;
	hitDistance2 = 150 * 150;

	bGodMode = FALSE;

	// open configuration file, name doesnt have appended extension
	wstring filepath = L"Data\\Entity\\";
	filepath += file;
	filepath += L".ini";
	hCFG cfg =	g_gi.CfgFileLoad(filepath.c_str());

	// fields to load hud textures
	tCHAR *g_hudTextures[TP_HUD_MAXTEXTURES]={L"normalface", L"badface", L"KOface", L"hitface",
											L"normalname", L"badname", L"KOname"};

	// load hud texture names
	for(u8 i = 0; i < TP_HUD_MAXTEXTURES; i++){
		tCHAR temp[MAXCHARBUFF];
		cfg->GetItemStr(L"hudtextures", g_hudTextures[i], temp, MAXCHARBUFF);
		wstring crap;
		crap = temp;
		hudTexturesFiles.push_back(crap);
	}

	// init hud state
	hudstate = TP_HUDSTATE_NORMAL;

	// attributes TODO: read from file or save file?
	maxhealth = currhealth = 15;
	maxmagic = currmagic = 100;
	bcanrecoverhp = (currhealth == maxhealth)? FALSE : TRUE ;
	bcanrecovermp = (currmagic == maxmagic)? FALSE : TRUE ;

	// timings
	// actions (animations)
	actionduration = actiontime = 0.0f;
	bcanbecontrolled = TRUE;
	// botch
	botchduration = 0.5f;	// TODO: hardcodded ok?
	botchtime = 0.0f;
	bcanbehurt = TRUE;
	// recovery
	hprecoveryfrac = 0.0f;
	hprecoveryratepersec = 1; // TODO: read from file?
    mprecoveryfrac = 0.0f;
	mprecoveryratepersec = 1; // TODO: read from file?
	hppercentforrecovery = 0.2f; // TODO: read from file, hardcodded?
		
	// koed related
	bfallingdown = bkoed = bgettingup = FALSE;

	cfg->Release();

}

TeamPlayer::~TeamPlayer(){
}

void TeamPlayer::drainHealth(u8 amount, Vec3D *projectilepos){
	
	// kinda pointless, but check anyways
	if(!bcanbehurt)
		return;
	
	bcanbehurt = FALSE;

	// reduce health by amount
	if(currhealth <= amount){
		currhealth = 0;
		bcanrecoverhp = FALSE;
		// TODO: knock player down
		PerformAction(TP_ACTION_DROPDOWN);
	}else{

		bcanrecoverhp = TRUE;
		currhealth -= amount;

		// play hit animation according to projectilepos
		if(projectilepos){
			// determine ball's height range according to player's bounding box
			f32 projectileheight = projectilepos->y;

			f32 projectilemax = m_ent->m_loc.y + m_ent->m_boundMax.y;
			f32 projectilemin = m_ent->m_loc.y + m_ent->m_boundMin.y;
			f32 projectilemid = (projectilemax + projectilemin) / 2;
			
			// execute corresponding action
			if(projectileheight >= projectilemax)
				PerformAction(TP_ACTION_HITU);
			else if(projectileheight >= projectilemid)
				PerformAction(TP_ACTION_HITM);
			else
				PerformAction(TP_ACTION_HITD);

		// play default hit animation
		}else{
			PerformAction(TP_ACTION_HITM);
		}
	}
}

void TeamPlayer::drainMagic(u8 amount){
	if(currmagic <= amount)
		currmagic = 0;
	else
		currmagic -= amount;
}

void TeamPlayer::activateGodMode(){
	TPDat *TP_pDat = (TPDat *)m_ent->m_usrDat;

	m_ent->m_flag |= ENTITY_FLAG_IGNORE_COLLIDE | ENTITY_FLAG_NOGRAVITY;

	for(u8 i = 0; i < SPEED_MAX; i++)
		TP_pDat->m_speed[i] = 600.0f;

	bGodMode = TRUE;
}

// BEGIN Player 'inherited' methods
void TeamPlayer::Rotate(f32 amt)
{
	// dont rotate player if it's not supossed to be controlled
	if(!bcanbecontrolled)
		return;

	Vec3D dir = m_ent->m_dir;
	Vec3D tempdir3;
	Vec2D tempdir2;

	Matrix rotMtx;
	g_gi.MtxRotateY(amt, &rotMtx);
	
	if(bGodMode){
		g_gi.Vec3DTransformNormal(&dir, &rotMtx, &m_ent->m_dir);
		return;
	}

	g_gi.Vec3DTransformNormal(&dir, &rotMtx, &tempdir3);

	tempdir2.x = tempdir3.x;
	tempdir2.y = tempdir3.z;

	f32 angle = g_gi.Vec2DGetAngle(&tempdir2);
	
	f32 anglemin, anglemax, anglemin2, anglemax2;

	if(g_game.localteam == Match::getMe()->getTeam(1)){
		// angle range
		anglemin = 1 * PI / 180;
		anglemax = 179 * PI / 180;
		if((angle >= anglemin && angle <= anglemax))
			m_ent->m_dir = tempdir3;

	}else if(g_game.localteam == Match::getMe()->getTeam(2)){
		// angle ranges
		anglemin = -90 * PI / 180;
		anglemax = -1 * PI / 180;
		
		anglemin2 = 181 * PI / 180;
		anglemax2 = 270 * PI / 180;

		if((angle >= anglemin && angle <= anglemax) || (angle >= anglemin2 && angle <= anglemax2))
			m_ent->m_dir = tempdir3;
	}

}

void TeamPlayer::ClearAnimations()
{
	TPDat *TP_pDat = (TPDat *)m_ent->m_usrDat;

	if(m_ent->m_obj)
	{
		for(u8 i = 0; i < TP_STATE_MAX; i++)
			m_ent->m_obj->MixerClearCycle(TP_pDat->m_movAnim[i], 0.1f);
		for(u8 i = 0; i < TP_ACTION_MAX; i++)
			m_ent->m_obj->MixerClearCycle(TP_pDat->m_animAction[i], 0.1f);
	}

}

//set the movement
void TeamPlayer::Move(TPMove move)
{
	TPDat *TP_pDat = (TPDat *)m_ent->m_usrDat;

	TP_pDat->m_moveNext = move;
}

void TeamPlayer::MoveV(f32 amount){
	m_ent->m_loc.y += amount;
}

/*
   Determine which side of a plane the point p lies.
   The plane is defined by a normal n and point on the plane vp.
   Return 1 if the point is on the same side as the normal, 
   otherwise -1.
*/
int WhichSide(Vec3D p, Vec3D n, Vec3D vp)
{
	double D,s;

	D = -(n.x*vp.x + n.y*vp.y + n.z*vp.z);
	s = n.x*p.x + n.y*p.y + n.z*p.z + D;

	return(s<=0 ? -1 : 1);
}

void TeamPlayer::determineShootAction(){

	TPDat *TP_pDat = (TPDat *)m_ent->m_usrDat;

	if(!TP_pDat->mPlayer->isControllable())
		return;

	// deterime ball side according to player's orientation
	Vec3D upvect(0,1,0);

	Vec3D viewvect = m_ent->m_dir;

	Vec3D planenormal;
	Vec3DCross(&viewvect, &upvect, &planenormal);

	s8 side = WhichSide(Ball::getMe()->getEnt()->m_loc, planenormal, m_ent->m_loc);

	// determine ball's height range according to player's bounding box
	f32 ballheight = Ball::getMe()->getEnt()->m_loc.y;

	f32 ballmax = m_ent->m_loc.y + m_ent->m_boundMax.y;
	f32 ballmin = m_ent->m_loc.y + m_ent->m_boundMin.y;
	f32 ballmid = (ballmax + ballmin) / 2;

	// execute corresponding action
	if(side >= 0){
		if(ballheight >= ballmax)
			PerformAction(TP_ACTION_SHOOTJR);
		else if(ballheight >= ballmid)
			PerformAction(TP_ACTION_SHOOTUR);
		else
			PerformAction(TP_ACTION_SHOOTDR);
	}else{
		if(ballheight >= ballmax)
			PerformAction(TP_ACTION_SHOOTJL);
		else if(ballheight >= ballmid)
			PerformAction(TP_ACTION_SHOOTUL);
		else
			PerformAction(TP_ACTION_SHOOTDL);
	}

}

void TeamPlayer::PerformAction(TPAction a)
{

	TPDat *TP_pDat = (TPDat *)m_ent->m_usrDat;

	if(!m_ent->m_obj)
		return;

	f32 actionduration = m_ent->m_obj->AnimGetDuration(TP_pDat->m_animAction[a]);

	// do other stuff related to an executed action
	switch(a){
		case TP_ACTION_SHOOTJL:
		case TP_ACTION_SHOOTUL:
		case TP_ACTION_SHOOTDL:
		case TP_ACTION_SHOOTJR:
		case TP_ACTION_SHOOTUR:
		case TP_ACTION_SHOOTDR:
			if(canHitBall())
				hitBall();
			break;
		case TP_ACTION_DROPDOWN:
			ClearAnimations();
			bfallingdown = TRUE;
			fallingdowntime = 0.0f;
			fallingdownduration = actionduration;
			break;
		case TP_ACTION_RISE:
			ClearAnimations();
			bgettingup = TRUE;
			gettinguptime = 0.0f;
			gettingupduration = actionduration;
			break;
	}

	// execute action
	m_ent->m_obj->MixerExecuteAction(TP_pDat->m_animAction[a], 0.15f, 0.15f, 1.0f, FALSE);
	// set action duration on team player
	TP_pDat->mPlayer->setActionDuration(actionduration);
	
}

void TeamPlayer::UpdateView(f32 mX, f32 mY)
{
	TPDat *TP_pDat = (TPDat *)m_ent->m_usrDat;

	// original 
	//rotating via mouse
	g_game.viewRotX += (mY)/100; 

	if(!bGodMode){
		if(g_game.viewRotX < -PI/4)
			g_game.viewRotX = -PI/4;
		else if(g_game.viewRotX > PI/4)
			g_game.viewRotX = PI/4;
	}

	if(g_game.viewbLook)
	{
		g_game.viewRotY += (mX)/100;

		f32 dirRotY = g_gi.Vec2DGetAngle(&Vec2D(m_ent->m_dir.x, m_ent->m_dir.z));

		Matrix rotMtx;
		g_gi.MtxYawPitchRoll(dirRotY+g_game.viewRotY, g_game.viewRotX, 0, &rotMtx);
		
		g_gi.Vec3DTransformNormal(&g_game.viewDirDefault, &rotMtx, &g_game.viewDir);
	}
	else
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

	//set the view to back of current player
	Vec3D lookat(TP_pDat->m_lookatOfs + m_ent->m_loc);

	g_game.view->SetTgt(lookat);
	g_game.view->SetPt(lookat + (TP_pDat->m_lookDist*g_game.viewDir));

}

Team * TeamPlayer::GetTeam (void)
{
	return m_ent != 0 ? Match::getMe()->getTeam(static_cast<TPDat*>(m_ent->m_usrDat)->teamsideid[0] - L'0') : 0;
}

void TeamPlayer::updateTime(f32 deltatime){

	// update falling down time
	if(bfallingdown){
		bcanrecoverhp = FALSE;
		fallingdowntime += deltatime;
		if(fallingdowntime >= fallingdownduration){
			bfallingdown = FALSE;
			bcanbecontrolled = FALSE;
			bkoed = TRUE;
			bcanrecoverhp = TRUE;
			Move(TP_STATE_DOWNED);
		}
	// check health level to see if player should get up
	}else if(bkoed){
		if( (currhealth * 1.0f) / (maxhealth * 1.0f) >= hppercentforrecovery){
			bkoed = FALSE;
			PerformAction(TP_ACTION_RISE);
		}
	// update getting up time
	}else if(bgettingup){
		gettinguptime += deltatime;
		if(gettinguptime >= gettingupduration){
			bgettingup = FALSE;
			bcanbecontrolled = TRUE;
			Move(TP_STATE_IDLE1);
		}
	// update normal animation times
	}else if(!bcanbecontrolled && !bkoed){
		actiontime += deltatime;
		if(actiontime >= actionduration)
			bcanbecontrolled = TRUE;
	}

	// update botch times (to avoid a single 'hit' doing damage several times)
	if(!bcanbehurt && !bfallingdown && !bkoed && !bgettingup){ // means we were recently hurt and are 'invencible' momentarily
		botchtime += deltatime;
		if(botchtime >= botchduration){
			botchtime = 0.0f;
			bcanbehurt = TRUE;
		}
	}

	// update attribute recovering times (accumulate attribute recovery and update integers)
	if(bcanrecoverhp){
		hprecoveryfrac += hprecoveryratepersec * deltatime;
		if(hprecoveryfrac >= 1){
			currhealth += 1;
			hprecoveryfrac -= 1;
			if(currhealth == maxhealth){
				bcanrecoverhp = FALSE;
				hprecoveryfrac = 0.0f;
			}
		}
	}	


	// update hitting times (to delay action in regards to input)

}

void TeamPlayer::setEnt (hENTITY ent)
{
	if (m_ent) static_cast<TPDat*>(m_ent->m_usrDat)->mPlayer = 0;

	m_ent = ent;

	if (m_ent) static_cast<TPDat*>(m_ent->m_usrDat)->mPlayer = this;
}

// END

u8 TeamPlayer::canHitBall(){
	if (GetTeam() == Match::getMe()->getTeam(2)) return TRUE;
	// test distance
	Vec3D * ballpos = Ball::getMe()->getPos();
	Vec3D diff = *ballpos - m_ent->m_loc;
	if(Vec3DLenSQ(&diff) > hitDistance2)
		return FALSE;

	// test angle
	Vec3D viewVect, targetVect;
	viewVect = GetEnt()->m_dir;
	targetVect = *ballpos - GetEnt()->m_loc;
	g_gi.Vec3DNormal(&viewVect, &viewVect);
	g_gi.Vec3DNormal(&targetVect, &targetVect);
	f32 anglediff = acos(Vec3DDot(&viewVect, &targetVect));
	if(anglediff > hitAngle)
		return FALSE;

	return TRUE;
}

void TeamPlayer::hitBall(){

	// calculate direction and normalize
	Vec3D hitDirection;
	
	if (this == g_game.player)
	{
		hitDirection = g_game.view->GetTgt() - g_game.view->GetPt();

		// angle correction, lower Y component to fake a greater angle	
		hitDirection.y += 100;
	}
	
	else
	{
		hitDirection = GetEnt()->m_dir;
	}

	//
	Vec3DScaleTo(&hitDirection, 450.0f, &hitDirection);

	Ball::getMe()->hit(hitDirection);

	//
	lua_getglobal(g_game.mLua, GetTeam() == Match::getMe()->getTeam(1) ? "team1" : "team2");// ball, team

	Lua::SetGlobal(g_game.mLua, "ball.hitter");
}

///
/// LUA
///
using namespace Lua;

template<> char const * Lua::_typeT<TeamPlayer> (void)
{
	return "TeamPlayer";
}

static inline TPDat * _TP (lua_State * L)
{
	return static_cast<TPDat*>(_pT<TeamPlayer>(L, 1)->GetEnt()->m_usrDat);
}

static int GetEnt (lua_State * L)
{
	class_New(L, "hENTITY", "u", _pT<TeamPlayer>(L, 1)->GetEnt());

	return 1;
}

static int GetFacing (lua_State * L)
{
	class_New(L, "Vec3D", "u", &_TP(L)->mFacing);	// facing

	return 1;
}

static int getHealthPercent (lua_State * L)
{
	lua_pushnumber(L, _pT<TeamPlayer>(L, 1)->getHealthPercent());

	return 1;
}

static int getMagicPercent (lua_State * L)
{
	lua_pushnumber(L, _pT<TeamPlayer>(L, 1)->getMagicPercent());

	return 1;
}

static int GetMaxSpeed (lua_State * L)
{
	f32 maxs = 0.0f, * speeds = _TP(L)->m_speed;

	for (int index = 0; index < SPEED_MAX; ++index) maxs = speeds[index] > maxs ? speeds[index] : maxs;

	lua_pushnumber(L, maxs);// maxs

	return 1;
}

static int GetOrigin (lua_State * L)
{
	class_New(L, "Vec3D", "u", &_TP(L)->mOrigin);	// origin

	return 1;
}

static int GetSteering (lua_State * L)
{
	lua_rawgeti(L, LUA_REGISTRYINDEX, _TP(L)->mSteering);	// steering

	return 1;
}

static int GetTeam (lua_State * L)
{
	lua_getglobal(L, _pT<TeamPlayer>(L, 1)->GetTeam() == Match::getMe()->getTeam(1) ? "team1" : "team2");	// team

	return 1;
}

static int IsHurt (lua_State * L)
{
	lua_pushboolean(L, _pT<TeamPlayer>(L, 1)->isInvincible());

	return 1;
}

static int Rotate (lua_State * L)
{
	_pT<TeamPlayer>(L, 1)->Rotate(F(L, 2));

	return 0;
}

static int SetNeighborRange (lua_State * L)
{
	_TP(L)->mNeighborRange = F(L, 2);

	return 0;
}

static int SetProc (lua_State * L)
{
	luaL_unref(L, LUA_REGISTRYINDEX, _TP(L)->mProc);

	if (lua_isfunction(L, 2))
	{
		lua_newthread(L);	// player, func, T
		lua_insert(L, 2);	// player, T, func
		lua_xmove(L, lua_tothread(L, 2), 1);// T: func -> player, T
	}

	_TP(L)->mProc = lua_isthread(L, 2) ? luaL_ref(L, LUA_REGISTRYINDEX) : LUA_REFNIL;

	return 0;
}

static int Shoot (lua_State * L)
{
	_pT<TeamPlayer>(L, 1)->determineShootAction();

	return 0;
}

static int UpdateView (lua_State * L)
{
	_pT<TeamPlayer>(L, 1)->UpdateView(F(L, 2), F(L, 3));

	return 0;
}

static int TeamPlayer_gc (lua_State * L)
{
	_pT<TeamPlayer>(L, 1)->~TeamPlayer();

	return 0;
}

static int TeamPlayer_New (lua_State * L)
{
	new (UD(L, 1)) TeamPlayer(WS_(L, 2)->c_str(), WS_(L, 3)->c_str());

	return 0;
}

void luabind_TeamPlayer (void)
{
	// Define the team player.
	luaL_reg funcs[] = {
		{ "__gc", TeamPlayer_gc },
		{ "GetEnt", GetEnt },
		{ "GetFacing", GetFacing },
		{ "getHealthPercent", getHealthPercent },
		{ "getMagicPercent", getMagicPercent },
		{ "GetMaxSpeed", GetMaxSpeed },
		{ "GetOrigin", GetOrigin },
		{ "GetSteering", GetSteering },
		{ "GetTeam", GetTeam },
		{ "IsHurt", IsHurt },
		{ "Rotate", Rotate },
		{ "SetNeighborRange", SetNeighborRange },
		{ "SetProc", SetProc },
		{ "Shoot", Shoot },
		{ "UpdateView", UpdateView },
		{ 0, 0 }
	};

	class_Define(g_game.mLua, "TeamPlayer", funcs, TeamPlayer_New, 0, sizeof(TeamPlayer));
}