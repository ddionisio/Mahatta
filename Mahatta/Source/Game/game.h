#ifndef _game_h
#define _game_h

#define	GAME_VERSION	1

// This is the header for game dll, visible to engine/server

//
// For parsing Map Entities
//

class EntityParse {	//this is used for loading entities
public:
	EntityParse() {}
	~EntityParse() {}

	struct EP_Less {
		bool operator () (std::wstring const & _L, std::wstring const & _R) const
		{
			return wcsicmp(_L.c_str(), _R.c_str()) < 0;
		}
	};

	//add a variable with given value
	void AddVar(const tCHAR *lVal, const tCHAR *rVal)
	{
		if (m_vars.find(lVal) == m_vars.end()) m_vars[lVal] = rVal;
	}

	//get value of variable (0 if not found)
	const tCHAR *GetVal(const tCHAR *varName) const
	{
		std::map<std::wstring, std::wstring, EP_Less>::const_iterator vi = m_vars.find(varName);

		return vi != m_vars.end() ? vi->second.c_str() : 0;
	}

private:
	std::map<std::wstring, std::wstring, EP_Less> m_vars;
};

//
// functions provided by the main engine
//
typedef struct _GameImport
{
	//
	// STD I/O Imports
	//
	void * (*MemAlloc) (u64 size);
	void   (*MemFree) (void *mem);
	void * (*MemRealloc) (void *mem, u64 size);

	void (*LogClear) ();
	void (*LogPrintf) (u32 flags, const tCHAR *str, ...);
	void (*LogMsg) (u32 flags, const tCHAR *header, const tCHAR *msg);

	u8   (*FSGetPath) (const tCHAR *filepath, tCHAR *strOut, u32 size);
	hFILE (*FSOpen) (const tCHAR *filepath, const tCHAR *mode);
	void (*FSClose) (hFILE fp);
	u64  (*FSRead) (void *buffer, u64 size, u64 count, hFILE fp);
	u64  (*FSWrite) (void *buffer, u64 size, u64 count, hFILE fp);
	u8   (*FSEOF) (hFILE fp);
	s32  (*FSGetChar) (hFILE fp);
	s32  (*FSPutChar) (s32 c, hFILE fp);
	s32  (*FSSeek) (hFILE fp, s32 offset, FS_SEEK origin);
	s32  (*FSGetPos) (hFILE fp, s64 *pos);
	s32  (*FSSetPos) (hFILE fp, s64 *pos);
	s32  (*FSPrintf) (hFILE fp, const tCHAR *str, ...);

	RETCODE (*ParserReadStringFile) (hFILE fp, tCHAR *buff, s32 buffSize, const tCHAR beginat, const tCHAR stopat);
	RETCODE (*ParserCountStringFile) (hFILE fp, s32 *numChar, const tCHAR beginat, const tCHAR stopat);
	tCHAR (*ParserReadWordFile) (hFILE fp, tCHAR *buff, s32 buffSize, const tCHAR stopat);
	tCHAR (*ParserCountWordFile) (hFILE fp, s32 *numChar, const tCHAR stopat);
	RETCODE (*ParserSkipCharFile) (hFILE fp, const tCHAR thing, BOOL returnFalseOnOthers);
	BOOL (*ParserSkipString) (hFILE fp, const tCHAR beginAt, const tCHAR stopAt, const tCHAR *string);

	RETCODE (*ParserReadStringBuff) (tCHAR **theBuff, tCHAR *buff, s32 buffSize, const tCHAR beginat, const tCHAR stopat);
	RETCODE (*ParserCountStringBuff) (const tCHAR *theBuff, s32 *numChar, const tCHAR beginat, const tCHAR stopat);
	tCHAR (*ParserReadWordBuff) (tCHAR **theBuff, tCHAR *buff, s32 buffSize, const tCHAR stopat);
	tCHAR (*ParserCountWordBuff) (const tCHAR *theBuff, s32 *numChar, const tCHAR stopat);
	RETCODE (*ParserSkipCharBuff) (tCHAR **theBuff, const tCHAR thing, BOOL returnFalseOnOthers);
	tCHAR * (*GetFilePart) (const tCHAR *PathName);
	tCHAR * (*GetExtension) (const tCHAR *PathName);
	void (*GetFilename) (const tCHAR *PathName, tCHAR *str, s32 strLen);
	void (*ConcatinateFilePath) (const tCHAR *path, tCHAR *relativePath, tCHAR *out);
	void (*ParserFindFiles) (tCHAR *dirname, tCHAR *pattern, BOOL bCheckSubDir, FINDCALLBACK callback, void *userDat);
	tCHAR * (*ParserGrabAllTextFile) (const tCHAR *filename);

	//
	// CFG/Script Import
	//
	hCFG (*CfgFileLoad) (const tCHAR *filename);

	//
	// Math Imports
	//
	void (F_API *Vec2DCatMullRom) (const Vec2D *v1, const Vec2D *v2, const Vec2D *v3, const Vec2D *v4, f32 s, Vec2D *vOut);
	void (F_API *Vec2DHermite) (const Vec2D *v1, const Vec2D *t1, const Vec2D *v2, const Vec2D *t2, f32 s, Vec2D *vOut);
	f32  (F_API *Vec2DNormal) (const Vec2D *v, Vec2D *vOut);
	void (F_API *Vec2DTransform) (const Vec2D *v, const Matrix *mtx, Vec4D *vOut);
	void (F_API *Vec2DTransformCoord) (const Vec2D *v, const Matrix *mtx, Vec2D *vOut);
	void (F_API *Vec2DTransformNormal) (const Vec2D *v, const Matrix *mtx, Vec2D *vOut);
	f32  (F_API *Vec2DGetAngle) (const Vec2D *v);
	void (F_API *Vec3DCatMullRom) (const Vec3D *v1, const Vec3D *v2, const Vec3D *v3, const Vec3D *v4, f32 s, Vec3D *vOut);
	void (F_API *Vec3DHermite) (const Vec3D *v1, const Vec3D *t1, const Vec3D *v2, const Vec3D *t2, f32 s, Vec3D *vOut);
	f32  (F_API *Vec3DNormal) (const Vec3D *v, Vec3D *vOut);
	void (F_API *Vec3DTransform) (const Vec3D *v, const Matrix *mtx, Vec4D *vOut);
	void (F_API *Vec3DTransformCoord) (const Vec3D *v, const Matrix *mtx, Vec3D *vOut);
	void (F_API *Vec3DTransformNormal) (const Vec3D *v, const Matrix *mtx, Vec3D *vOut);
	void (F_API *Vec3DPerpendicular) (const Vec3D *v, Vec3D *vOut);
	void (F_API *Vec3DMakeNormalVectors) (const Vec3D *forward, Vec3D *right, Vec3D *up);
	void (F_API *Vec3DRotatePointAroundVector) (const Vec3D *dir, const Vec3D *point, f32 rad, Vec3D *vOut);
	void (F_API *Vec4DCatMullRom) (const Vec4D *v1, const Vec4D *v2, const Vec4D *v3, const Vec4D *v4, f32 s, Vec4D *vOut);
	void (F_API *Vec4DHermite) (const Vec4D *v1, const Vec4D *t1, const Vec4D *v2, const Vec4D *t2, f32 s, Vec4D *vOut);
	f32  (F_API *Vec4DNormal) (const Vec4D *v, Vec4D *vOut);
	void (F_API *Vec4DCross) (const Vec4D *v1, const Vec4D *v2, const Vec4D *v3, Vec4D *vOut);
	void (F_API *Vec4DTransform) (const Vec4D *v, const Matrix *mtx, Vec4D *vOut);
	void (F_API *QuatLN) (const Quaternion *q, Quaternion *qOut);
	void (F_API *QuatExp) (const Quaternion *q, Quaternion *qOut);
	f32  (F_API *QuatNorm) (const Quaternion *q, Quaternion *qOut);
	void (F_API *QuatInv) (const Quaternion *q, Quaternion *qOut);
	void (F_API *QuatRotAxis) (const Vec3D *axis, f32 theta, Quaternion *qOut);
	void (F_API *QuatRotEuler) (const Vec3D *angles, Quaternion *qOut);
	void (F_API *QuatRotMtx) (const Matrix *mtx, Quaternion *qOut);
	void (F_API *QuatToAxisAngle) (const Quaternion *q, Vec3D *axisOut, f32 *thetaOut);
	void (F_API *QuatSlerp) (const Quaternion *q1, const Quaternion *q2, f32 t, Quaternion *qOut);
	void (F_API *QuatBaryCentric) (const Quaternion *q1, const Quaternion *q2, const Quaternion *q3, f32 f, f32 g, Quaternion *qOut);
	void (F_API *QuatSquadSetup) (const Quaternion *q0, const Quaternion *q1, const Quaternion *q2, const Quaternion *q3, Quaternion *aOut, Quaternion *bOut, Quaternion *cOut);
	void (F_API *QuatSquad) (const Quaternion *q1, const Quaternion *a, const Quaternion *b, const Quaternion *c, f32 t, Quaternion *qOut);
	void (F_API *PlaneFromPoints) (const Vec3D *pt1, const Vec3D *pt2, const Vec3D *pt3, Plane *plOut);
	void (F_API *PlaneNormalize) (const Plane *p, Plane *pOut);
	void (F_API *PlaneTransform) (const Matrix *mtx, const Plane *p, Plane *pOut);
	void (F_API *ProjectPointOnPlane) (const Vec3D *p, const Vec3D *norm, Vec3D *vOut);
	void (F_API *PlaneCategorize) (Plane *p);
	s16	 (F_API *PlaneTypeForNormal) (const Plane *p);
	s16  (F_API *SignbitsForPlane) (const Plane *p);
	s32  (F_API *BoxOnPlaneSide) (const Vec3D *mins, const Vec3D *maxs, const Plane *p);
	u8   (F_API *MtxInverse) (const Matrix *mtx, Matrix *mOut);
	f32  (F_API *MtxDeterminant) (const Matrix *mtx);
	void (F_API *MtxTranspose) (const Matrix *mtx, Matrix *mOut);
	void (F_API *MtxTranslate) (const Vec3D *pt, Matrix *mOut);
	void (F_API *MtxScale) (const Vec3D *scale, Matrix *mOut);
	void (F_API *MtxRotateX) (f32 angle, Matrix *mOut);
	void (F_API *MtxRotateY) (f32 angle, Matrix *mOut);
	void (F_API *MtxRotateZ) (f32 angle, Matrix *mOut);
	void (F_API *MtxYawPitchRoll) (f32 yaw, f32 pitch, f32 roll, Matrix *mOut);
	void (F_API *MtxAxisRotation) (const Vec3D *axis, f32 angle, Matrix *mOut);
	void (F_API *MtxQuaternion) (const Quaternion *q, Matrix *mOut);
	void (F_API *MtxOrtho) (f32 w, f32 h, f32 zn, f32 zf, Matrix *mOut);
	void (F_API *MtxOrthoOffCenter) (f32 l, f32 r, f32 t, f32 b, f32 zn, f32 zf, Matrix *mOut);
	void (F_API *MtxPerspective) (f32 w, f32 h, f32 zn, f32 zf, Matrix *mOut);
	void (F_API *MtxPerspectiveOffCenter) (f32 l, f32 r, f32 t, f32 b, f32 zn, f32 zf, Matrix *mOut);
	void (F_API *MtxPerspectiveFOV) (f32 fov, f32 aspect, f32 zn, f32 zf, Matrix *mOut);
	void (F_API *MtxLookAt) (const Vec3D *center, const Vec3D *dir, const Vec3D *up, Matrix *mOut);
	f32  (F_API *RayIntersectBox) (const Vec3D *p, const Vec3D *dir, const Vec3D *boxMin, const Vec3D *boxMax, Vec3D *nOut);
	u8   (F_API *BoundsIntersect) (const Vec3D & mins1, const Vec3D & maxs1, const Vec3D & mins2, const Vec3D & maxs2);
	void (F_API *BoundsClear) (Vec3D & mins, Vec3D & maxs);
	void (F_API *BoundsAddPoint) (const Vec3D & v, Vec3D & mins, Vec3D & maxs);
	f64  (F_API *RandDouble) ();
	f32  (F_API *RandRngFloat) (f32 a, f32 b);
	s32  (F_API *RandRngInt) (s32 a1, s32 a2);

	//
	// Input Imports
	//
	u8 (*INPGetKey) (const tCHAR *str, eInputType *deviceOut, u8 *kCodeOut);
	u8 (*INPGetStr) (eInputType device, u8 key, tCHAR *strOut, u32 size);
	u8 (*INPGetName)(eInputType device, u8 key, tCHAR *strOut, u32 size);

	u8 (*INPIsPressed) (eInputType device, u8 key);
	u8 (*INPIsReleased) (eInputType device, u8 key);
	u8 (*INPAnyKeyPressed) (eInputType device);
	u8 (*INPAnyKeyReleased) (eInputType device);

	u8 (*INPJStickAnyArrowReleased) ();
	u8 (*INPJStickAnyArrowPressed) ();

	void (F_API *INPMouseSetBound) (iPoint *pBound);
	void (F_API *INPMouseGetMove) (s32 *pX, s32 *pY, s32 *pZ);
	void (F_API *INPMouseGetLoc) (iPoint *pLoc);

	u8	  (F_API *INPKBGetBufferStates) (inpState buff[SAMPLE_BUFFER_SIZE]); //returns number of states
	tCHAR (F_API *INPKBGetAscii) (u8 kCode);

	//
	// Graphics Imports
	//

	void (F_API *GFXSetProj) (u32 width, u32 height, f32 fov);
	void (F_API *GFXSetProjMtx) (const Matrix * pMtx);
	void (F_API *GFXGetProjMtx) (Matrix *mOut);
	void (F_API *GFXGetViewMtx) (Matrix *mOut);
	void (F_API *GFXGetViewProjMtx) (Matrix *mOut);
	void (F_API *GFXGetMode) (gfxMode *pModeOut);
	u32  (F_API *GFXClear) (u32 flags, s32 numRect, const iRect* pRects, u32 color, f32 Z, u32 stencil);
	void (F_API *GFXPickRay) (s32 x, s32 y, Vec3D *pRayPos, Vec3D *pRayDir);
	void (F_API *GFXProjectPt) (const Vec3D *pLoc, Vec3D *pOut);
	void (F_API *GFXRenderBoundBox) (const Vec3D *min, const Vec3D *max, u32 clr);

	u8   (F_API *FrustrumCheckPoint) (f32 x, f32 y, f32 z);
	u8   (F_API *FrustrumCheckSphere) (f32 x, f32 y, f32 z, f32 r);
	u8   (F_API *FrustrumCheckCube) (f32 x, f32 y, f32 z, f32 s);
	u8   (F_API *FrustrumCheckBox) (const Vec3D & min, const Vec3D & max);
	s32  (F_API *FrustrumCheckBoxEX) (const Vec3D & min, const Vec3D & max);

	hTXT (F_API *TextureCreate) (const tCHAR *name, u32 width, u32 height, u32 mipLevels, u32 usage, GFXFORMAT fmt, TXTCSMCREATE func);
	hTXT (F_API *TextureLoad) (const tCHAR *filename, u8 bClrKey, u32 srcClrKey);
	hTXT (F_API *TextureSearchName) (const tCHAR *name);
	void (F_API *TextureBegin) ();
	void (F_API *TextureEnd) ();

	hSPRITE (F_API *SpriteCreate) (hTXT txt, u32 numFrame, const iPoint *frameSize, f32 delay, u32 numImgCol, const iPoint *imgOffset, u32 numState, const gfxSprFrame *states);
	hSPRITE (F_API *SpriteLoad) (hTXT txt, u32 numImageCol, const iPoint *imgOffset, const tCHAR *filename);
	
	hFNT (F_API *FontCreateIndirect) (const gfxFontDesc *pDesc);
	hFNT (F_API *FontCreate) (const tCHAR *typeName, s32 size, u32 fmtFlag);

	hEFFECT (F_API *EffectCreate) (const tCHAR *name, const tCHAR *buff);
	hEFFECT (F_API *EffectLoad) (const tCHAR *filename);
	hEFFECT (F_API *EffectSearchName) (const tCHAR *name);
	
	void (F_API *LightEnableRender) (u8 bYes);
	void (F_API *LightSetAmbient) (u32 ambient);
	u32  (F_API *LightGetAmbient) ();
	hLIGHT (F_API *LightAdd) (const gfxLight *lightDat);
	void (F_API *LightGetDat) (hLIGHT l, gfxLight *lightDat);
	void (F_API *LightSetDat) (hLIGHT l, const gfxLight *lightDat);
	void (F_API *LightSetPriority) (hLIGHT l, f32 priority);
	void (F_API *LightRemove) (hLIGHT l);
	void (F_API *LightRemoveAll)();
	u32  (F_API *LightGetNum) ();

	hMDL (F_API *MDLLoad) (const tCHAR *filename);
	hMDL (F_API *MDLSearchName) (const tCHAR *name);

	hOBJ (F_API *OBJCreate) (hMDL mdl);

	u32  (F_API *GFXSceneGetNumTri) ();
	void (F_API *GFXSceneClear) ();
	u32  (F_API *GFXSceneRender) ();

	void (F_API *FogEnable) (u8 bYes);
	u8   (F_API *FogIsEnable) ();
	void (F_API *FogSetMode) (u32 mode);
	void (F_API *FogSetColor) (u32 clr);
	void (F_API *FogSetStart) (f32 val);
	void (F_API *FogSetEnd) (f32 val);
	void (F_API *FogSetDensity) (f32 val);

	u32 (F_API *GFXScreenCapture) ();
	u32 (F_API *GFXScreenDisplay) (f32 xOfs, f32 yOfs, gfxBlt *pBlt);

	u8 (F_API *ParSysExist) (const tCHAR *name);
	u32 (F_API *ParSysLoad) (const tCHAR *filename);
	void (F_API *ParSysShutdown) ();
	u8 (F_API *ParSysUpdate) (s32 id, gfxParInfo* info);
	void (F_API *ParSysRender) (s32 ID);
	s32 (F_API *ParSysStart) (const tCHAR *type);
	void (F_API *ParSysGetBound) (s32 id, Vec3D *pMin, Vec3D *pMax);
	void (F_API *ParSysSetDir) (s32 id, const Vec3D & dir);

	u32  (F_API *LineDraw) (const Vec2D *pVtxList, u32 vtxNum, u32 clr);
	u32  (F_API *LineDraw3D) (const Vec3D *pVtxList, u32 vtxNum, const Matrix *pMtx, u32 clr);
	u8   (F_API *LineGetAntiAlias) ();
	void (F_API *LineSetAntiAlias) (u8 bState);
	u32  (F_API *LineGetPattern) ();
	void (F_API *LineSetPattern) (u32 val);
	f32  (F_API *LineGetPatternScale) ();
	void (F_API *LineSetPatternScale) (f32 val);
	f32  (F_API *LineGetWidth) ();
	void (F_API *LineSetWidth) (f32 val);

	//
	// Audio Imports
	//

	void (*SoundSetVolume) (u8 vol);
	u8   (*SoundGetVolume) ();
	s32  (*SoundSysLoad) (const tCHAR *filename);
	void (*SoundSysDestroy) ();
	u8   (*SoundSysGetID) (const tCHAR *name, s32 *sndSysIDOut);
	u8   (*SoundIsValid) (s32 sndID);
	s32  (*SoundPlay) (s32 sndSysID, const Vec3D & pos);
	u8   (*SoundSet) (s32 sndID, const Vec3D & pos, const Vec3D & vel);
	u8   (*SoundPause) (s32 sndID, u8 bPause);
	u8   (*SoundIsPause) (s32 sndID);
	u8   (*SoundStop) (s32 sndID);
	u8   (*SoundStopAll) ();
	u8   (*SoundSetListener) (const Vec3D &pos, const Vec3D &vel, const Vec3D &fwd, const Vec3D &top);
	u8   (*SoundUpdate) ();

	u8   (*MusicGetVolume) ();
	void (*MusicSetVolume) (u8 vol);
	u8   (*MusicPlay) (const tCHAR *filename);
	void (*MusicStop) (f32 fadeOutTime);

	//
	// Game Engine Imports
	//
	f32 (*EngineGetTick) ();
	u32 (*EngineGetFPS) ();
	u32 (*EngineGetAvgFPS) ();

	void (*EngineFlagSet) (u32 bit, u8 bSet);
	u8 (*EngineFlagCheck) (u32 bits);

	s32 (*EngineVideoPlay) (const tCHAR *filename);

	void (*EngineQuit) ();

	void (*EngineLoadDisplay) (hTXT txt, f32 x, f32 y, u8 bClear);
	void (*EngineLoadDisplayText) (hFNT fnt, const tCHAR *str, ...);

	const tCHAR * (*BindGetBuffer) (eInputType device, u8 key);
	void (*BindUnSetAllKey) ();
	void (*BindUnSetKey) (eInputType device, u8 key);
	void (*BindSetKey) (eInputType device, u8 key, const tCHAR *str);
	u8   (*BindGetKey) (const tCHAR *param, eInputType *deviceOut, u8 *keyOut);
	void (*BindExecute) (void);

	void (*ValueSetFlags) (const tCHAR *name, u32 flag);
	u32 (*ValueGetFlags) (const tCHAR *name);
	u8 (*ValueExist) (const tCHAR *name);
	s32 (*ValueSetStr) (const tCHAR *name, const tCHAR *str);
	s32 (*ValueSetFloat) (const tCHAR *name, f32 val);
	s32 (*ValueGetStr) (const tCHAR *name, tCHAR *strOut, u32 size);
	s32 (*ValueGetFloat) (const tCHAR *name, f32 *fOut);

	s32 (*CmdAdd) (const tCHAR *name, COMMANDPROC proc);
	u8 (*CmdExist) (const tCHAR *name);
	s32 (*CmdCall) (const tCHAR *name, const tCHAR *param);
	s32 (*CmdCallStr) (const tCHAR *str);
	s32 (*CmdPost) (const tCHAR *name, const tCHAR *param);
	s32 (*CmdPostStr) (const tCHAR *str);
	s32 (*CmdExecFile) (const tCHAR *filename);
	void (*CmdClear) ();
	u32 (*CmdArgC) ();
	tCHAR * (*CmdArgV) (u32 i);

	hVIEW (*ViewCreate) (const Vec3D & pt, const Vec3D & target);

	hENTITY (*EntCreate) (const tCHAR *name, u32 type, const tCHAR *modelFile, const EntityVFTable &vfTable, void *initDat);
	
	hQBSP (*WorldGetMap) ();
	s32 (*WorldLoad) (const tCHAR *filename);
	const tCHAR * (*WorldGetPostMapPath) ();
	s32 (*WorldLoadPostMap)();
	void (*WorldDestroy) ();
	void (*WorldSetGravity) (const Vec3D & v);
	Vec3D (*WorldGetGravity) ();
	void (*WorldAddEntity) (hENTITY ent);
	void (*WorldRemoveEntity) (hENTITY ent);
	u8 (*WorldEntityExist) (hENTITY ent);
	s32 (*WorldUpdate) ();
	void (*WorldPreview) (hENTITY ent);
	void (*WorldPreviewEx) (hQBSP qbsp, std::list<hENTITY> & entities);
	s32 (*WorldDisplay) (const Vec3D & eye);
	void (*WorldEnumEntities)(void (*Enum)(hENTITY, void*), void * context);

	void (*WorldTargetSet) (const tCHAR *name, const Vec3D & pt);
	u8 (*WorldTargetGet) (const tCHAR *name, Vec3D *ptOut);

	u8 (*WorldWPExist) (const tCHAR *name);
	void (*WorldWPSet) (const tCHAR *name, const wayPt & wp);
	u8 (*WorldWPGet) (const tCHAR *name, wayPt * wpOut);

	hWPNODE (*WPNodeCreate) (const tCHAR *wpName);

	u32 (*SkyLoad) (const tCHAR *modelfile);
	void (*SkyDestroy) ();
	void (*SkyRender) ();

} GameImport;

//
// functions exported by game.dll
//
typedef struct _GameExport
{
	s32 version;

	void (*Init) ();
	void (*Destroy) ();

	// each new level entered will cause a call to SpawnEntity for all entities
	void (*SpawnEntity) (const EntityParse & entDat);

	// Read/Write Game is for storing persistant cross level information
	// about the world state and the clients.
	// WriteGame is called every time a level is exited.
	// ReadGame is called on a loadgame.
	void		(*WriteGame) (const tCHAR *filename, u8 bAutosave);
	void		(*ReadGame) (const tCHAR *filename);

	// ReadLevel is called after the default map information has been
	// loaded
	void		(*WriteLevel) (const tCHAR *filename);
	void		(*ReadLevel) (const tCHAR *filename);

	void (*UpdateFrame) ();

	void (*RenderFrame) ();
} GameExport;

#endif