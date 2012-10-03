#include "engine_local.h"

#include "command.h"

#include "value.h"

#include "world.h"

#include "entity.h"

#include "view.h"

#include "skybox.h"

//The world Interface, this includes loading the world
//has a list of entities, targets and waypoints

//internal globals

Vec3D	 g_gravity(0,0,0);		//the gravity that affects all entities

wstring  g_mapPath;				//the current map's path

hQBSP	 g_map;					//the current loaded map

EntList	 g_entities;			//all the entities in this world

//list of waypoints? targets? entities?

/////////////////////////////////////
// Name:	WorldGetMap
// Purpose:	get the QBSP
// Output:	none
// Return:	the QBSP
/////////////////////////////////////
hQBSP WorldGetMap()
{
	return g_map;
}

/////////////////////////////////////
// Name:	WorldDestroy
// Purpose:	destroy current world
// Output:	world destroyed!
// Return:	none
/////////////////////////////////////
void WorldDestroy()
{
	//call WriteLevel from game export
	if(g_ge->WriteLevel)
		g_ge->WriteLevel(g_saveFile.c_str());

	//call WriteGame from game export
	if(g_ge->WriteGame)
		g_ge->WriteGame(g_saveFile.c_str(), g_autoSave);

	//destroy entities
	for(EntIt i = g_entities.begin(); i != g_entities.end(); i++)
	{ (*i)->m_bWrldAttached = FALSE; (*i)->Release(); }
	
	g_entities.clear();

	//clear out targets
	WorldTargetClear();

	//clear out waypoints
	WorldWPClear();

	//destroy map
	if(g_map)
	{ g_map->Release(); g_map = 0; }

	//clear out lights?
	g_GFX->LightRemoveAll();
}

/////////////////////////////////////
// Name:	WorldSetGravity
// Purpose:	set gravity
// Output:	gravity set
// Return:	none
/////////////////////////////////////
void WorldSetGravity(const Vec3D & v)
{
	g_gravity = v;
}

/////////////////////////////////////
// Name:	WorldGetGravity
// Purpose:	get gravity
// Output:	none
// Return:	gravity
/////////////////////////////////////
Vec3D WorldGetGravity()
{
	return g_gravity;
}

/////////////////////////////////////
// Name:	WorldAddEntity
// Purpose:	add entity to world, this
//			will make the entity
//			update and display
// Output:	entity added to list
// Return:	none
/////////////////////////////////////
void WorldAddEntity(hENTITY ent)
{
	hIENTITY iEnt = (hIENTITY)ent;

	if(iEnt->m_bWrldAttached) return; //we are already attached

	iEnt->m_bWrldAttached = TRUE;
	iEnt->AddRef();
	g_entities.push_back(iEnt);
}

/////////////////////////////////////
// Name:	WorldRemoveEntity
// Purpose:	remove entity from world, this
//			will make the entity
//			not update and display
// Output:	entity removed from list
// Return:	none
/////////////////////////////////////
void WorldRemoveEntity(hENTITY ent)
{
	hIENTITY iEnt = (hIENTITY)ent;

	if(iEnt->m_bWrldAttached)
	{
		iEnt->m_bWrldAttached = FALSE;

		g_entities.remove(iEnt);

		iEnt->Release();
	}
}

/////////////////////////////////////
// Name:	WorldEntityExist
// Purpose:	check to see if given
//			entity is in the world
// Output:	none
// Return:	TRUE if found
/////////////////////////////////////
u8 WorldEntityExist(hENTITY ent)
{
	return ((hIENTITY)ent)->m_bWrldAttached;
}

/////////////////////////////////////
// Name:	WorldEnumEntities
// Purpose:	invokes a callback on each
//			entity in the world
// Output:	none
// Return:	none
/////////////////////////////////////
void WorldEnumEntities (void (*Enum)(hENTITY, void*), void * context)
{
	for (EntIt it = g_entities.begin(); it != g_entities.end(); ++it) Enum(*it, context);
}

/////////////////////////////////////
// Name:	WorldUpdate
// Purpose:	update the world and the
//			entity.
// Output:	a lot stuff happens
// Return:	TRUE for success
/////////////////////////////////////
s32 WorldUpdate()
{
	//update world
	hIENTITY ent;

	//go through entities and update them
	EntIt it, itNext;

	for(it = g_entities.begin(); it != g_entities.end(); it = itNext)
	{
		itNext = it; itNext++;

		ent = *it;

		if(ent)
		{
			//does this entity want to die?
			if(TESTFLAGS(ent->m_flag, ENTITY_FLAG_POLLDEATH))
			{
				//just in case there are reference of this entity
				CLEARFLAG(ent->m_flag, ENTITY_FLAG_POLLDEATH);

				//remove from world
				WorldRemoveEntity((hENTITY)ent);
			}
			else
			{
				ent->Update(g_map);

				//set gravity
				if(!TESTFLAGS(ent->m_flag, ENTITY_FLAG_NOGRAVITY))
					ent->m_vel += g_gravity*EngineGetTick();

				ent->m_vel += ent->m_pushVel;
				ent->m_pushVel.x = ent->m_pushVel.y = ent->m_pushVel.z = 0;
			}
		}
		else
		{
			//remove from list
			g_entities.erase(it);
		}
	}

	//collide entities
	WorldCollisionExec(g_map, g_entities);

	return TRUE;
}

/////////////////////////////////////
// Name:	WorldDisplay
// Purpose:	renders the world and the
//			entities
// Output:	stuff displayed
// Return:	TRUE for success
/////////////////////////////////////
s32 WorldDisplay(const Vec3D & eye)
{
	if(g_map)
		g_map->PrepareFrame();

	//go through entities and render them
	EntIt it;

	for(it = g_entities.begin(); it != g_entities.end(); it++)
	{
		(*it)->Render(g_map, eye);
	}

	//render world
	if(g_map)
	{
		g_map->Render();
	}

	return TRUE;
}

//
// World commands
//

void CMD_Map()
{
	if(CmdArgC() != 1) 
	{
		LogMsg(0, 0, L"usage: map <mapname>.");
		return;
	}

	wstring mapPath = L"Maps\\";

	//check to see if argument 0 is a variable
	if(ValueExist(CmdArgV(0)))
	{
		tCHAR cmdStr[MAXCHARBUFF];
		ValueGetStr(CmdArgV(0), cmdStr, MAXCHARBUFF);

		mapPath += cmdStr;
	}
	else
		mapPath += CmdArgV(0);

	mapPath += L".bsp";

	//first, check to see if this map is valid...
	if(!g_GFX->QBSPIsValid(mapPath.c_str()))
	{ LogMsg(0, mapPath.c_str(), L"File does not exist or is invalid."); return; }

	WorldPostMap(mapPath.c_str());
}

/////////////////////////////////////
// Name:	WorldCmdInit
// Purpose:	initialize world basic
//			commands
// Output:	more basic commands (like map)
// Return:	none
/////////////////////////////////////
void WorldCmdInit()
{
	CmdAdd(L"map", CMD_Map);
}