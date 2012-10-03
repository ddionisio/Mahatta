#include "engine_common.h"

#include "world.h"

//targets are just 3d vectors with names
//for easy access from levels

map<wstring, Vec3D> g_targets;
typedef map<wstring, Vec3D>::iterator tgtIt;

/////////////////////////////////////
// Name:	WorldTargetSet
// Purpose:	add/change a target in world
// Output:	add/set target
// Return:	none
/////////////////////////////////////
void WorldTargetSet(const tCHAR *name, const Vec3D & pt)
{
	//that's it
	g_targets[name] = pt;
}

/////////////////////////////////////
// Name:	WorldTargetGet
// Purpose:	get the target
// Output:	ptOut set
// Return:	TRUE if found
/////////////////////////////////////
u8 WorldTargetGet(const tCHAR *name, Vec3D *ptOut)
{
	tgtIt it = g_targets.find(name);

	if(it != g_targets.end())
	{
		if(ptOut)
			*ptOut = (*it).second;

		return TRUE;
	}

	return FALSE;
}

//exclusive engine use:

/////////////////////////////////////
// Name:	WorldTargetAddEnt
// Purpose:	add a target based on given
//			entity parse
// Output:	new target added
// Return:	none
/////////////////////////////////////
void WorldTargetAddEnt(const EntityParse & entityDat)
{
	const tCHAR * pName = entityDat.GetVal(L"targetname");
	wstring theName = pName ? pName : L"NULL";
	Vec3D theLoc(0,0,0);

	//bad target
	if(theName.c_str()[0] == 0)
		return;

	//get location
	if(entityDat.GetVal(L"origin"))
	{
		swscanf(entityDat.GetVal(L"origin"), L"%f %f %f", &theLoc.x, &theLoc.z, &theLoc.y);

		theLoc.z *= -1;
	}

	//add to list
	WorldTargetSet(theName.c_str(), theLoc);
}

/////////////////////////////////////
// Name:	WorldTargetClear
// Purpose:	clear out all targets
// Output:	no more targets
// Return:	none
/////////////////////////////////////
void WorldTargetClear()
{
	g_targets.clear();
}