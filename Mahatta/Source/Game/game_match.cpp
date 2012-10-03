#include "game_match.h"
#include "game_team.h"
#include "game_teamplayer.h"
#include "game_lua.h"

Match::Match() {
	lua_newthread(g_game.mLua);	// T
	lua_getglobal(lua_tothread(g_game.mLua, -1), "MatchProc");	// T: MatchProc

	mMatchProc = luaL_ref(g_game.mLua, LUA_REGISTRYINDEX);	// _R[mMatchProc] = T -> stack clear

	// timer init
	matchtimer = 0;
	
	// scoring flags
	scoring = FALSE;
	collisionring1 = collisionscore1 = collisioncourt1 = FALSE;
	collisionring2 = collisionscore2 = collisioncourt2 = FALSE;
	scoreteam1 = scoreteam2 = 0;
	scoringduration = 3.0;

	wcscpy(message, L"");
}

Match::~Match(){
	luaL_unref(g_game.mLua, LUA_REGISTRYINDEX, mMatchProc);
}

Match* Match::getMe(){
	return Lua::_toclassT<Match>(g_game.mLua, "match");
}

Team * Match::getTeam (u8 teamid)
{
	return 1 == teamid || 2 == teamid ? Lua::_toclassT<Team>(g_game.mLua, 1 == teamid ? "team1" : "team2") : 0;
}

void Match::setMovementDir(MovementDir movDir){
	if(movDir == MOV_FORWARD)
		bmoveforward = TRUE;
	else if(movDir == MOV_LEFT)
		bmoveleft = TRUE;
	else if(movDir == MOV_RIGHT)
		bmoveright = TRUE;
	else if(movDir == MOV_BACKWARD)
		bmovebackward = TRUE;
}

void Match::update(){
	//
	if (InPlay()) g_gi.BindExecute();

	// timer stuff
	matchtimer += g_gi.EngineGetTick();
	matchminutes = matchtimer / 60;
	matchseconds = matchtimer - (matchminutes * 60);
	matchcentiseconds = (matchtimer - (matchminutes * 60) - matchseconds) * 100;

	// decide stuff according to collision flags set
	if(collisionscore1 || collisionscore2 || collisionring1 || collisionring2)
		score();
	
	checkBallPosition();

	processInput();

	// clear all collision flags so entities can set them anew on next world update
	collisionring1 = collisionscore1 = collisioncourt1 = FALSE;
	collisionring2 = collisionscore2 = collisioncourt2 = FALSE;

	// update scoring time and see if scoring flag needs to be cleared
	if(scoring){
		scoringtime += g_gi.EngineGetTick();
		if(scoringtime >= scoringduration){
			scoring = FALSE;
			wcscpy(message, L"");
		}
	}

	// Step through the match logic.
	Lua::Resume(g_game.mLua, mMatchProc);

	// Keep camera properly oriented.
	g_game.player->UpdateView(0, 0);

	// Update game objects.
	g_gi.WorldUpdate();
}

void Match::render(){
	// display score message, animation or whatever...
	if(wcslen(message) > 0)
		g_game.dbgFnt->Printf(150, 100, 0xffffffff, 0, message);

	// print scores
	g_game.dbgFnt->Printf(200, 50, 0xffffffff, 0, L"%d - %d", scoreteam1, scoreteam2);

	// print match timer
	g_game.dbgFnt->Printf(250, 25, 0xffffffff, 0, L"Time: %.2d:%.2d:%.2d", matchminutes, matchseconds, matchcentiseconds);
/*
	// print selected player indicator
	if(getTeam(1)->getPlayer(i) == g_game.player);
		g_game.dbgFnt->Printf(xpos + playernametexturewidth + 5, ypos + 5, 0xff00ff00, 0, L"<<");
		
	// print selected player indicator
	if(getTeam(2)->getPlayer(i) == g_game.player);
		g_game.dbgFnt->Printf(xpos - 5 - 20, ypos + 5, 0xff00ff00, 0, L">>");
*/
/// DEBUG
g_gi.TextureBegin();
Matrix Mtx;	g_gi.GFXGetViewProjMtx(&Mtx);
while (!lines.empty())
{
	Line line = lines.front();
	lines.pop_front();
	g_gi.LineDraw3D(line.vs, 2, &Mtx, line.color);
}
g_gi.TextureEnd();
/// DEBUG
}

void Match::setBallCollision(wstring entityname, wstring side_id){
	
	// ring collisions
	if(entityname == L"ring_area"){
		lua_getglobal(g_game.mLua, side_id == L"1" ? "team1" : "team2");// team

		Lua::SetGlobal(g_game.mLua, "match.scorer");

		if(side_id == L"1") collisionring1 = TRUE;
		if(side_id == L"2") collisionring2 = TRUE;
	
	// score area collisions
	}else if(entityname == L"score_area"){
		lua_getglobal(g_game.mLua, side_id == L"1" ? "team1" : "team2");// team

		Lua::SetGlobal(g_game.mLua, "match.scorer");

		if(side_id == L"1") collisionscore1 = TRUE;
		if(side_id == L"2") collisionscore2 = TRUE;

	// court side collisions
	}else if(entityname == L"court_side"){
		if(side_id == L"1")
			collisioncourt1  = TRUE;
		else if(side_id == L"2")
			collisioncourt2  = TRUE;
	}
}

