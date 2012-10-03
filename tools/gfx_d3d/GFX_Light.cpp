#include "gfx_i.h"

#include "GFX/gfx_math.h"

hLIGHT *g_lights=0;
u32	   *g_lightInds=0;
u32 g_numLight=0, g_numAvail=0;

//the culling function
LIGHTCULLFUNC g_cullF = 0;

//used for restoring device
u32		g_ambient=0xffffffff;

/////////////////////////////////////
// Name:	LightEnableRender
// Purpose:	enable/disable lighting
// Output:	light enabled/disabled
// Return:	none
/////////////////////////////////////
void F_API LightEnableRender(u8 bYes)
{
	if(bYes) SETFLAG(g_FLAGS, GFX_LIGHTENABLE);
	else CLEARFLAG(g_FLAGS, GFX_LIGHTENABLE);

	g_p3DDevice->SetRenderState(D3DRS_LIGHTING,bYes);
}

/////////////////////////////////////
// Name:	Light(Set/Get)Ambient
// Purpose:	set/get the ambient
// Output:	ambient if 'Get'
// Return:	none
/////////////////////////////////////
void F_API LightSetAmbient(u32 ambient)
{
	g_ambient = ambient;

	g_p3DDevice->SetRenderState(D3DRS_AMBIENT,g_ambient);
}

u32 F_API LightGetAmbient()
{
	DWORD amb;
	g_p3DDevice->GetRenderState(D3DRS_AMBIENT, &amb);
	
	return (u32)amb;
}

//call this after the device has been restored
void LightOnReset()
{
	g_p3DDevice->SetRenderState(D3DRS_LIGHTING,TESTFLAGS(g_FLAGS, GFX_LIGHTENABLE));
	g_p3DDevice->SetRenderState(D3DRS_AMBIENT,g_ambient);
}

/////////////////////////////////////
// Name:	LightAdd
// Purpose:	add a light to the light
//			list.
// Output:	new light added
// Return:	light pointer that you
//			can manipulate
/////////////////////////////////////
hLIGHT F_API LightAdd(const gfxLight *lightDat)
{
	u32 newInd = g_numLight;

	g_lights = (hLIGHT *)realloc(g_lights, sizeof(hLIGHT)*(g_numLight+1));
	g_lightInds = (u32 *)realloc(g_lightInds, sizeof(u32)*(g_numLight+1));

	if(g_lights && g_lightInds)
	{
		g_lights[newInd] = (hLIGHT)MemAlloc(sizeof(gfxLightItm));
		memcpy(&g_lights[newInd]->lDat, lightDat, sizeof(gfxLight));

		g_lightInds[newInd] = newInd;

		g_numLight++;

		return g_lights[newInd];
	}

	return 0;
}

/////////////////////////////////////
// Name:	LightGetDat
// Purpose:	get data of given light
// Output:	lightDat set
// Return:	none
/////////////////////////////////////
void F_API LightGetDat(hLIGHT l, gfxLight *lightDat)
{
	memcpy(lightDat, &l->lDat, sizeof(gfxLight));
}

/////////////////////////////////////
// Name:	LightSetDat
// Purpose:	set the light data
// Output:	light data set
// Return:	none
/////////////////////////////////////
void F_API LightSetDat(hLIGHT l, const gfxLight *lightDat)
{
	memcpy(&l->lDat, lightDat, sizeof(gfxLight));
}

/////////////////////////////////////
// Name:	LightSetPriority
// Purpose:	set the light priority for
//			shadows (0 is lowest)
// Output:	light priority set
// Return:	none
/////////////////////////////////////
void F_API LightSetPriority(hLIGHT l, f32 priority)
{
	l->prior = priority;
}

