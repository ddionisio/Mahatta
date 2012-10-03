#include "game_local.h"

void Spawn_Door(const EntityParse & entDat);
void Spawn_Doodad(const EntityParse & entDat);
//void Spawn_LoadMap(const EntityParse & entDat);
void Spawn_SkyBox(const EntityParse & entDat);
void Spawn_NPC(const EntityParse & entDat);
void Spawn_Block(const EntityParse & entDat);
// new Entity
void Spawn_Scorearea(const EntityParse & entDat);
void Spawn_Ringarea(const EntityParse & entDat);
void Spawn_Courtside(const EntityParse & entDat);
void Spawn_TP(const EntityParse & entDat);

//for spawning entities

void GameSpawnEntity(const EntityParse & entDat)
{
	const tCHAR *name = entDat.GetVal(L"classname");
	const tCHAR *val;

	//now let's determine how to load this entity
	if(name)
	{
		if(wcsicmp(name, L"worldspawn") == 0)
		{
			g_gi.EngineLoadDisplayText(g_game.dbgFnt, L"Setting Very Important Game Variables");

			//get ambient
			val = entDat.GetVal(L"_color");
			if(val)
			{
				Vec3D ambClr(1,1,1);
				swscanf(val, L"%f %f %f", 
					&ambClr.x, &ambClr.y, &ambClr.z);
				
				g_gi.LightSetAmbient(GFXCOLOR_COLORVALUE(ambClr.x, ambClr.y, ambClr.z, 1.0f));
			}

			//get skyboxes
			Spawn_SkyBox(entDat);

			//get music
			val = entDat.GetVal(L"music");
			if(val)
				g_gi.MusicPlay(val);

			////////////////////////////////////////
			//set fog
			val = entDat.GetVal(L"fogParam");
			if(val)
			{
				g_gi.FogEnable(TRUE);
				g_gi.FogSetMode(GFXFOG_LINEAR);

				f32 fogMin=0,fogMax=0;
				swscanf(val, L"%f %f", &fogMin, &fogMax);

				g_gi.FogSetStart(fogMin);
				g_gi.FogSetEnd(fogMax);

				//get color
				val = entDat.GetVal(L"fogColor");
				if(val)
				{
					Vec3D fogClr(1,1,1);
					swscanf(val, L"%f %f %f", 
						&fogClr.x, &fogClr.y, &fogClr.z);

					g_gi.FogSetColor(GFXCOLOR_COLORVALUE(fogClr.x, fogClr.y, fogClr.z, 1.0f));
				}
			}
			else
				g_gi.FogEnable(FALSE);
			////////////////////////////////////////
		}
		else if(wcsicmp(name, L"door") == 0)
		{
			g_gi.EngineLoadDisplayText(g_game.dbgFnt, L"Loading Entity: Door");

			Spawn_Door(entDat);
		}
		else if(wcsicmp(name, L"doodad") == 0 || wcsicmp(name, L"doodad_b") == 0)
		{
			g_gi.EngineLoadDisplayText(g_game.dbgFnt, L"Loading Entity: Doodad");

			Spawn_Doodad(entDat);
		}
		/*else if(wcsicmp(name, L"loadmap") == 0)
		{
			g_gi.EngineLoadDisplayText(g_game.dbgFnt, L"Loading Entity: LoadMap");

			Spawn_LoadMap(entDat);
		}*/
		else if(wcsicmp(name, L"block") == 0)
		{
			g_gi.EngineLoadDisplayText(g_game.dbgFnt, L"Loading Entity: Block");

			Spawn_Block(entDat);
		}
		else if(wcsicmp(name, L"info_npc_start") == 0)
		{
			g_gi.EngineLoadDisplayText(g_game.dbgFnt, L"Loading Entity: NPC");

			Spawn_NPC(entDat);
		}
		// new entity
		else if(wcsicmp(name, L"score_area") == 0)
		{
			g_gi.EngineLoadDisplayText(g_game.dbgFnt, L"Loading Entity: Score Area");

			Spawn_Scorearea(entDat);
		}
		else if(wcsicmp(name, L"ring_area") == 0)
		{
			g_gi.EngineLoadDisplayText(g_game.dbgFnt, L"Loading Entity: Ring Area");

			Spawn_Ringarea(entDat);
		}
		else if(wcsicmp(name, L"court_side") == 0)
		{
			g_gi.EngineLoadDisplayText(g_game.dbgFnt, L"Loading Entity: Court Side");

			Spawn_Courtside(entDat);
		}
		else if(wcsicmp(name, L"teamplayer_slot") == 0)
		{
			g_gi.EngineLoadDisplayText(g_game.dbgFnt, L"Loading Entity: Team Player Slot");

			Spawn_TP(entDat);
		}
		/*/this is here for now...
		else if((wcsicmp(name, L"info_player_deathmatch") == 0
			     || wcsicmp(name, L"info_player_start") == 0)
			&& g_game.player)
		{
			g_gi.EngineLoadDisplayText(g_game.dbgFnt, L"Loading Entity: Player");

			tCHAR plyrTgt[MAXCHARBUFF]={0}; g_game.player->GetTargetStr(plyrTgt);

			const tCHAR *buff;
			f32 angle=0;
			Vec3D loc, dir(0,0,1);

			//do we want this spawn?
			buff = entDat.GetVal(L"target");

			if(buff && plyrTgt[0] != 0)
			{
				if(wcsicmp(plyrTgt, buff) != 0)
					return;
			}

			//get point
			buff = entDat.GetVal(L"origin");

			if(buff)
			{
				//swap at the same time, the y and z
				swscanf(buff, L"%f %f %f", 
					&loc.x, &loc.z, &loc.y);

				loc.z *= -1;
			}

			//get angle
			if(entDat.GetVal(L"angle"))
			{
				Matrix mtx;
				Vec3D edir(1,0,0);
				f32 angle;
				swscanf(entDat.GetVal(L"angle"), L"%f", &angle);
				
				angle = TO_RAD(angle);

				g_gi.MtxRotateY(angle, &mtx);
				g_gi.Vec3DTransformNormal(&edir, &mtx, &dir);
			}

			g_game.player->AddToWorld(loc, dir);
		}*/
	}
}