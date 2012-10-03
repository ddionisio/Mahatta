#include "game_local.h"
#include "game_lua.h"

#define GAME_MAIN_SEC	(L"main")

#define DBGFNT_RELSIZE		0.01875f //12/640

//Initialization

void GameInit()
{
	tCHAR buff[MAXCHARBUFF]={0}, path[MAXCHARBUFF]={0};

	g_game.mPhysicsStep = 1.0f / 45;

	//get load screen
	g_gi.ValueGetStr(L"loadscreen", path, MAXCHARBUFF);
	if(path[0] != 0)
		g_game.loadImg = g_gi.TextureLoad(path, 0, 0);

	//create debug font
	gfxMode mode={0}; g_gi.GFXGetMode(&mode);
	g_game.dbgFnt = g_gi.FontCreate(L"Arial Black", (s32)(DBGFNT_RELSIZE*mode.width), 0);

	//play intro video
	/*path[0] = 0;
	g_gi.ValueGetStr(L"introvid", path, MAXCHARBUFF);
	if(path[0] != 0)
		g_gi.EngineVideoPlay(path);*/
	//g_gi.EngineVideoPlay(L"videos\\intro.avi");

	//display loading
	GameDisplayLoad(TRUE);

	//initialize commands
	GameCmdInit();

	///////////////////////////////////////
	//load the view direction
	Vec3D vDir(0,0,1);
	f32 rotX=0, rotY=0, rotZ=0;

	g_gi.ValueGetFloat(L"viewrotX", &rotX);
	g_gi.ValueGetFloat(L"viewrotY", &rotY);
	g_gi.ValueGetFloat(L"viewrotZ", &rotZ);

	rotX = TO_RAD(rotX);
	rotY = TO_RAD(rotY);
	rotZ = TO_RAD(rotZ);

	Matrix theMtx;
	g_gi.MtxYawPitchRoll(rotY, rotX, rotZ, &theMtx);
	g_gi.Vec3DTransformNormal(&vDir, &theMtx, &g_game.viewDirDefault);

	g_game.viewDir = g_game.viewDirDefault;

	///////////////////////////////////////
	//Get scene FXs
	for (u32 i = 0; ; ++i)
	{
		swprintf(buff, L"sceneFX_%d", i);
		
		if(!g_gi.ValueExist(buff))
			break;

		g_gi.ValueGetStr(buff, path, MAXCHARBUFF);

		hEFFECT fx = g_gi.EffectLoad(path);
		g_game.sceneFX.push_back(fx);
	}

	// Setup Lua.
	g_game.mLua = Lua::NewState();

	Lua::BindGameLogic();

	//START PLAYING SOME BACKGROUND MUSIC THIS IS HARDCODED FIX ME!
	g_gi.MusicPlay(L"sounds\\music.ogg");

	g_gi.EngineLoadDisplayText(g_game.dbgFnt, L"Loading particles...");
	g_gi.ParSysLoad(L"particles\\particle.ps");

	///////////////////////////////////////
	//some other game global initialization
	g_game.curFX = -1;

	//gravity
	f32 gravSpd=0; g_gi.ValueGetFloat(L"grav", &gravSpd);
	g_gi.WorldSetGravity(Vec3D(0,-gravSpd,0));

	//g_gi.EngineFlagSet(ENGINE_FLAG_SHOWBBOX, TRUE);

	//g_gi.GFXSetProj(0,0,PI);
	g_game.flareImg = g_gi.TextureLoad(L"Textures\\table_side_glow.jpg", 0,0);

	HUDInit();
}
