#include "engine_local.h"

#include "bind.h"
#include "value.h"
#include "command.h"

//game specific
#include "view.h"
#include "entity.h"
#include "world.h"
#include "waypoint.h"

#include "skybox.h"

/////////////////////////////////////
// Name:	ModuleGameDestroy
// Purpose:	destroy game module
// Output:	game.dll unloaded
// Return:	none
/////////////////////////////////////
void ModuleGameDestroy()
{
	if(g_ge)
	{
		if(g_ge->Destroy) //this should never be NULL
			g_ge->Destroy();

		EngineModDestroy(MODULE_GAME);
		g_ge = 0;
	}
}

/////////////////////////////////////
// Name:	ModuleGameInit
// Purpose:	load up game.dll
// Output:	game.dll loaded and
//			initialized
// Return:	TRUE if success
/////////////////////////////////////
s32 ModuleGameInit()
{
	GameImport import;

	ModuleGameDestroy();

	//set the functions, this is gonna get long...

	////////////////////////// STD Stuff /////////////////////////////////
	import.MemAlloc = MemAlloc;
	import.MemFree = MemFree;
	import.MemRealloc = MemRealloc;

	import.LogClear = LogClear;
	import.LogPrintf = LogPrintf;
	import.LogMsg = LogMsg;

	import.FSGetPath = FSGetPath;
	import.FSOpen = FSOpen;
	import.FSClose = FSClose;
	import.FSRead = FSRead;
	import.FSWrite = FSWrite;
	import.FSEOF = FSEOF;
	import.FSGetChar = FSGetChar;
	import.FSPutChar = FSPutChar;
	import.FSSeek = FSSeek;
	import.FSGetPos = FSGetPos;
	import.FSSetPos = FSSetPos;
	import.FSPrintf = FSPrintf;

	import.ParserReadStringFile = ParserReadStringFile;
	import.ParserCountStringFile = ParserCountStringFile;
	import.ParserReadWordFile = ParserReadWordFile;
	import.ParserCountWordFile = ParserCountWordFile;
	import.ParserSkipCharFile = ParserSkipCharFile;
	import.ParserSkipString = ParserSkipString;

	import.ParserReadStringBuff = ParserReadStringBuff;
	import.ParserCountStringBuff = ParserCountStringBuff;
	import.ParserReadWordBuff = ParserReadWordBuff;
	import.ParserCountWordBuff = ParserCountWordBuff;
	import.ParserSkipCharBuff = ParserSkipCharBuff;
	import.GetFilePart = GetFilePart;
	import.GetExtension = GetExtension;
	import.GetFilename = GetFilename;
	import.ConcatinateFilePath = ConcatinateFilePath;
	import.ParserFindFiles = ParserFindFiles;
	import.ParserGrabAllTextFile = ParserGrabAllTextFile;

	//////////////////////// Config/Script ////////////////////////////////
	import.CfgFileLoad = CfgFileLoad;

	//////////////////////////// Maths ///////////////////////////////////
	import.Vec2DCatMullRom = g_GFX->Vec2DCatMullRom;
	import.Vec2DHermite = g_GFX->Vec2DHermite;
	import.Vec2DNormal = g_GFX->Vec2DNormal;
	import.Vec2DTransform = g_GFX->Vec2DTransform;
	import.Vec2DTransformCoord = g_GFX->Vec2DTransformCoord;
	import.Vec2DTransformNormal = g_GFX->Vec2DTransformNormal;
	import.Vec2DGetAngle = g_GFX->Vec2DGetAngle;
	import.Vec3DCatMullRom = g_GFX->Vec3DCatMullRom;
	import.Vec3DHermite = g_GFX->Vec3DHermite;
	import.Vec3DNormal = g_GFX->Vec3DNormal;
	import.Vec3DTransform = g_GFX->Vec3DTransform;
	import.Vec3DTransformCoord = g_GFX->Vec3DTransformCoord;
	import.Vec3DTransformNormal = g_GFX->Vec3DTransformNormal;
	import.Vec3DPerpendicular = g_GFX->Vec3DPerpendicular;
	import.Vec3DMakeNormalVectors = g_GFX->Vec3DMakeNormalVectors;
	import.Vec3DRotatePointAroundVector = g_GFX->Vec3DRotatePointAroundVector;
	import.Vec4DCross = g_GFX->Vec4DCross;
	import.Vec4DCatMullRom = g_GFX->Vec4DCatMullRom;
	import.Vec4DHermite = g_GFX->Vec4DHermite;
	import.Vec4DNormal = g_GFX->Vec4DNormal;
	import.Vec4DTransform = g_GFX->Vec4DTransform;
	import.QuatLN = g_GFX->QuatLN;
	import.QuatExp = g_GFX->QuatExp;
	import.QuatNorm = g_GFX->QuatNorm;
	import.QuatInv = g_GFX->QuatInv;
	import.QuatRotAxis = g_GFX->QuatRotAxis;
	import.QuatRotEuler = g_GFX->QuatRotEuler;
	import.QuatRotMtx = g_GFX->QuatRotMtx;
	import.QuatToAxisAngle = g_GFX->QuatToAxisAngle;
	import.QuatSlerp = g_GFX->QuatSlerp;
	import.QuatBaryCentric = g_GFX->QuatBaryCentric;
	import.QuatSquadSetup = g_GFX->QuatSquadSetup;
	import.QuatSquad = g_GFX->QuatSquad;
	import.PlaneFromPoints = g_GFX->PlaneFromPoints;
	import.PlaneNormalize = g_GFX->PlaneNormalize;
	import.PlaneTransform = g_GFX->PlaneTransform;
	import.ProjectPointOnPlane = g_GFX->ProjectPointOnPlane;
	import.PlaneCategorize = g_GFX->PlaneCategorize;
	import.PlaneTypeForNormal = g_GFX->PlaneTypeForNormal;
	import.SignbitsForPlane = g_GFX->SignbitsForPlane;
	import.BoxOnPlaneSide = g_GFX->BoxOnPlaneSide;
	import.MtxInverse = g_GFX->MtxInverse;
	import.MtxDeterminant = g_GFX->MtxDeterminant;
	import.MtxTranspose = g_GFX->MtxTranspose;
	import.MtxTranslate = g_GFX->MtxTranslate;
	import.MtxScale = g_GFX->MtxScale;
	import.MtxRotateX = g_GFX->MtxRotateX;
	import.MtxRotateY = g_GFX->MtxRotateY;
	import.MtxRotateZ = g_GFX->MtxRotateZ;
	import.MtxYawPitchRoll = g_GFX->MtxYawPitchRoll;
	import.MtxAxisRotation = g_GFX->MtxAxisRotation;
	import.MtxQuaternion = g_GFX->MtxQuaternion;
	import.MtxOrtho = g_GFX->MtxOrtho;
	import.MtxOrthoOffCenter = g_GFX->MtxOrthoOffCenter;
	import.MtxPerspective = g_GFX->MtxPerspective;
	import.MtxPerspectiveOffCenter = g_GFX->MtxPerspectiveOffCenter;
	import.MtxPerspectiveFOV = g_GFX->MtxPerspectiveFOV;
	import.MtxLookAt = g_GFX->MtxLookAt;
	import.RandDouble = g_GFX->RandDouble;
	import.RandRngFloat = g_GFX->RandRngFloat;
	import.RandRngInt = g_GFX->RandRngInt;
	import.RayIntersectBox = g_GFX->RayIntersectBox;
	import.BoundsIntersect = g_GFX->BoundsIntersect;
	import.BoundsClear = g_GFX->BoundsClear;
	import.BoundsAddPoint = g_GFX->BoundsAddPoint;

	//////////////////////////// Input ////////////////////////////////////
	import.INPJStickAnyArrowReleased = EngineInputJStickAnyArrowReleased;
	import.INPJStickAnyArrowPressed = EngineInputJStickAnyArrowPressed;
	
	import.INPIsPressed = EngineInputIsPressed;
	import.INPIsReleased = EngineInputIsReleased;
	import.INPAnyKeyPressed = EngineInputAnyKeyPressed;
	import.INPAnyKeyReleased = EngineInputAnyKeyReleased;

	import.INPKBGetBufferStates = g_INP->INPKBGetBufferStates;
	import.INPKBGetAscii = g_INP->INPKBGetAscii;

	import.INPMouseSetBound = g_INP->INPMouseSetBound;
	import.INPMouseGetMove = g_INP->INPMouseGetMove;
	import.INPMouseGetLoc = g_INP->INPMouseGetLoc;

	import.INPGetKey = EngineInputGetKey;
	import.INPGetStr = EngineInputGetStr;
	import.INPGetName = EngineInputGetName;

	/////////////////////////// Graphics //////////////////////////////////
	import.GFXSetProj = g_GFX->GFXSetProj;
	import.GFXSetProjMtx = g_GFX->GFXSetProjMtx;
	import.GFXGetProjMtx = g_GFX->GFXGetProjMtx;
	import.GFXGetViewMtx = g_GFX->GFXGetViewMtx;
	import.GFXGetViewProjMtx = g_GFX->GFXGetViewProjMtx;
	import.GFXGetMode = g_GFX->GFXGetMode;
	import.GFXClear = g_GFX->GFXClear;
	import.GFXPickRay = g_GFX->GFXPickRay;
	import.GFXProjectPt = g_GFX->GFXProjectPt;
	import.GFXRenderBoundBox = g_GFX->GFXRenderBoundBox;

	import.FrustrumCheckPoint = g_GFX->FrustrumCheckPoint;
	import.FrustrumCheckSphere = g_GFX->FrustrumCheckSphere;
	import.FrustrumCheckCube = g_GFX->FrustrumCheckCube;
	import.FrustrumCheckBox = g_GFX->FrustrumCheckBox;
	import.FrustrumCheckBoxEX = g_GFX->FrustrumCheckBoxEX;

	import.TextureCreate = g_GFX->TextureCreate;
	import.TextureLoad = g_GFX->TextureLoad;
	import.TextureSearchName = g_GFX->TextureSearchName;
	import.TextureBegin = g_GFX->TextureBegin;
	import.TextureEnd = g_GFX->TextureEnd;
	
	import.SpriteCreate = g_GFX->SpriteCreate;
	import.SpriteLoad = g_GFX->SpriteLoad;
	
	import.FontCreateIndirect = g_GFX->FontCreateIndirect;
	import.FontCreate = g_GFX->FontCreate;
	
	import.EffectCreate = g_GFX->EffectCreate;
	import.EffectLoad = g_GFX->EffectLoad;
	import.EffectSearchName = g_GFX->EffectSearchName;
	
	import.LightEnableRender = g_GFX->LightEnableRender;
	import.LightSetAmbient = g_GFX->LightSetAmbient;
	import.LightGetAmbient = g_GFX->LightGetAmbient;
	import.LightAdd = g_GFX->LightAdd;
	import.LightGetDat = g_GFX->LightGetDat;
	import.LightSetDat = g_GFX->LightSetDat;
	import.LightSetPriority = g_GFX->LightSetPriority;
	import.LightRemove = g_GFX->LightRemove;
	import.LightRemoveAll = g_GFX->LightRemoveAll;
	import.LightGetNum = g_GFX->LightGetNum;

	import.MDLLoad = g_GFX->MDLLoad;
	import.MDLSearchName = g_GFX->MDLSearchName;

	import.OBJCreate = g_GFX->OBJCreate;
	
	import.GFXSceneGetNumTri = g_GFX->GFXSceneGetNumTri;
	import.GFXSceneClear = g_GFX->GFXSceneClear;
	import.GFXSceneRender = g_GFX->GFXSceneRender;
		
	import.FogEnable = g_GFX->FogEnable;
	import.FogIsEnable = g_GFX->FogIsEnable;
	import.FogSetMode = g_GFX->FogSetMode;
	import.FogSetColor = g_GFX->FogSetColor;
	import.FogSetStart = g_GFX->FogSetStart;
	import.FogSetEnd = g_GFX->FogSetEnd;
	import.FogSetDensity = g_GFX->FogSetDensity;
	
	import.GFXScreenCapture = g_GFX->GFXScreenCapture;
	import.GFXScreenDisplay = g_GFX->GFXScreenDisplay;

	import.ParSysExist = g_GFX->ParSysExist;
	import.ParSysLoad = g_GFX->ParSysLoad;
	import.ParSysShutdown = g_GFX->ParSysShutdown;
	import.ParSysUpdate = g_GFX->ParSysUpdate;
	import.ParSysRender = g_GFX->ParSysRender;
	import.ParSysStart = g_GFX->ParSysStart;
	import.ParSysGetBound = g_GFX->ParSysGetBound;
	import.ParSysSetDir = g_GFX->ParSysSetDir;

	import.LineDraw = g_GFX->LineDraw;
	import.LineDraw3D = g_GFX->LineDraw3D;
	import.LineGetAntiAlias = g_GFX->LineGetAntiAlias;
	import.LineSetAntiAlias = g_GFX->LineSetAntiAlias;
	import.LineGetPattern = g_GFX->LineGetPattern;
	import.LineSetPattern = g_GFX->LineSetPattern;
	import.LineGetPatternScale = g_GFX->LineGetPatternScale;
	import.LineSetPatternScale = g_GFX->LineSetPatternScale;
	import.LineGetWidth = g_GFX->LineGetWidth;
	import.LineSetWidth = g_GFX->LineSetWidth;

	/////////////////////////// Audio Imports //////////////////////////////////
	import.SoundSetVolume = EngineSoundSetVolume;
	import.SoundGetVolume = EngineSoundGetVolume;
	import.SoundSysLoad = EngineSoundSysLoad;
	import.SoundSysDestroy = EngineSoundSysDestroy;
	import.SoundSysGetID = EngineSoundSysGetID;
	import.SoundIsValid = EngineSoundIsValid;
	import.SoundPlay = EngineSoundPlay;
	import.SoundSet = EngineSoundSet;
	import.SoundPause = EngineSoundPause;
	import.SoundIsPause = EngineSoundIsPause;
	import.SoundStop = EngineSoundStop;
	import.SoundStopAll = EngineSoundStopAll;
	import.SoundSetListener = EngineSoundSetListener;
	import.SoundUpdate = EngineSoundUpdate;

	import.MusicGetVolume = EngineMusicGetVolume;
	import.MusicSetVolume = EngineMusicSetVolume;
	import.MusicPlay = EngineMusicPlay;
	import.MusicStop = EngineMusicStop;

	/////////////////////////// Game Engine //////////////////////////////////
	import.EngineGetTick = EngineGetTick;
	import.EngineGetFPS = EngineGetFPS;
	import.EngineGetAvgFPS = EngineGetAvgFPS;

	import.EngineFlagSet = EngineFlagSet;
	import.EngineFlagCheck = EngineFlagCheck;

	import.EngineVideoPlay = EngineVideoPlay;

	import.EngineQuit = WinQuit;

	import.EngineLoadDisplay = EngineLoadDisplay;
	import.EngineLoadDisplayText = EngineLoadDisplayText;

	import.BindGetBuffer = BindGetBuffer;
	import.BindUnSetAllKey = BindUnSetAllKey;
	import.BindUnSetKey = BindUnSetKey;
	import.BindSetKey = BindSetKey;
	import.BindGetKey = BindGetKey;
	import.BindExecute = BindExecute;

	import.ValueSetFlags = ValueSetFlags;
	import.ValueGetFlags = ValueGetFlags;
	import.ValueExist = ValueExist;
	import.ValueSetStr = ValueSetStr;
	import.ValueSetFloat = ValueSetFloat;
	import.ValueGetStr = ValueGetStr;
	import.ValueGetFloat = ValueGetFloat;

	import.CmdAdd = CmdAdd;
	import.CmdExist = CmdExist;
	import.CmdCall = CmdCall;
	import.CmdCallStr = CmdCallStr;
	import.CmdPost = CmdPost;
	import.CmdPostStr = CmdPostStr;
	import.CmdExecFile = CmdExecFile;
	import.CmdClear = CmdClear;
	import.CmdArgC = CmdArgC;
	import.CmdArgV = CmdArgV;

	import.ViewCreate = ViewCreate;

	import.EntCreate = EntCreate;

	import.WorldGetMap = WorldGetMap;
	import.WorldLoad = WorldPostMap;
	import.WorldGetPostMapPath = WorldGetPostMapPath;
	import.WorldLoadPostMap = WorldLoadPostMap;
	import.WorldDestroy = WorldDestroy;
	import.WorldSetGravity = WorldSetGravity;
	import.WorldGetGravity = WorldGetGravity;
	import.WorldAddEntity = WorldAddEntity;
	import.WorldRemoveEntity = WorldRemoveEntity;
	import.WorldEntityExist = WorldEntityExist;
	import.WorldEnumEntities = WorldEnumEntities;
	import.WorldUpdate = WorldUpdate;
	import.WorldPreview = WorldPreview;
	import.WorldPreviewEx = WorldPreviewEx;
	import.WorldDisplay = WorldDisplay;

	import.WorldTargetSet = WorldTargetSet;
	import.WorldTargetGet = WorldTargetGet;

	import.WorldWPExist = WorldWPExist;
	import.WorldWPSet = WorldWPSet;
	import.WorldWPGet = WorldWPGet;

	import.WPNodeCreate = WPNodeCreate;

	import.SkyLoad = SkyLoad;
	import.SkyDestroy = SkyDestroy;
	import.SkyRender = SkyRender;
	
	g_ge = (GameExport *)EngineModLoad(MODULE_GAME, &import);

	if(!g_ge)
		return FALSE;

	//check the version defined from game.h
	//it better match!
	if(g_ge->version != GAME_VERSION)
	{ 
		tCHAR buff[MAXCHARBUFF]; 
		swprintf(buff, L"game.dll is version %d, not %d", g_ge->version, GAME_VERSION); 
		LogMsg(LOG_FILE, L"ModuleGameInit", buff); ModuleGameDestroy(); 
		return FALSE; 
	}

	//Initialize game module.
	if(g_ge->Init) //this should never be NULL
		g_ge->Init();

	return TRUE;
}