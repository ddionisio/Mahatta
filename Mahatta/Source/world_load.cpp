#include "engine_common.h"

#include "command.h"

#include "world.h"

#include "entity.h"

//found from world.cpp
extern wstring g_mapPath;		//the current map's path
extern hQBSP	g_map;			//the current loaded map

list<EntityParse> g_entParse; //temporary list of entities to parse

//add variables from given buff
static void _EntParse(EntityParse & entP, tCHAR *buff)
{
	tCHAR lVal[MAXCHARBUFF], rVal[MAXCHARBUFF];
	tCHAR **walker = &buff;

	while(**walker != 0)
	{
		//get the two string for lVal and rVal
		ParserReadStringBuff(walker, lVal, MAXCHARBUFF, '"', '"');
		ParserReadStringBuff(walker, rVal, MAXCHARBUFF, '"', '"');
		
		//skip newline
		ParserSkipCharBuff(walker, '\n', FALSE);

		entP.AddVar(lVal, rVal);
	}
}

//Callback for loading entities inside the map
void WorldEntityParse(tCHAR *entityStr, hQBSP qbsp, void *userDat)
{
	EntityParse newEnt;

	_EntParse(newEnt, entityStr);

	//check if it is an info null, then if so,
	//parse it and add to target list
	if(newEnt.GetVal(L"classname"))
	{
		//base entities that should be pre loaded
		if(wcsicmp(newEnt.GetVal(L"classname"), L"info_null") == 0)
			WorldTargetAddEnt(newEnt);
		else if(wcsicmp(newEnt.GetVal(L"classname"), L"waypoint") == 0)
			WorldWPAddEnt(newEnt);
		//will be loaded later by engine and game module
		else
			g_entParse.push_back(newEnt);
	}
}

u8 WorldLightCull(const Vec3D & pt, const gfxLight & light)
{
	hQBSP qbsp = WorldGetMap();

	if(qbsp)
	{
		return !qbsp->InPVS(pt, light.Position);
	}

	return FALSE;
}

void ParCollision(const Vec3D & pt1, const Vec3D & pt2, u8 bCollideOBJ, 
				  Vec3D *pImpact, Vec3D *pNorm, f32 *pT)
{
	hQBSP qbsp = WorldGetMap();

	if(qbsp)
	{
		gfxTrace trace;
		qbsp->Collision(0,0, pt1, pt2, &trace, 1.0f);

		if(trace.t < 1)
		{
			*pImpact = pt1 + trace.t*(pt2-pt1);
			*pNorm = trace.norm;
		}

		*pT = trace.t;
	}
}

