#ifndef _GAME_TEAMMANAGER_H
#define _GAME_TEAMMANAGER_H

#include <vector>
#include "game_share.h"
#include "game_team.h"

// Loads and manages teams
class TeamManager{

	public:
		// singleton thing, call this to get the object then call its methods
		static TeamManager* getMe();
		~TeamManager();

		void Init();									// load teams from file

		//Team* getTeamByName(const tCHAR name);			// useful?
		Team* getTeamByIndex(u8 index);

		u8 getNumberPlayersPerTeam(){
			return numberplayers;
		}

		//Team* getRandomTeam();							// get a random team
		//Team* getDifferentRandomTeam(Team *otherTeam);	// get a random team different
														 // to otherTeam
		std::vector <Team *> getTeams();

	protected:
		// declared here to avoid instancing
		TeamManager();			
		TeamManager& operator= (const TeamManager&);

	private:
		u8 numberteams, numberplayers;			// number of team, and number of players per team
		
		static TeamManager* pinstance;
		std::vector <Team *> teams;
		
};

#endif