/////////////////////////////////////
// Name:	LightRemove
// Purpose:	destroy given light and
//			remove it from list.
// Output:	given light destroyed
//			and light list updated
// Return:	none
/////////////////////////////////////
void F_API LightRemove(hLIGHT l)
{
	if(l)
	{
		hLIGHT *pNewLightList=0;
		u32	   *pNewLightInd=0;

		if(g_numLight > 1)
		{
			pNewLightList = (hLIGHT *)MemAlloc(sizeof(hLIGHT)*(g_numLight-1));
			pNewLightInd  = (u32 *)MemAlloc(sizeof(u32)*(g_numLight-1));
		}
		else
			pNewLightList = 0;

		if(pNewLightList && pNewLightInd)
		{
			for(u32 i = 0, j = 0; i < g_numLight; i++)
			{
				pNewLightInd[i] = i;

				if(l != g_lights[i])
				{
					pNewLightList[j] = g_lights[i];
					j++;
				}
			}
		}

		MemFree(l);
		MemFree(g_lights);
		MemFree(g_lightInds);

		g_lights = pNewLightList;
		g_lightInds = pNewLightInd;

		g_numLight--;
	}
}

/////////////////////////////////////
// Name:	LightRemoveAll
// Purpose:	remove all lights from
//			list.  Remember, this will
//			destroy lights from the
//			list as well, so you better
//			make sure you don't have
//			pointer to any of the lights
//			after calling this.
// Output:	light list cleared
// Return:	none
/////////////////////////////////////
void F_API LightRemoveAll()
{
	if(g_lights)
	{
		for(u32 i = 0; i < g_numLight; i++)
		{
			if(g_lights[i])
				MemFree(g_lights[i]);
		}

		MemFree(g_lights);
	}

	if(g_lightInds)
		MemFree(g_lightInds);

	g_lights = 0;
	g_lightInds = 0;
	g_numLight = 0;
}

/////////////////////////////////////
// Name:	LightGetNum
// Purpose:	get number of lights
// Output:	none
// Return:	number of lights on list
/////////////////////////////////////
u32 F_API LightGetNum()
{
	return g_numLight;
}

/////////////////////////////////////
// Name:	LightSetCullCallback
// Purpose:	set the light culling
//			function
// Output:	cull function set
// Return:	none
/////////////////////////////////////
void F_API LightSetCullCallback(LIGHTCULLFUNC lCB)
{
	g_cullF = lCB;
}

int _LightCompare( const void *arg1, const void *arg2 )
{
	hLIGHT l1 = g_lights[*(u32*)arg1];
	hLIGHT l2 = g_lights[*(u32*)arg2];

	if(l1->dist < l2->dist)
		return -1;
	else if(l1->dist > l2->dist)
		return 1;

	return 0;
}

void LightCullView(const Vec3D &eye)
{
	g_numAvail = 0;
	u32 i;
	for(i = 0; i < g_numLight; i++)
	{
		if(g_lights[i]->lDat.Type == GFXLIGHT_POINT || g_lights[i]->lDat.Type == GFXLIGHT_SPOT)
		{
			//call callback for culling...
			if(g_cullF && g_cullF(eye, g_lights[i]->lDat))
				continue;
		}
		
		//always add directional lights
		g_lightInds[g_numAvail] = i;
		g_numAvail++;
	}
}

