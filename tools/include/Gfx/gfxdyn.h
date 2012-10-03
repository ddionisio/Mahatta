#ifndef _gfxdyn_h
#define _gfxdyn_h

#ifdef WIN32
  #include <windows.h>
#else
  #include <dlfcn.h>
  #include <string.h>
#endif

#include "GFX\gfx.h"

typedef struct
{
    void           *module;

	void (F_API *GFXSetProj) (u32 width, u32 height, f32 fov);
	void (F_API *GFXSetProjMtx) (const Matrix * pMtx);
	void (F_API *GFXGetProjMtx) (Matrix *mOut);
	void (F_API *GFXGetViewMtx) (Matrix *mOut);
	void (F_API *GFXGetViewProjMtx) (Matrix *mOut);
	void (F_API *GFXSetFS) (hFMAIN fs);
	u32  (F_API *GFXGetModeCount) (GFXFORMAT fmt);
	u8   (F_API *GFXEnumMode) (GFXFORMAT fmt, u32 ind, gfxMode *pModeOut);
	u32  (F_API *GFXInit) (void *handle, gfxMode *pMode);
	void (F_API *GFXGetMode) (gfxMode *pModeOut);
	u32  (F_API *GFXSetMode) (gfxMode *pMode);
	u32  (F_API *GFXReset) ();
	void (F_API *GFXDestroy) ();
	u32  (F_API *GFXClear) (u32 flags, s32 numRect, const iRect* pRects, u32 color, f32 Z, u32 stencil);
	void (F_API *GFXBegin) ();
	void (F_API *GFXEnd) ();
	u32  (F_API *GFXPresent) (void *handle);
	void (F_API *GFXUpdate) (f32 time);
	void (F_API *GFXPickRay) (s32 x, s32 y, Vec3D *pRayPos, Vec3D *pRayDir);
	void (F_API *GFXProjectPt) (const Vec3D *pLoc, Vec3D *pOut);
	void (F_API *GFXUpdateClientRect) (const iRect *pRect);
	void (F_API *GFXRenderBoundBox) (const Vec3D *min, const Vec3D *max, u32 clr);
	u32  (F_API *GFXSetTextureLimit) (s32 min, s32 max);

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

	void (F_API *CameraInit) (gfxCamera *pCam, const Vec3D & vEye, f32 yaw, f32 pitch, f32 roll);
	void (F_API *CameraSetScene) (gfxCamera *pCam);
	void (F_API *CameraMov) (gfxCamera *pCam, const Vec3D & amt);
	void (F_API *CameraSet) (gfxCamera *pCam, const Vec3D & vEye);
	void (F_API *CameraSetTarget) (gfxCamera *pCam, const Vec3D & vTarget);
	void (F_API *CameraMovDir) (gfxCamera *pCam, f32 amt);
	void (F_API *CameraYawPitchRoll) (gfxCamera *pCam, f32 yaw, f32 pitch, f32 roll);

	u8   (F_API *FrustrumCheckPoint) (f32 x, f32 y, f32 z);
	u8   (F_API *FrustrumCheckSphere) (f32 x, f32 y, f32 z, f32 r);
	u8   (F_API *FrustrumCheckCube) (f32 x, f32 y, f32 z, f32 s);
	u8   (F_API *FrustrumCheckBox) (const Vec3D & min, const Vec3D & max);
	s32  (F_API *FrustrumCheckBoxEX) (const Vec3D & min, const Vec3D & max);

	hTXT (F_API *TextureCreate) (const tCHAR *name, u32 width, u32 height, u32 mipLevels, u32 usage, GFXFORMAT fmt, TXTCSMCREATE func);
	hTXT (F_API *TextureLoad) (const tCHAR *filename, u8 bClrKey, u32 srcClrKey);
	hTXT (F_API *TextureSearchName) (const tCHAR *name);
	void (F_API *TextureSet) (hTXT txt, u32 base);
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
	void (F_API *LightSetCullCallback) (LIGHTCULLFUNC lCB);

	hMDL (F_API *MDLLoad) (const tCHAR *filename);
	hMDL (F_API *MDLSearchName) (const tCHAR *name);

	hOBJ (F_API *OBJCreate) (hMDL mdl);

	u8	  (F_API *QBSPIsValid) (const tCHAR *filename);
	hQBSP (F_API *QBSPLoad) (const tCHAR *filename, const tCHAR *txtPath, f32 curveDetail, ENTITYPARSEFUNC parseFunc, void *userDat);

	u32  (F_API *GFXSceneGetNumTri) ();
	void (F_API *GFXSceneClear) ();
	u32  (F_API *GFXSceneRender) ();

	u8   (F_API *SkyBoxBegin) ();
	void (F_API *SkyBoxEnd) ();

	void (F_API *FogEnable) (u8 bYes);
	u8   (F_API *FogIsEnable) ();
	void (F_API *FogSetMode) (u32 mode);
	void (F_API *FogSetColor) (u32 clr);
	void (F_API *FogSetStart) (f32 val);
	void (F_API *FogSetEnd) (f32 val);
	void (F_API *FogSetDensity) (f32 val);

	u32 (F_API *GFXScreenCapture) ();
	u32 (F_API *GFXScreenDisplay) (f32 xOfs, f32 yOfs, gfxBlt *pBlt);

	hBKFX (F_API *BKFXCreate) (u32 type, f32 delay, void *initStruct);
	s32  (F_API *BKFXUpdate) (hBKFX bkFX);
	s32  (F_API *BKFXDisplay) (hBKFX bkFX);
	void (F_API *BKFXDestroy) (hBKFX bkFX);

	void (F_API *ParSysSetCollisionFunc) (PARCOLLFUNC func);
	u8 (F_API *ParSysExist) (const tCHAR *name);
	u32 (F_API *ParSysLoad) (const tCHAR *filename);
	void (F_API *ParSysShutdown) ();
	void (F_API *ParSysRender) (s32 ID);
	void (F_API *ParSysClear) ();
	u8 (F_API *ParSysUpdate) (s32 id, gfxParInfo* info);
	s32 (F_API *ParSysStart) (const tCHAR *type);
	void (F_API *ParSysGetBound) (s32 id, Vec3D *pMin, Vec3D *pMax);
	void (F_API *ParSysSetDir) (s32 id, const Vec3D & dir);

	u32 (F_API *GFXShadowSetShade) (u32 color);

	s32 (F_API *GFXVideoPlay) (HMAIN hwnd, const tCHAR *filename);

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
	
} GFX_INSTANCE;

