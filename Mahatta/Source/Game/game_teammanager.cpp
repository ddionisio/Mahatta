#include "game_teammanager.h"
#include "game_local.h"

TeamManager* TeamManager::pinstance = NULL;

TeamManager::TeamManager(){

}

TeamManager::~TeamManager(){
	for(size_t i = 0; i < teams.size(); i++)
		if(teams[i])
			delete teams[i], teams[i] = NULL;
	teams.clear();
}

TeamManager* TeamManager::getMe(){
	if(pinstance == NULL)
		pinstance = new TeamManager();
	return pinstance;
}

Team* TeamManager::getTeamByIndex(u8 teamindex){
	return teams[teamindex];
}

void TeamManager::Init(){
	
	hCFG textfile;
	textfile = g_gi.CfgFileLoad(L"teams.ini"); // hardcodded ini file

	if(textfile)
	{
/*		tCHAR itemtosearch[50];
		tCHAR itemvalue[50], itemvalue2[50], itemvalue3[100];
		
		numberteams = textfile->GetItemInt(L"Teams", 
										L"numberteams");  // get number of teams
		numberplayers = textfile->GetItemInt(L"Teams",
										L"numberplayers");  // get number of players
															  //per team

		// load as many teams as specified
		for(u8 i = 1; i <= numberteams; i++){
			
			swprintf(itemtosearch, L"team%d", i);
			textfile->GetItemStr(L"Teams", itemtosearch, itemvalue, 50);	// read team name
			
			Team* tempTeam = new Team(itemvalue); // create team 
			tCHAR texturefile[MAXCHARBUFF];
			textfile->GetItemStr(itemvalue, L"teamnametexture", texturefile, MAXCHARBUFF);
			tempTeam->setNameTextureFile(texturefile); // add team name texture
			teams.push_back(tempTeam); // add team to teams list					

			// start adding players
			for(u8 j = 1; j <= numberplayers; j++){
				swprintf(itemtosearch, L"p%dname", j);
				textfile->GetItemStr(itemvalue, itemtosearch, itemvalue2, 50);	// read player name
				swprintf(itemtosearch, L"p%dfile", j);
				textfile->GetItemStr(itemvalue, itemtosearch, itemvalue3, 100); // read player file
				teams[i-1]->addTeamPlayer(new TeamPlayer(itemvalue2, itemvalue3));	// add player
			}

		}
*/
		textfile->Release();
	}

}