/////////////////////////////////////
// Name:	LightGetAllNearest
// Purpose:	add lights that are near
//			given world AABB to given list
// Output:	lights added to given list
// Return:	none
/////////////////////////////////////
void LightGetAllNearest(const Vec3D & mins, const Vec3D &maxs, hLIGHT *pLights)
{
	//the point is at the center of both mins/maxs
	Vec3D pt((mins+maxs)*0.5f);

	//clear out given list first...
	memset(pLights, 0, sizeof(hLIGHT)*MAXLIGHT);

	//don't bother doing anything for no lights
	if(!TESTFLAGS(g_FLAGS, GFX_LIGHTENABLE))
		return;

	Vec3D v;

	if(g_lights && g_lightInds && g_numLight > 0)
	{
		u32 i, ind;
		for(i = 0; i < g_numAvail; i++)
		{
			ind = g_lightInds[i];

			if(g_lights[ind]->lDat.Type == GFXLIGHT_POINT || g_lights[ind]->lDat.Type == GFXLIGHT_SPOT)
			{
				v = pt - g_lights[ind]->lDat.Position;

				g_lights[ind]->dist = Vec3DDot(&v, &v);

				//make sure it is in range
				/*if(g_lights[ind]->dist <= g_lights[ind]->lDat.Range*g_lights[ind]->lDat.Range)
				{
				}*/
			}
			else //we prioritize directional lights, so they are always added
			{
				g_lights[ind]->dist = -((f32)i);
			}
		}

		//sort
		qsort(g_lightInds, g_numAvail, sizeof(u32), _LightCompare);

		//get the first 3 or less lights
		f32 r;

		ind = 0;

		Vec3D lMin, lMax;

		u32 max = MAXLIGHT < g_numAvail ? MAXLIGHT : g_numAvail;
		for(i = 0; i < max; i++)
		{
			lMin.x = g_lights[g_lightInds[i]]->lDat.Position.x - g_lights[g_lightInds[i]]->lDat.Range;
			lMin.y = g_lights[g_lightInds[i]]->lDat.Position.y - g_lights[g_lightInds[i]]->lDat.Range;
			lMin.z = g_lights[g_lightInds[i]]->lDat.Position.z - g_lights[g_lightInds[i]]->lDat.Range;

			lMax.x = g_lights[g_lightInds[i]]->lDat.Position.x + g_lights[g_lightInds[i]]->lDat.Range;
			lMax.y = g_lights[g_lightInds[i]]->lDat.Position.y + g_lights[g_lightInds[i]]->lDat.Range;
			lMax.z = g_lights[g_lightInds[i]]->lDat.Position.z + g_lights[g_lightInds[i]]->lDat.Range;

			//r = g_lights[g_lightInds[i]]->lDat.Range; r *= r;
			//if(g_lights[g_lightInds[i]]->dist <= r)
			if(g_lights[g_lightInds[i]]->lDat.Type == GFXLIGHT_DIRECTIONAL
				|| BoundsIntersect(mins, maxs, lMin, lMax))
			{ pLights[ind] = g_lights[g_lightInds[i]]; ind++; }
		}
	}
}

/////////////////////////////////////
// Name:	LightEnableList
// Purpose:	add lights from list
//			to graphics device
// Output:	lights set
// Return:	
/////////////////////////////////////
void LightEnableList(const hLIGHT *pLights)
{
	if(pLights)
	{
		u32 max = g_d3dcaps.MaxActiveLights < MAXLIGHT ? g_d3dcaps.MaxActiveLights : MAXLIGHT;

		for(u32 i = 0; i < max; i++)
		{
			if(pLights[i])
			{
				g_p3DDevice->SetLight(i, (const D3DLIGHT9 *)&pLights[i]->lDat);
				g_p3DDevice->LightEnable(i, TRUE);
			}
		}
	}
}

/////////////////////////////////////
// Name:	LightDisableList
// Purpose:	remove lights from list
//			from graphics device
// Output:	lights unset
// Return:	
/////////////////////////////////////
void LightDisableList()
{
	for(u32 ind = 0; ind < MAXLIGHT; ind++)
		g_p3DDevice->LightEnable(ind, FALSE);
}

//get the light we will use for shadows (NULL for none)
hLIGHT _LightGetShadow(hLIGHT lights[MAXLIGHT])
{
	//for now we don't look for directional lights

	hLIGHT l = lights[0];

	for(u8 i = 1; i < MAXLIGHT; i++)
	{
		if(lights[i])// && lights[i]->lDat.Type != GFXLIGHT_DIRECTIONAL)
		{
			if(!l || (lights[i]->prior > l->prior))
				l = lights[i];
		}
	}

	return l;
}