static GFX_INSTANCE *GFXCreateInstance(LPCSTR dllName)
{
    GFX_INSTANCE *instance;

    instance = (GFX_INSTANCE *)calloc(sizeof(GFX_INSTANCE), 1);
    if (!instance)
    {
        return NULL;
    }

#ifdef WIN32
    instance->module = LoadLibrary(dllName);
#else
    instance->module = dlopen(dllName, RTLD_GLOBAL | RTLD_LAZY);
#endif
    if (!instance->module)
    {
        free(instance);
        return NULL;
    }

	MOD_GETPROC(GFXSetProj, "GFXSetProj");
	MOD_GETPROC(GFXSetProjMtx, "GFXSetProjMtx");
	MOD_GETPROC(GFXGetProjMtx, "GFXGetProjMtx");
	MOD_GETPROC(GFXGetViewMtx, "GFXGetViewMtx");
	MOD_GETPROC(GFXGetViewProjMtx, "GFXGetViewProjMtx");
	
	MOD_GETPROC(GFXSetFS, "GFXSetFS");
	MOD_GETPROC(GFXGetModeCount, "GFXGetModeCount");
	MOD_GETPROC(GFXEnumMode, "GFXEnumMode");
	MOD_GETPROC(GFXInit, "GFXInit");
	MOD_GETPROC(GFXGetMode, "GFXGetMode");
	MOD_GETPROC(GFXSetMode, "GFXSetMode");
	MOD_GETPROC(GFXReset, "GFXReset");
	MOD_GETPROC(GFXDestroy, "GFXDestroy");
	MOD_GETPROC(GFXClear, "GFXClear");
	MOD_GETPROC(GFXBegin, "GFXBegin");
	MOD_GETPROC(GFXEnd, "GFXEnd");
	MOD_GETPROC(GFXPresent, "GFXPresent");
	MOD_GETPROC(GFXUpdate, "GFXUpdate");
	MOD_GETPROC(GFXPickRay, "GFXPickRay");
	MOD_GETPROC(GFXProjectPt, "GFXProjectPt");
	MOD_GETPROC(GFXUpdateClientRect, "GFXUpdateClientRect");
	MOD_GETPROC(GFXRenderBoundBox, "GFXRenderBoundBox");
	MOD_GETPROC(GFXSetTextureLimit, "GFXSetTextureLimit");
	
	MOD_GETPROC(Vec2DCatMullRom, "Vec2DCatMullRom");
	MOD_GETPROC(Vec2DHermite, "Vec2DHermite");
	MOD_GETPROC(Vec2DNormal, "Vec2DNormal");
	MOD_GETPROC(Vec2DTransform, "Vec2DTransform");
	MOD_GETPROC(Vec2DTransformCoord, "Vec2DTransformCoord");
	MOD_GETPROC(Vec2DTransformNormal, "Vec2DTransformNormal");
	MOD_GETPROC(Vec2DGetAngle, "Vec2DGetAngle");
	
	MOD_GETPROC(Vec3DCatMullRom, "Vec3DCatMullRom");
	MOD_GETPROC(Vec3DHermite, "Vec3DHermite");
	MOD_GETPROC(Vec3DNormal, "Vec3DNormal");
	MOD_GETPROC(Vec3DTransform, "Vec3DTransform");
	MOD_GETPROC(Vec3DTransformCoord, "Vec3DTransformCoord");
	MOD_GETPROC(Vec3DTransformNormal, "Vec3DTransformNormal");
	MOD_GETPROC(Vec3DPerpendicular, "Vec3DPerpendicular");
	MOD_GETPROC(Vec3DMakeNormalVectors, "Vec3DMakeNormalVectors");
	MOD_GETPROC(Vec3DRotatePointAroundVector, "Vec3DRotatePointAroundVector");
	
	MOD_GETPROC(Vec4DCross, "Vec4DCross");
	MOD_GETPROC(Vec4DCatMullRom, "Vec4DCatMullRom");
	MOD_GETPROC(Vec4DHermite, "Vec4DHermite");
	MOD_GETPROC(Vec4DNormal, "Vec4DNormal");
	MOD_GETPROC(Vec4DTransform, "Vec4DTransform");
	
	MOD_GETPROC(QuatLN, "QuatLN");
	MOD_GETPROC(QuatExp, "QuatExp");
	MOD_GETPROC(QuatNorm, "QuatNorm");
	MOD_GETPROC(QuatInv, "QuatInv");
	MOD_GETPROC(QuatRotAxis, "QuatRotAxis");
	MOD_GETPROC(QuatRotEuler, "QuatRotEuler");
	MOD_GETPROC(QuatRotMtx, "QuatRotMtx");
	MOD_GETPROC(QuatToAxisAngle, "QuatToAxisAngle");
	MOD_GETPROC(QuatSlerp, "QuatSlerp");
	MOD_GETPROC(QuatBaryCentric, "QuatBaryCentric");
	MOD_GETPROC(QuatSquadSetup, "QuatSquadSetup");
	MOD_GETPROC(QuatSquad, "QuatSquad");
	
	MOD_GETPROC(PlaneFromPoints, "PlaneFromPoints");
	MOD_GETPROC(PlaneNormalize, "PlaneNormalize");
	MOD_GETPROC(PlaneTransform, "PlaneTransform");
	MOD_GETPROC(ProjectPointOnPlane, "ProjectPointOnPlane");
	
	MOD_GETPROC(PlaneCategorize, "PlaneCategorize");
	MOD_GETPROC(PlaneTypeForNormal, "PlaneTypeForNormal");
	MOD_GETPROC(SignbitsForPlane, "SignbitsForPlane");
	MOD_GETPROC(BoxOnPlaneSide, "BoxOnPlaneSide");
	
	MOD_GETPROC(MtxInverse, "MtxInverse");
	MOD_GETPROC(MtxDeterminant, "MtxDeterminant");
	MOD_GETPROC(MtxTranspose, "MtxTranspose");
	MOD_GETPROC(MtxTranslate, "MtxTranslate");
	MOD_GETPROC(MtxScale, "MtxScale");
	MOD_GETPROC(MtxRotateX, "MtxRotateX");
	MOD_GETPROC(MtxRotateY, "MtxRotateY");
	MOD_GETPROC(MtxRotateZ, "MtxRotateZ");
	MOD_GETPROC(MtxYawPitchRoll, "MtxYawPitchRoll");
	MOD_GETPROC(MtxAxisRotation, "MtxAxisRotation");
	MOD_GETPROC(MtxQuaternion, "MtxQuaternion");
	MOD_GETPROC(MtxOrtho, "MtxOrtho");
	MOD_GETPROC(MtxOrthoOffCenter, "MtxOrthoOffCenter");
	MOD_GETPROC(MtxPerspective, "MtxPerspective");
	MOD_GETPROC(MtxPerspectiveOffCenter, "MtxPerspectiveOffCenter");
	MOD_GETPROC(MtxPerspectiveFOV, "MtxPerspectiveFOV");
	MOD_GETPROC(MtxLookAt, "MtxLookAt");
	MOD_GETPROC(RandDouble, "RandDouble");
	MOD_GETPROC(RandRngFloat, "RandRngFloat");
	MOD_GETPROC(RandRngInt, "RandRngInt");
	MOD_GETPROC(RayIntersectBox, "RayIntersectBox");
	MOD_GETPROC(BoundsIntersect, "BoundsIntersect");
	MOD_GETPROC(BoundsClear, "BoundsClear");
	MOD_GETPROC(BoundsAddPoint, "BoundsAddPoint");
	
	MOD_GETPROC(CameraInit, "CameraInit");
	MOD_GETPROC(CameraSetScene, "CameraSetScene");
	MOD_GETPROC(CameraMov, "CameraMov");
	MOD_GETPROC(CameraSet, "CameraSet");
	MOD_GETPROC(CameraSetTarget, "CameraSetTarget");
	MOD_GETPROC(CameraMovDir, "CameraMovDir");
	MOD_GETPROC(CameraYawPitchRoll, "CameraYawPitchRoll");
	
	MOD_GETPROC(FrustrumCheckPoint, "FrustrumCheckPoint");
	MOD_GETPROC(FrustrumCheckSphere, "FrustrumCheckSphere");
	MOD_GETPROC(FrustrumCheckCube, "FrustrumCheckCube");
	MOD_GETPROC(FrustrumCheckBox, "FrustrumCheckBox");
	MOD_GETPROC(FrustrumCheckBoxEX, "FrustrumCheckBoxEX");
	
	MOD_GETPROC(TextureCreate, "TextureCreate");
	MOD_GETPROC(TextureLoad, "TextureLoad");
	MOD_GETPROC(TextureSearchName, "TextureSearchName");
	MOD_GETPROC(TextureSet, "TextureSet");
	MOD_GETPROC(TextureBegin, "TextureBegin");
	MOD_GETPROC(TextureEnd, "TextureEnd");
	
	MOD_GETPROC(SpriteCreate, "SpriteCreate");
	MOD_GETPROC(SpriteLoad, "SpriteLoad");
	
	MOD_GETPROC(FontCreateIndirect, "FontCreateIndirect");
	MOD_GETPROC(FontCreate, "FontCreate");
	
	MOD_GETPROC(EffectCreate, "EffectCreate");
	MOD_GETPROC(EffectLoad, "EffectLoad");
	MOD_GETPROC(EffectSearchName, "EffectSearchName");
	
	MOD_GETPROC(LightEnableRender, "LightEnableRender");
	MOD_GETPROC(LightSetAmbient, "LightSetAmbient");
	MOD_GETPROC(LightGetAmbient, "LightGetAmbient");
	MOD_GETPROC(LightAdd, "LightAdd");
	MOD_GETPROC(LightGetDat, "LightGetDat");
	MOD_GETPROC(LightSetDat, "LightSetDat");
	MOD_GETPROC(LightSetPriority, "LightSetPriority");
	MOD_GETPROC(LightRemove, "LightRemove");
	MOD_GETPROC(LightRemoveAll, "LightRemoveAll");
	MOD_GETPROC(LightGetNum, "LightGetNum");
	MOD_GETPROC(LightSetCullCallback, "LightSetCullCallback");
	
	MOD_GETPROC(MDLLoad, "MDLLoad");
	MOD_GETPROC(MDLSearchName, "MDLSearchName");
	
	MOD_GETPROC(OBJCreate, "OBJCreate");
	
	MOD_GETPROC(QBSPIsValid, "QBSPIsValid");
	MOD_GETPROC(QBSPLoad, "QBSPLoad");

	MOD_GETPROC(GFXSceneGetNumTri, "GFXSceneGetNumTri");
	MOD_GETPROC(GFXSceneClear, "GFXSceneClear");
	MOD_GETPROC(GFXSceneRender, "GFXSceneRender");
	
	MOD_GETPROC(SkyBoxBegin, "SkyBoxBegin");
	MOD_GETPROC(SkyBoxEnd, "SkyBoxEnd");
	
	MOD_GETPROC(FogEnable, "FogEnable");
	MOD_GETPROC(FogIsEnable, "FogIsEnable");
	MOD_GETPROC(FogSetMode, "FogSetMode");
	MOD_GETPROC(FogSetColor, "FogSetColor");
	MOD_GETPROC(FogSetStart, "FogSetStart");
	MOD_GETPROC(FogSetEnd, "FogSetEnd");
	MOD_GETPROC(FogSetDensity, "FogSetDensity");
	
	MOD_GETPROC(GFXScreenCapture, "GFXScreenCapture");
	MOD_GETPROC(GFXScreenDisplay, "GFXScreenDisplay");
	
	MOD_GETPROC(BKFXCreate, "BKFXCreate");
	MOD_GETPROC(BKFXUpdate, "BKFXUpdate");
	MOD_GETPROC(BKFXDisplay, "BKFXDisplay");
	MOD_GETPROC(BKFXDestroy, "BKFXDestroy");

	MOD_GETPROC(ParSysSetCollisionFunc, "ParSysSetCollisionFunc");
	MOD_GETPROC(ParSysExist, "ParSysExist");
	MOD_GETPROC(ParSysLoad, "ParSysLoad");
	MOD_GETPROC(ParSysShutdown, "ParSysShutdown");
	MOD_GETPROC(ParSysRender, "ParSysRender");
	MOD_GETPROC(ParSysClear, "ParSysClear");
	MOD_GETPROC(ParSysUpdate, "ParSysUpdate");
	MOD_GETPROC(ParSysStart, "ParSysStart");
	MOD_GETPROC(ParSysGetBound, "ParSysGetBound");
	MOD_GETPROC(ParSysSetDir, "ParSysSetDir");

	MOD_GETPROC(GFXShadowSetShade, "GFXShadowSetShade");

	MOD_GETPROC(GFXVideoPlay, "GFXVideoPlay");

	MOD_GETPROC(LineDraw, "LineDraw");
	MOD_GETPROC(LineDraw3D, "LineDraw3D");
	MOD_GETPROC(LineGetAntiAlias, "LineGetAntiAlias");
	MOD_GETPROC(LineSetAntiAlias, "LineSetAntiAlias");
	MOD_GETPROC(LineGetPattern, "LineGetPattern");
	MOD_GETPROC(LineSetPattern, "LineSetPattern");
	MOD_GETPROC(LineGetPatternScale, "LineSetPattern");
	MOD_GETPROC(LineSetPatternScale, "LineSetPatternScale");
	MOD_GETPROC(LineGetWidth, "LineGetWidth");
	MOD_GETPROC(LineSetWidth, "LineSetWidth");

	return instance;
}

static void GFXFreeInstance(GFX_INSTANCE *instance)
{
    if (instance)
    {
        if (instance->module)
        {
#ifdef WIN32
            FreeLibrary((HMODULE)instance->module);
#else
            dlclose(instance->module);
#endif
        }
        free(instance);
    }
}

#endif