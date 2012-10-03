#include "game_team.h"
#include "game_ball.h"
#include "game_match.h"
#include "game_teamplayer.h"
#include "game_lua.h"
#include <cfloat>

Team::Team(tCHAR const oname[]) {
	wcscpy(name, oname);
}

Team::~Team(){
	for (size_t index = 0; index < players.size(); ++index)
	{
		luaL_unref(g_game.mLua, LUA_REGISTRYINDEX, players[index]);
	}
}

tCHAR* Team::getName(){
	return name;
}

tCHAR* Team::getProfileText(){
	return profiletext;
}

tCHAR* Team::getProfileGraphic(){
	return profilegraphic;
}

TeamPlayer* Team::getPlayerMapped(const tCHAR *map){
	return getPlayer(getIndexMapped(map));
}

TeamPlayer* Team::getPlayer(u8 index){
	if (index >= players.size()) return 0;

	lua_rawgeti(g_game.mLua, LUA_REGISTRYINDEX, players[index]);// player

	TeamPlayer * tp = static_cast<TeamPlayer*>(Lua::UD(g_game.mLua, -1));

	lua_pop(g_game.mLua, 1);

	return tp;
}

tCHAR* Team::getPlayerMappedCFGFile(const tCHAR *map){
	return getPlayer(getIndexMapped(map))->getFile();
}

void Team::setEntToPlayerMapped(const tCHAR *map, hENTITY ent){
	getPlayer(getIndexMapped(map))->setEnt(ent);
}

void Team::changeToNextAvailablePlayer(){
	u8 currentplayer = getCurrentPlayerIndex();
	u8 nextavailableplayer = getNextAvailablePlayerIndex();
	if(nextavailableplayer != players.size()){
		// update types
		getPlayer(currentplayer)->setType(TEAMPLAYERTYPE_AI);
		getPlayer(nextavailableplayer)->setType(TEAMPLAYERTYPE_HUMANLOCAL);
		// update currently controlled player to game module
		g_game.player = getPlayer(nextavailableplayer);
	}
}

std::wstring Team::getPlayerHudFileMapped(std::wstring const & map, u8 id){
	u8 index = getIndexMapped(map.c_str());
	return getPlayer(index)->getHudFile(id);
}

/////////////////////////////////////
// Purpose:	add TeamPlayer to this Team
// Output:	TeamPlayer added to Team
// Return:	TRUE or FALSE
/////////////////////////////////////
u8 Team::addTeamPlayer (int pref)
{
	players.push_back(pref);

	return TRUE;
}

// private methods
u8 Team::getIndexMapped(const tCHAR *map){
	u8 index;
	if(wcsicmp(map, L"1") == 0)
		index = 0;
	else if(wcsicmp(map, L"2") == 0)
		index = 1;
	else if(wcsicmp(map, L"3") == 0)
		index = 2;
	else if(wcsicmp(map, L"4") == 0)
		index = 3;
	return index;
}

u8 Team::getCurrentPlayerIndex(){
	for(u8 idx = 0; idx < players.size(); idx++)
		if(getPlayer(idx)->getType() == TEAMPLAYERTYPE_HUMANLOCAL)
			return idx;
	return players.size();  // should never happen
}

u8 Team::getNextAvailablePlayerIndex(){
	// find current controlled player index
	u8 idx = getCurrentPlayerIndex();
	
	// start 'cycling up' from current controllable player and see if next is available
	// if so, return it, else keep cycling, if there's no available player, return out of bounds index
	for(u64 i = 1; i < players.size() - 1; i++)
		if(getPlayer((idx + i) % players.size())->getType() == TEAMPLAYERTYPE_AI)
			return (idx + i) % players.size();
	return players.size();	// COULD happen, control this case
}

///
/// LUA
///
using namespace Lua;

template<> char const * Lua::_typeT<Team> (void)
{
	return "Team";
}

static int addTeamPlayer (lua_State * L)
{
	lua_settop(L, 2);	// team, player

	_pT<Team>(L, 1)->addTeamPlayer(luaL_ref(L, LUA_REGISTRYINDEX));

	return 0;
}

int _LuaTeam_TP (Team * team, int index)
{
	return team->players[index];
}

static int getPlayer (lua_State * L)
{
	lua_rawgeti(L, LUA_REGISTRYINDEX, _LuaTeam_TP(_pT<Team>(L, 1), sI(L, 2)));	// player

	return 1;
}

static int getPlayerHudFileMapped (lua_State * L)
{
	class_New(L, "WS", "u", &_pT<Team>(L, 1)->getPlayerHudFileMapped(WS_(L, 2)->c_str(), U8(L, 3)));

	return 1;
}

static int Team_gc (lua_State * L)
{
	_pT<Team>(L, 1)->~Team();

	return 0;
}

static int Team_len (lua_State * L)
{
	lua_pushinteger(L, _pT<Team>(L, 1)->GetSize());

	return 1;
}

static int Team_New (lua_State * L)
{
	new (UD(L, 1)) Team(WS_(L, 2)->c_str());

	return 0;
}

void luabind_Team (void)
{
	// Define the team.
	luaL_reg funcs[] = {
		{ "addTeamPlayer", addTeamPlayer },
		{ "getPlayer", getPlayer },
		{ "getPlayerHudFileMapped", getPlayerHudFileMapped },
		{ "__gc", Team_gc },
		{ "__len", Team_len },
		{ 0, 0 }
	};

	class_Define(g_game.mLua, "Team", funcs, Team_New, 0, sizeof(Team));
}