#ifndef _GAME_TEAM_H
#define _GAME_TEAM_H

#include "game_local.h"

#define TEAM_MAXNUMPLAYERS 4

class Team{
	public:
		Team(tCHAR const name[]);	// create team
		~Team();

		tCHAR* getName();			// return team name
		tCHAR* getProfileText();
		tCHAR* getProfileGraphic();	

		class TeamPlayer* getPlayerMapped(const tCHAR *map);
		class TeamPlayer* getPlayer(u8 index);
		tCHAR* getPlayerMappedName(const tCHAR *map);
		tCHAR* getPlayerMappedCFGFile(const tCHAR *map);
		
		// hud related methods
		std::wstring getPlayerHudFileMapped(std::wstring const & map, u8 id);

		// controlled player related methods
		void changeToNextAvailablePlayer();

		void setEntToPlayerMapped(const tCHAR *map, hENTITY ent);

		u8 addTeamPlayer (int pref);

		size_t GetSize (void) { return players.size(); }

		friend int _LuaTeam_TP (Team * team, int index);
	private:
		u8 getIndexMapped(const tCHAR *map);
		u8 getCurrentPlayerIndex();
		u8 getNextAvailablePlayerIndex();

		tCHAR name[50];				// team chosen
		tCHAR profiletext[500];		// team profile (description)
		tCHAR profilegraphic[50];	// team portrait / 2D graphic
		tCHAR nameFile[MAXCHARBUFF];

		std::vector<int> players;
};

void luabind_Team (void);

#endif