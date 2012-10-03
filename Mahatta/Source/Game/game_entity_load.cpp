#include "game_local.h"

#define ENTITY_FOLDER		(L"Data\\Entity\\")

#define ENTITY_SEC			(L"main")

/////////////////////////////////////
// Name:	GameEntityLoad
// Purpose:	generic entity load from
//			ini file (Data\Entity)
// Output:	entity loaded
// Return:	
/////////////////////////////////////
hENTITY GameEntityLoad(const tCHAR *name, u32 type, const tCHAR *dataName, const EntityVFTable & vftbl)
{
	hENTITY newEnt = 0;

	wstring filepath = ENTITY_FOLDER;
	filepath += dataName;
	filepath += L".ini";

	hCFG cfg = g_gi.CfgFileLoad(filepath.c_str());

	if(cfg)
	{
		///////////////////////////////////////////////////////
		//get all common data
		tCHAR buff[MAXCHARBUFF];

		tCHAR mdlFile[MAXCHARBUFF]={0};
		f32 spd, boundRadius;
		Vec3D boundMin, boundMax;
		FLAGS flag=0;
		
		cfg->GetItemStr(ENTITY_SEC, L"model", mdlFile, MAXCHARBUFF);
		
		spd = cfg->GetItemFloat(ENTITY_SEC, L"speed");

		if(cfg->GetItemStr(ENTITY_SEC, L"bound_min", buff, MAXCHARBUFF))
			swscanf(buff, L"%f,%f,%f", &boundMin.x, &boundMin.y, &boundMin.z);

		if(cfg->GetItemStr(ENTITY_SEC, L"bound_max", buff, MAXCHARBUFF))
			swscanf(buff, L"%f,%f,%f", &boundMax.x, &boundMax.y, &boundMax.z);

		boundRadius = cfg->GetItemFloat(ENTITY_SEC, L"bound_radius");

		if(cfg->ItemExist(ENTITY_SEC, L"flags"))
			flag = cfg->GetItemInt(ENTITY_SEC, L"flags");

		///////////////////////////////////////////////////////

		//create entity
		if(type >= 0 && type < ENTITY_MAX_TYPE)
		{
			newEnt = g_gi.EntCreate(name, type, mdlFile, vftbl, cfg);

			if(newEnt)
			{
				newEnt->m_spd = spd;

				newEnt->m_boundMin = boundMin;
				newEnt->m_boundMax = boundMax;
				newEnt->m_boundRadius = boundRadius;

				newEnt->m_flag = flag;
			}
		}

		cfg->Release();
	}

	return newEnt;
}