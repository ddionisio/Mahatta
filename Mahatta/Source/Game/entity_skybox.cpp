#include "game_local.h"

void Spawn_SkyBox(const EntityParse & entDat)
{
	const tCHAR *val;

	hEFFECT fx=0;

	val = entDat.GetVal(L"skyBox");
	if(val) g_gi.SkyLoad(val); /*fx = g_gi.EffectLoad(val);

	if(fx)
	{
		g_gi.SkyBoxInit(10.0f, fx);

		//just remove the ref...
		g_gi.EffectDestroy(fx);
	}*/
}