//returns TRUE after loading the exclusive entity
//FALSE o.w. so then we can send it to the game module.
static u8 _WorldLoadExclusive(const EntityParse & entDat)
{
	const tCHAR *buff;

	//check for light; if so, add it to light list
	if(wcsicmp(entDat.GetVal(L"classname"), L"light") == 0)
	{
		gfxLight newLight;
		memset(&newLight, 0, sizeof(gfxLight));

		newLight.Type = GFXLIGHT_POINT;

		newLight.Attenuation0 = 1.0f;
		//newLight.Attenuation1 = 0.0075f;
		//newLight.Attenuation2 = 0.000025f;

		buff = entDat.GetVal(L"_color");

		//get diffuse color
		if(buff)
		{
			swscanf(buff, L"%f %f %f", 
				&newLight.Diffuse.x, &newLight.Diffuse.y, &newLight.Diffuse.z);
			newLight.Diffuse.w = 1.0f;

			//newLight.Ambient = newLight.Diffuse;
			//newLight.Specular = newLight.Diffuse;
		}

		//get point
		buff = entDat.GetVal(L"origin");

		if(buff)
		{
			//swap at the same time, the y and z
			swscanf(buff, L"%f %f %f", 
				&newLight.Position.x, &newLight.Position.z, &newLight.Position.y);

			newLight.Position.z *= -1;
		}

		//get range
		buff = entDat.GetVal(L"light");

		if(buff)
		{
			swscanf(buff, L"%f", &newLight.Range);
		}

		//check for target
		buff = entDat.GetVal(L"target");
		if(buff)
		{
			Vec3D tgt;

			//make sure the target is available
			if(WorldTargetGet(buff, &tgt))
			{
				//set direction
				newLight.Direction = tgt - newLight.Position;
				f32 r1 = g_GFX->Vec3DNormal(&newLight.Direction, &newLight.Direction);

				//check if this is a sun entity
				buff = entDat.GetVal(L"_sun");
				if(buff && buff[0] == '1')
					newLight.Type = GFXLIGHT_DIRECTIONAL;
				//this is a spotlight
				else
				{
					newLight.Type = GFXLIGHT_SPOT;

					//get radius
					f32 radius = 0;

					buff = entDat.GetVal(L"radius");
					if(buff)
						swscanf(buff, L"%f", &radius);

					newLight.Falloff = 1.0f;

					Vec3D up(0,1,0), perp; Vec3DCross(&up, &newLight.Direction, &perp);
					g_GFX->Vec3DNormal(&perp, &perp);

					Vec3D tgt2(tgt + perp*radius);

					Vec3D dir2(tgt2 - newLight.Position);

					g_GFX->Vec3DNormal(&dir2, &dir2);

					newLight.Theta = acosf(Vec3DDot(&newLight.Direction, &dir2));
					newLight.Phi = newLight.Theta;

					//check for range
					if(newLight.Range < r1*r1) newLight.Range = r1*r1;
				}
			}
		}

		//add the light
		hLIGHT newL = g_GFX->LightAdd(&newLight);
		
		//get priority
		buff = entDat.GetVal(L"priority");
		if(buff)
		{
			f32 priority=0;
			swscanf(buff, L"%f", &priority);
			g_GFX->LightSetPriority(newL, priority);
		}
	}
	//not an exclusive engine entity
	else
		return FALSE;

	return TRUE;
}

/////////////////////////////////////
// Name:	WorldLoad
// Purpose:	load the world
// Output:	world loaded
// Return:	TRUE if success
/////////////////////////////////////
s32 WorldLoad(const tCHAR *filename)
{
	//initialize the entity bound
	IEntity::sBoundInit();

	//destroy previous world
	WorldDestroy();

	g_mapPath = filename;

	//load the map
	g_map = g_GFX->QBSPLoad(filename, 0, 6, WorldEntityParse, 0);

	//call the spawnentity for each item from game export
	for(list<EntityParse>::iterator et = g_entParse.begin(); et != g_entParse.end(); et++)
	{
		//if this is an engine exclusive entity,
		//then load it and continue
		if(!_WorldLoadExclusive(*et))
		{
			//all other unknown junk goes here
			if(g_ge->SpawnEntity)
				g_ge->SpawnEntity(*et);
		}
	}

	//we don't need these junk anymore
	g_entParse.clear();

	//call ReadLevel from game export
	if(g_ge->ReadLevel)
		g_ge->ReadLevel(g_saveFile.c_str());

	//set light culling
	g_GFX->LightSetCullCallback(WorldLightCull);

	//set particle collision
	g_GFX->ParSysSetCollisionFunc(ParCollision);

	return TRUE;
}

wstring g_mapPost = L"\0";

//post a map to be loaded after update (used by game.dll)
s32 WorldPostMap(const tCHAR *filename)
{
	if(filename)
		g_mapPost = filename;
	else
		g_mapPost = L"\0";

	return TRUE;
}

//check the path of posted map
const tCHAR * WorldGetPostMapPath()
{
	return g_mapPost.c_str();
}

//load the posted map
s32 WorldLoadPostMap()
{
	if(*g_mapPost.c_str() != 0)
	{
		WorldLoad(g_mapPost.c_str());
		g_mapPost = L"\0";
	}

	return TRUE;
}