bool Match::InPlay (void)
{
	Lua::GetGlobal(g_game.mLua, "match.bInPlay");	// bInPlay

	bool bInPlay = lua_toboolean(g_game.mLua, -1) != 0;

	lua_pop(g_game.mLua, 1);

	return bInPlay;
}

void Match::score(){
	// initialize scoring stuff if a score is not already in process
	if(!scoring){
		scoring = TRUE;
		scoringtime = 0.0;
		
		// give priority to score area, then check if ring area was hit
		if(collisionscore1){
			scoreteam1 += SCORE_INTORING;
			swprintf(message, L"The %s team scores %d point(s)", getTeam(1)->getName(), SCORE_INTORING);
		}else if(collisionscore2){
			scoreteam2 += SCORE_INTORING;
			swprintf(message, L"The %s team scores %d point(s)", getTeam(2)->getName(), SCORE_INTORING);
		}else if(collisionring1){
			scoreteam1 += SCORE_HITRING;
			swprintf(message, L"The %s team scores %d point(s)", getTeam(1)->getName(), SCORE_HITRING);
		}else if(collisionring2){
			scoreteam2 += SCORE_HITRING;
			swprintf(message, L"The %s team scores %d point(s)", getTeam(2)->getName(), SCORE_HITRING);
		}
	}
}

void Match::checkBallPosition(){
	if(!scoring){
		if(!(collisioncourt1 || collisioncourt2)){
			wcscpy(message, L"Ball out of court area");
		}else if(collisioncourt1 && collisioncourt2)
			wcscpy(message, L"Ball is passing sides");
		else if(collisioncourt1)
			swprintf(message, L"Ball is on the %s team side", getTeam(1)->getName());
		else if(collisioncourt2)
			swprintf(message, L"Ball is on the %s team side", getTeam(2)->getName());
		else
			wcscpy(message, L"");
	}
}

void Match::processInput(){

	// check to see if there's something to control, else ignore input
	if(g_game.player && g_game.player->GetEnt()){

		// decide what to do on currently controlled player according to input received
		if(bmoveleft){
			if(bmoveforward){
				g_game.player->Move(TP_STATE_RUNSTRAFELF);
				g_game.player->GetEnt()->MoveStrafe(-.707f);
				g_game.player->GetEnt()->MoveDir(.707f);
			}else if(bmovebackward){
				g_game.player->Move(TP_STATE_RUNSTRAFELB);
				g_game.player->GetEnt()->MoveStrafe(-.707f);
				g_game.player->GetEnt()->MoveDir(-.707f);
			}else if(!bmoveright){
				g_game.player->Move(TP_STATE_RUNSTRAFEL);
				g_game.player->GetEnt()->MoveStrafe(-1.0f);
			}
		}else if(bmoveright){
			if(bmoveforward){
				g_game.player->Move(TP_STATE_RUNSTRAFERF);
				g_game.player->GetEnt()->MoveStrafe(.707f);
				g_game.player->GetEnt()->MoveDir(.707f);
			}else if(bmovebackward){
				g_game.player->Move(TP_STATE_RUNSTRAFERB);
				g_game.player->GetEnt()->MoveStrafe(.707f);
				g_game.player->GetEnt()->MoveDir(-.707f);
			}else if(!bmoveleft){
				g_game.player->Move(TP_STATE_RUNSTRAFER);
				g_game.player->GetEnt()->MoveStrafe(1.0f);
			}
		}else if(bmoveforward && !bmovebackward){
			g_game.player->Move(TP_STATE_RUNF);
			g_game.player->GetEnt()->MoveDir(1.0f);
		}else if(bmovebackward && !bmoveforward){
			g_game.player->Move(TP_STATE_RUNB);
			g_game.player->GetEnt()->MoveDir(-1.0f);
		}
	}

	// reset input flags
	bmoveleft = bmoveright = bmoveforward = bmovebackward = FALSE;
}

///
/// LUA
///
template<> char const * Lua::_typeT<Match> (void)
{
	return "Match";
}

static int Render (lua_State * L)
{
	Lua::_pT<Match>(L, 1)->render();

	return 0;
}

static int Update (lua_State * L)
{
	Lua::_pT<Match>(L, 1)->update();

	return 0;
}

static int Match_gc (lua_State * L)
{
	Lua::_pT<Match>(L, 1)->~Match();

	return 0;
}

void LuaMatch_A (void * pData)
{
	new (pData) Match;
}

static int Match_New (lua_State * L)
{
	LuaMatch_A(Lua::UD(L, 1));

	return 0;
}

void luabind_Match (void)
{
	// Define the match.
	luaL_reg funcs[] = {
		{ "__gc", Match_gc },
		{ "Render", Render },
		{ "Update", Update },
		{ 0, 0 }
	};

	Lua::class_Define(g_game.mLua, "Match", funcs, Match_New, 0, sizeof(Match));
}
