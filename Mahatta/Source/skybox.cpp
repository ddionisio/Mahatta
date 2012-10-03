#include "engine_common.h"

//for now we just have an hOBJ to load
hOBJ g_skyBoxOBJ = 0;

void SkyDestroy()
{
	if(g_skyBoxOBJ)
	{ g_skyBoxOBJ->Release(); g_skyBoxOBJ = 0; }
}

u32 SkyLoad(const tCHAR *modelfile)
{
	SkyDestroy();

	hMDL mdl = g_GFX->MDLLoad(modelfile);

	if(mdl)
	{
		g_skyBoxOBJ = g_GFX->OBJCreate(mdl);
		mdl->Release();
	}

	return g_skyBoxOBJ ? TRUE : FALSE;
}

void SkyRender()
{
	if(g_skyBoxOBJ)
	{
		g_GFX->SkyBoxBegin();
		g_skyBoxOBJ->DisplaySkyBox(0,0);
		g_GFX->SkyBoxEnd();
	}
}