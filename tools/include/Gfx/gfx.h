#ifndef _gfx_h
#define _gfx_h

#include "GFX\gfx_types.h"

/////////////////////////////////////////////////////////////////
// Functions
//

/////////////////////////////////////
// Name:	GFXSetProj
// Purpose:	set scene projection.
//			if width/height == 0,
//			then we use screen size
// Output:	projection set
// Return:	none
/////////////////////////////////////
DLL_API void F_API GFXSetProj(u32 width, u32 height, f32 fov);

/////////////////////////////////////
// Name:	GFXSetProjMtx
// Purpose:	set the projection matrix
// Output:	projection set
// Return:	none
/////////////////////////////////////
DLL_API void F_API GFXSetProjMtx(const Matrix * pMtx);

/////////////////////////////////////
// Name:	GFXGetProjMtx
// Purpose:	get the projection 
//			matrix
// Output:	mOut set
// Return:	none
/////////////////////////////////////
DLL_API void F_API GFXGetProjMtx(Matrix *mOut);

/////////////////////////////////////
// Name:	GFXGetViewMtx
// Purpose:	get the view 
//			matrix
// Output:	mOut set
// Return:	none
/////////////////////////////////////
DLL_API void F_API GFXGetViewMtx(Matrix *mOut);

/////////////////////////////////////
// Name:	GFXGetViewProjMtx
// Purpose:	get the view*projection
//			matrix
// Output:	mOut set
// Return:	none
/////////////////////////////////////
DLL_API void F_API GFXGetViewProjMtx(Matrix *mOut);

/////////////////////////////////////
// Name:	GFXSetFS
// Purpose:	set the GFX's File System
// Output:	added paths for loading
// Return:	none
/////////////////////////////////////
DLL_API void F_API GFXSetFS(hFMAIN fs);

/////////////////////////////////////
// Name:	GFXGetModeCount
// Purpose:	gives the number of resolutions
//			of the given format.  Only use:
//			GFXFMT_X8R8G8B8, GFXFMT_A8R8G8B8, 
//			GFXFMT_A2R10G10B10, GFXFMT_X1R5G5B5, 
//			GFXFMT_A1R5G5B5, and GFXFMT_R5G6B5
// Output:	none
// Return:	number of resolutions
/////////////////////////////////////
DLL_API u32 F_API GFXGetModeCount(GFXFORMAT fmt);

/////////////////////////////////////
// Name:	GFXEnumMode
// Purpose:	enumerate given format.
//			ind = [0, GFXGetModeCount(fmt)-1]
// Output:	pModeOut filled
// Return:	TRUE if mode is available
/////////////////////////////////////
DLL_API u8 F_API GFXEnumMode(GFXFORMAT fmt, u32 ind, gfxMode *pModeOut);

/////////////////////////////////////
// Name:	GFXInit
// Purpose:	initialize the graphics
//			if pMode is NULL, uses
//			window mode
// Output:	graphics initialized
// Return:	TRUE if success 
/////////////////////////////////////
DLL_API u32 F_API GFXInit(void *handle, gfxMode *pMode);

/////////////////////////////////////
// Name:	GFXGetMode
// Purpose:	get the current mode
// Output:	pModeOut filled
// Return:	none
/////////////////////////////////////
DLL_API void F_API GFXGetMode(gfxMode *pModeOut);

/////////////////////////////////////
// Name:	GFXSetMode
// Purpose:	set the graphics mode
// Output:	graphics mode set,
//			resets all shaders,
//			textures and models
// Return:	TRUE if success
/////////////////////////////////////
DLL_API u32 F_API GFXSetMode(gfxMode *pMode);

/////////////////////////////////////
// Name:	GFXReset
// Purpose:	reset the device
// Output:	resets all shaders,
//			textures and models
// Return:	TRUE if success
/////////////////////////////////////
DLL_API u32 F_API GFXReset();

/////////////////////////////////////
// Name:	GFXDestroy
// Purpose:	destroy the graphics
// Output:	graphics destroyed
// Return:	none
/////////////////////////////////////
DLL_API void F_API GFXDestroy();

/////////////////////////////////////
// Name:	GFXClear
// Purpose:	clears the screen depending
//			on given flags: GFXCLEAR_*
// Output:	screen cleared
// Return:	TRUE if success
/////////////////////////////////////
DLL_API u32 F_API GFXClear(u32 flags, s32 numRect, const iRect* pRects, 
						   u32 color, f32 Z, u32 stencil);

/////////////////////////////////////
// Name:	GFXBegin
// Purpose:	readies device to render
//			stuff.
// Output:	shit happens
// Return:	none
/////////////////////////////////////
DLL_API void F_API GFXBegin();

/////////////////////////////////////
// Name:	GFXEnd
// Purpose:	call this after finish
//			with rendering stuff
// Output:	more shit happens
// Return:	none
/////////////////////////////////////
DLL_API void F_API GFXEnd();

/////////////////////////////////////
// Name:	GFXPresent
// Purpose:	render the scene to screen
//			if handle is not null, then
//			scene is rendered to that
//			handle
// Output:	scene rendered
// Return:	TRUE if success
/////////////////////////////////////
DLL_API u32 F_API GFXPresent(void *handle);

/////////////////////////////////////
// Name:	GFXUpdate
// Purpose:	set the time elapse since
//			last update in milliseconds
//			Call this prior to all other
//			GFX updates and display
//			This will also check to see
//			if device has been lost,
//			if so, then we reset.
// Output:	gfx time elapse set
// Return:	none
/////////////////////////////////////
DLL_API void F_API GFXUpdate(f32 time);

/////////////////////////////////////
// Name:	GFXPickRay
// Purpose:	create a ray from given 
//			screen loc
// Output:	ray pos&dir is filled
// Return:	none
/////////////////////////////////////
DLL_API void F_API GFXPickRay(s32 x, s32 y, Vec3D *pRayPos, Vec3D *pRayDir);

/////////////////////////////////////
// Name:	GFXProjectPt
// Purpose:	project the given 3d loc.
//			to 2d screen space
// Output:	pOut set
// Return:	none
/////////////////////////////////////
DLL_API void F_API GFXProjectPt(const Vec3D *pLoc, Vec3D *pOut);

/////////////////////////////////////
// Name:	GFXUpdateClientRect
// Purpose:	update the client rect
//			(use this on WM_MOVE or
//			whenever the window is
//			changed
// Output:	client rect set
// Return:	none
/////////////////////////////////////
DLL_API void F_API GFXUpdateClientRect(const iRect *pRect);

/////////////////////////////////////
// Name:	GFXRenderBoundBox
// Purpose:	display AABB
// Output:	lines representing the
//			AABB displayed
// Return:	none
/////////////////////////////////////
DLL_API void F_API GFXRenderBoundBox(const Vec3D *min, const Vec3D *max, u32 clr);

/////////////////////////////////////
// Name:	GFXSetTextureLimit
// Purpose:	set the texture limitation
//			for size (2^n!!!!)
//			set -1 to both for no limit
// Output:	(for now, all currently
//			loaded textures are not reloaded)
//			So call this before loading 
//			anything.
// Return:	TRUE if success
/////////////////////////////////////
DLL_API u32 F_API GFXSetTextureLimit(s32 min, s32 max);

//
// Camera
//

/////////////////////////////////////
// Name:	CameraInit
// Purpose:	initialize camera
// Output:	given pCam initialized
// Return:	none
/////////////////////////////////////
DLL_API void F_API CameraInit(gfxCamera *pCam, const Vec3D & vEye, 
							  f32 yaw, f32 pitch, f32 roll);

/////////////////////////////////////
// Name:	CameraSetScene
// Purpose:	set the camera to scene
// Output:	view matrix set
// Return:	none
/////////////////////////////////////
DLL_API void F_API CameraSetScene(gfxCamera *pCam);

/////////////////////////////////////
// Name:	CameraMov
// Purpose:	move camera
// Output:	pCam moved
// Return:	none
/////////////////////////////////////
DLL_API void F_API CameraMov(gfxCamera *pCam, const Vec3D & amt);

/////////////////////////////////////
// Name:	CameraSet
// Purpose:	set camera's loc.
// Output:	pCam loc. set
// Return:	none
/////////////////////////////////////
DLL_API void F_API CameraSet(gfxCamera *pCam, const Vec3D & vEye);

/////////////////////////////////////
// Name:	CameraSetTarget
// Purpose:	set camera target
// Output:	set the camera target
// Return:	none
/////////////////////////////////////
DLL_API void F_API CameraSetTarget(gfxCamera *pCam, const Vec3D & vTarget);

/////////////////////////////////////
// Name:	CameraMovDir
// Purpose:	move camera to its dir.
// Output:	pCam moved
// Return:	none
/////////////////////////////////////
DLL_API void F_API CameraMovDir(gfxCamera *pCam, f32 amt);

/////////////////////////////////////
// Name:	CameraYawPitchRoll
// Purpose:	rotate camera with
//			yaw, pitch and roll
// Output:	camera set
// Return:	none
/////////////////////////////////////
DLL_API void F_API CameraYawPitchRoll(gfxCamera *pCam, f32 yaw, f32 pitch, f32 roll);

//
// Frustrum
//

/////////////////////////////////////
// Name:	FrustrumCheckPoint
// Purpose:	check point in frustrum
// Output:	none
// Return:	TRUE if so
/////////////////////////////////////
DLL_API u8 F_API FrustrumCheckPoint(f32 x, f32 y, f32 z);

/////////////////////////////////////
// Name:	FrustrumCheckSphere
// Purpose:	check sphere in frustrum
// Output:	none
// Return:	TRUE if so
/////////////////////////////////////
DLL_API u8 F_API FrustrumCheckSphere(f32 x, f32 y, f32 z, f32 r);

/////////////////////////////////////
// Name:	FrustrumCheckCube
// Purpose:	check cube in frustrum
// Output:	none
// Return:	TRUE if so
/////////////////////////////////////
DLL_API u8 F_API FrustrumCheckCube(f32 x, f32 y, f32 z, f32 s);

/////////////////////////////////////
// Name:	FrustrumCheckBox
// Purpose:	check box in frustrum
// Output:	none
// Return:	TRUE if so
/////////////////////////////////////
DLL_API u8 F_API FrustrumCheckBox(const Vec3D & min, const Vec3D & max);

/////////////////////////////////////
// Name:	FrustrumCheckBoxEX
// Purpose:	check box in frustrum
// Output:	none
// Return:	BBOX_IN
//			BBOX_OUT
//			BBOX_INTERSECT
/////////////////////////////////////
DLL_API s32 F_API FrustrumCheckBoxEX(const Vec3D & min, const Vec3D & max);

//
// Textures
//

/////////////////////////////////////
// Name:	TextureCreate
// Purpose:	create a texture with
//			given func, if not then
//			just a blank texture.
//			if fmt==GFXFMT_UNKNOWN, use gfx's
//			mode
// Output:	new texture added
// Return:	new texture
/////////////////////////////////////
DLL_API hTXT F_API TextureCreate(const tCHAR *name, u32 width, u32 height, u32 mipLevels, 
								 u32 usage, GFXFORMAT fmt, TXTCSMCREATE func);

/////////////////////////////////////
// Name:	TextureLoad
// Purpose:	load a texture from file
// Output:	new texture added
// Return:	new texture
/////////////////////////////////////
DLL_API hTXT F_API TextureLoad(const tCHAR *filename, u8 bClrKey, u32 srcClrKey);

/////////////////////////////////////
// Name:	TextureSearchName
// Purpose:	get texture by it's name
// Output:	none
// Return:	the texture if found
/////////////////////////////////////
DLL_API hTXT F_API TextureSearchName(const tCHAR *name);

/////////////////////////////////////
// Name:	TextureSet
// Purpose:	sets texture for texture
//			mapping.  Base is usually
//			0.
// Output:	texture is set
// Return:	none
/////////////////////////////////////
DLL_API void F_API TextureSet(hTXT txt, u32 base);

/////////////////////////////////////
// Name:	TextureBegin
// Purpose:	before blting anything
//			you need to call this
// Output:	textures ready to be
//			blted
// Return:	none
/////////////////////////////////////
DLL_API void F_API TextureBegin();

/////////////////////////////////////
// Name:	TextureEnd
// Purpose:	after you're done blting
//			textures, call this
// Output:	
// Return:	none
/////////////////////////////////////
DLL_API void F_API TextureEnd();

//
// Sprites
//

/////////////////////////////////////
// Name:	SpriteCreate
// Purpose:	creates a sprite with txt.
//			NOTE: states has to be allocated with
//				  size numState.
// Output:	none
// Return:	new sprite created
/////////////////////////////////////
DLL_API hSPRITE F_API SpriteCreate(hTXT txt, u32 numFrame, const iPoint *frameSize, 
							f32 delay, u32 numImgCol, const iPoint *imgOffset,
							u32 numState, const gfxSprFrame *states);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
DLL_API hSPRITE F_API SpriteLoad(hTXT txt, u32 numImageCol, 
							   const iPoint *imgOffset, const tCHAR *filename);

//
// Fonts
//

/////////////////////////////////////
// Name:	FontCreateIndirect
// Purpose:	create font with given
//			description
// Output:	new font created
// Return:	the new font
/////////////////////////////////////
DLL_API hFNT F_API FontCreateIndirect(const gfxFontDesc *pDesc);

/////////////////////////////////////
// Name:	FontCreate
// Purpose:	create a font with given
//			type and format
// Output:	font created
// Return:	new font
/////////////////////////////////////
DLL_API hFNT F_API FontCreate(const tCHAR *typeName, s32 size, u32 fmtFlag);

//
// Effects
//

/////////////////////////////////////
// Name:	EffectCreate
// Purpose:	create a new effect with
//			given NULL terminated
//			buffer
// Output:	new effect created
// Return:	new effect
/////////////////////////////////////
DLL_API hEFFECT F_API EffectCreate(const tCHAR *name, const s8 *buff);

/////////////////////////////////////
// Name:	EffectLoad
// Purpose:	create a new effect from
//			file
// Output:	new effect created
// Return:	new effect
/////////////////////////////////////
DLL_API hEFFECT F_API EffectLoad(const tCHAR *filename);

/////////////////////////////////////
// Name:	EffectSearchName
// Purpose:	find effect based on name
//			(filepath for loaded fxs)
// Output:	none
// Return:	the effect if found
/////////////////////////////////////
DLL_API hEFFECT F_API EffectSearchName(const tCHAR *name);

//
// Lights
//

/////////////////////////////////////
// Name:	LightEnableRender
// Purpose:	enable/disable lighting
// Output:	light enabled/disabled
// Return:	none
/////////////////////////////////////
DLL_API void F_API LightEnableRender(u8 bYes);

/////////////////////////////////////
// Name:	Light(Set/Get)Ambient
// Purpose:	set/get the ambient
// Output:	ambient if 'Get'
// Return:	none
/////////////////////////////////////
DLL_API void F_API LightSetAmbient(u32 ambient);
DLL_API u32 F_API LightGetAmbient();

/////////////////////////////////////
// Name:	LightAdd
// Purpose:	add a light to the light
//			list.
// Output:	new light added
// Return:	light pointer that you
//			can manipulate
/////////////////////////////////////
DLL_API hLIGHT F_API LightAdd(const gfxLight *lightDat);

/////////////////////////////////////
// Name:	LightGetDat
// Purpose:	get data of given light
// Output:	lightDat set
// Return:	none
/////////////////////////////////////
DLL_API void F_API LightGetDat(hLIGHT l, gfxLight *lightDat);

/////////////////////////////////////
// Name:	LightSetDat
// Purpose:	set the light data
// Output:	light data set
// Return:	none
/////////////////////////////////////
DLL_API void F_API LightSetDat(hLIGHT l, const gfxLight *lightDat);

/////////////////////////////////////
// Name:	LightSetPriority
// Purpose:	set the light priority for
//			shadows (0 is lowest)
// Output:	light priority set
// Return:	none
/////////////////////////////////////
DLL_API void F_API LightSetPriority(hLIGHT l, f32 priority);

/////////////////////////////////////
// Name:	LightRemove
// Purpose:	destroy given light and
//			remove it from list.
// Output:	given light destroyed
//			and light list updated
// Return:	none
/////////////////////////////////////
DLL_API void F_API LightRemove(hLIGHT l);

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
DLL_API void F_API LightRemoveAll();

/////////////////////////////////////
// Name:	LightGetNum
// Purpose:	get number of lights
// Output:	none
// Return:	number of lights on list
/////////////////////////////////////
DLL_API u32 F_API LightGetNum();

/////////////////////////////////////
// Name:	LightSetCullCallback
// Purpose:	set the light culling
//			function
// Output:	cull function set
// Return:	none
/////////////////////////////////////
DLL_API void F_API LightSetCullCallback(LIGHTCULLFUNC lCB);

//
// Models
//

/////////////////////////////////////
// Name:	MDLLoad
// Purpose:	load a model from given
//			filename
// Output:	model loaded
// Return:	new model
/////////////////////////////////////
DLL_API hMDL F_API MDLLoad(const tCHAR *filename);

/////////////////////////////////////
// Name:	MDLSearchName
// Purpose:	search model by name
// Output:	none
// Return:	the model
/////////////////////////////////////
DLL_API hMDL F_API MDLSearchName(const tCHAR *name);

//
// Objects
//

/////////////////////////////////////
// Name:	OBJCreate
// Purpose:	create object with given
//			model
// Output:	new object created
// Return:	new object
/////////////////////////////////////
DLL_API hOBJ F_API OBJCreate(hMDL mdl);

//
// QBSP map
//

/////////////////////////////////////
// Name:	QBSPIsValid
// Purpose:	quickly check if given file
//			exists and is a valid 
//			QBSP map.
// Output:	none
// Return:	TRUE if exist and valid
/////////////////////////////////////
DLL_API u8 F_API QBSPIsValid(const tCHAR *filename);

/////////////////////////////////////
// Name:	QBSPLoad
// Purpose:	loads a .bsp file and
//			create a qbsp map
//			curveDetail == 0 for default
// Output:	none
// Return:	new qbsp, 0 if failure
/////////////////////////////////////
DLL_API hQBSP F_API QBSPLoad(const tCHAR *filename, const tCHAR *txtPath, 
							 f32 curveDetail, ENTITYPARSEFUNC parseFunc, void *userDat);

//
// Scene
//

/////////////////////////////////////
// Name:	GFXSceneGetNumTri
// Purpose:	get the number of triangles
//			that will be rendered, or
//			has rendered.  Call this
//			before GFXSceneClear().
// Output:	none
// Return:	number of tris.
/////////////////////////////////////
DLL_API u32 F_API GFXSceneGetNumTri();

/////////////////////////////////////
// Name:	GFXSceneClear
// Purpose:	clear the scene
// Output:	scene cleared
// Return:	none
/////////////////////////////////////
DLL_API void F_API GFXSceneClear();

/////////////////////////////////////
// Name:	GFXRenderScene
// Purpose:	render the scene with
//			provided models and
//			what-not
// Output:	scene rendered
// Return:	TRUE if success
/////////////////////////////////////
DLL_API u32 F_API GFXSceneRender();

//
// SkyBox
//

/////////////////////////////////////
// Name:	SkyBoxBegin
// Purpose:	prepares the skybox render 
//			system
//			(do not subsitute GFXBegin()
//			with this!!!)
//			Between SkyBoxBegin and SkyBoxEnd,
//			you call skybox rendering,
//			but not SCENE!!!
// Output:	sky box rendering prepared
// Return:	success if success
/////////////////////////////////////
DLL_API u8 F_API SkyBoxBegin();

/////////////////////////////////////
// Name:	SkyBoxEnd
// Purpose:	ends the skybox render system
// Output:	sky box rendering end
// Return:	success if success
/////////////////////////////////////
DLL_API void F_API SkyBoxEnd();

//
// Fog
//

/////////////////////////////////////
// Name:	FogEnable
// Purpose:	enable/disable the fog
// Output:	fog enabled/disabled
// Return:	none
/////////////////////////////////////
DLL_API void F_API FogEnable(u8 bYes);

/////////////////////////////////////
// Name:	FogIsEnable
// Purpose:	check to see if fog is
//			enabled
// Output:	none
// Return:	true if enabled
/////////////////////////////////////
DLL_API u8 F_API FogIsEnable();

/////////////////////////////////////
// Name:	
// Purpose:	set mode to:
//			 GFXFOG_NONE
//			 GFXFOG_EXP    (uses density)
//			 GFXFOG_EXP2   (uses density)
//			 GFXFOG_LINEAR (uses start/end)
//			note: vertex fog can only
//				  use linear
// Output:	mode set
// Return:	none
/////////////////////////////////////
DLL_API void F_API FogSetMode(u32 mode);

/////////////////////////////////////
// Name:	FogSetColor
// Purpose:	set color of fog
// Output:	color of fog is set
// Return:	none
/////////////////////////////////////
DLL_API void F_API FogSetColor(u32 clr);

/////////////////////////////////////
// Name:	FogSetStart
// Purpose:	set the start of fog,
//			used in linear mode
// Output:	fog start set
// Return:	none
/////////////////////////////////////
DLL_API void F_API FogSetStart(f32 val);

/////////////////////////////////////
// Name:	FogSetEnd
// Purpose:	set the end of fog,
//			used in linear mode
// Output:	fog end set
// Return:	none
/////////////////////////////////////
DLL_API void F_API FogSetEnd(f32 val);

/////////////////////////////////////
// Name:	FogSetDensity
// Purpose:	set the density of fog,
//			used in exp/exp2 mode
// Output:	fog density set
// Return:	none
/////////////////////////////////////
DLL_API void F_API FogSetDensity(f32 val);

//
// Screenshot
//

/////////////////////////////////////
// Name:	GFXScreenCapture
// Purpose:	take a snapshot of what
//			currently is on screen
// Output:	screen stuff filled
// Return:	success if success
/////////////////////////////////////
DLL_API u32 F_API GFXScreenCapture();

/////////////////////////////////////
// Name:	GFXScreenDisplay
// Purpose:	display the snapshot of
//			the screen
//			NOTE: call TextureBegin
//				  first!!!
// Output:	stuff displayed
// Return:	success if success
/////////////////////////////////////
DLL_API u32 F_API GFXScreenDisplay(f32 xOfs, f32 yOfs, gfxBlt *pBlt);

//
// Background FX
//

/////////////////////////////////////
// Name:	BKFXCreate
// Purpose:	creates a background FX
//			refer to: gfx_bkgrndFX.h
//			for different types.
// Output:	none
// Return:	the new background FX
/////////////////////////////////////
DLL_API hBKFX F_API BKFXCreate(u32 type, f32 delay, void *initStruct);

/////////////////////////////////////
// Name:	BKFXUpdate
// Purpose:	updates and displays the
//			background FX
// Output:	stuff displayed
// Return:	-1 if duration
//			is over or FX expired
/////////////////////////////////////
DLL_API s32 F_API BKFXUpdate(hBKFX bkFX);

/////////////////////////////////////
// Name:	BKFXDisplay
// Purpose:	display background FX
// Output:	stuff
// Return:	success
/////////////////////////////////////
DLL_API s32 F_API BKFXDisplay(hBKFX bkFX);

/////////////////////////////////////
// Name:	BKFXDestroy
// Purpose:	destroys given bkFX
// Output:	stuff destroyed
// Return:	none
/////////////////////////////////////
DLL_API void F_API BKFXDestroy(hBKFX bkFX);

//
// Particle System
//

/////////////////////////////////////
// Name:	ParSysSetCollisionFunc
// Purpose:	set the collision function
// Output:	collision function set
// Return:	none
/////////////////////////////////////
DLL_API void F_API ParSysSetCollisionFunc(PARCOLLFUNC func);

/////////////////////////////////////
// Name:	ParSysExist
// Purpose:	check to see if given
//			particle system exists
// Output:	none
// Return:	TRUE if so
/////////////////////////////////////
DLL_API u8 F_API ParSysExist(const tCHAR *name);

/////////////////////////////////////
// Name:	ParSysLoad
// Purpose:	load particle systems
//			the file may contain several
//			particle system definitions
// Output:	system loaded
// Return:	TRUE if success
/////////////////////////////////////
DLL_API u32 F_API ParSysLoad(const tCHAR *filename);

/////////////////////////////////////
// Name:	ParSysShutdown
// Purpose:	clear out all active
//			particles and remove all
//			particle systems
// Output:	everything cleared out
// Return:	none
/////////////////////////////////////
DLL_API void F_API ParSysShutdown();

/////////////////////////////////////
// Name:	ParSysRender
// Purpose:	add given particle to
//			the scene for rendering
// Output:	scene stuff
// Return:	none
/////////////////////////////////////
DLL_API void F_API ParSysRender(s32 ID);

/////////////////////////////////////
// Name:	ParSysClear
// Purpose:	
/*
Examines the list of active particle systems. Any particle systems
that have not been updated since the last call to r_ClearParticleSystems()
will be destroyed. This means that as soon as the client code forgets
about a particle system (either on purpose or though a logic error) we
are able to clean it up. By refusing to mark particle systems with no
more active particles as updated, we are able to clean up all the expired
systems as well, even if the caller ignores the return values and continues
to request updates on dead systems.
This function should be called once a frame by the client code to ensure
this behaviour.
*/
// Output:	clear off certain particles
// Return:	none
/////////////////////////////////////
DLL_API void F_API ParSysClear();

/////////////////////////////////////
// Name:	ParSysUpdate
// Purpose:	
/*
exported function that allows you to update an active
particle system. Returns rtrue is the particle system is
still alive and wants further processing, or rfalse if the
particle system has been killed off. It is safe to forget
all your local state about a particle system when this function
returns false.
*/
// Output:	update given particle
// Return:	FALSE if particle is dead
/////////////////////////////////////
DLL_API u8 F_API ParSysUpdate(s32 id, gfxParInfo* info);

/////////////////////////////////////
// Name:	ParSysStart
// Purpose:	
/*
Creates a new instance of a particle system of the type specified.
The type is the name of the particle system.
You should call ParSysUpdate before the next call to
ParSysClear or the particle system will be deleted.
*/
// Output:	new active particle
// Return:	the particle ID (0 for failure)
/////////////////////////////////////
DLL_API s32 F_API ParSysStart(const tCHAR *type);

/////////////////////////////////////
// Name:	ParSysGetBound
// Purpose:	get the current particle
//			bound box (objectspace)
// Output:	pMin/Max set
// Return:	none
/////////////////////////////////////
DLL_API void F_API ParSysGetBound(s32 id, Vec3D *pMin, Vec3D *pMax);

/////////////////////////////////////
// Name:	ParSysSetDir
// Purpose:	set the direction of given
//			active particle
// Output:	direction set
// Return:	none
/////////////////////////////////////
DLL_API void F_API ParSysSetDir(s32 id, const Vec3D & dir);

//
// Shadow stuff
//

/////////////////////////////////////
// Name:	GFXShadowSetShade
// Purpose:	set the shadow shading
// Output:	shading color set
// Return:	TRUE if success
/////////////////////////////////////
DLL_API u32 F_API GFXShadowSetShade(u32 color);

//
// Video Player
//

/////////////////////////////////////
// Name:	GFXVideoPlay
// Purpose:	play a movie from given
//			file.
// Output:	movie played
// Return:	something
/////////////////////////////////////
DLL_API s32 F_API GFXVideoPlay(HMAIN hwnd, const tCHAR *filename);

//
// Line stuff
//

/////////////////////////////////////
// Name:	LineDraw
// Purpose:	draws a line strip in
//			screen-space.
// Output:	line strip drawn in 2d
// Return:	TRUE if success
/////////////////////////////////////
DLL_API u32 F_API LineDraw(const Vec2D *pVtxList, u32 vtxNum, u32 clr);

/////////////////////////////////////
// Name:	LineDraw3D
// Purpose:	draws a line strip in
//			screen-space (transformed)
// Output:	line strip drawn
// Return:	TRUE if success
/////////////////////////////////////
DLL_API u32 F_API LineDraw3D(const Vec3D *pVtxList, u32 vtxNum, const Matrix *pMtx, u32 clr);

/////////////////////////////////////
// Name:	LineGetAntiAlias
// Purpose:	Gets the line antialiasing 
//			state. 
// Output:	none
// Return:	the anti-alias state
/////////////////////////////////////
DLL_API u8 F_API LineGetAntiAlias();

/////////////////////////////////////
// Name:	LineSetAntiAlias
// Purpose:	Toggles line antialiasing.
// Output:	antialias set on/off
// Return:	none
/////////////////////////////////////
DLL_API void F_API LineSetAntiAlias(u8 bState);

/////////////////////////////////////
// Name:	LineGetPattern
// Purpose:	Gets the line stipple pattern. 
// Output:	none
// Return:	the line stipple pattern: 
//			1 is opaque, 0 is transparent.
/////////////////////////////////////
DLL_API u32 F_API LineGetPattern();

/////////////////////////////////////
// Name:	LineSetPattern
// Purpose:	Applies a stipple pattern 
//			to the line. 
// Output:	pattern set
// Return:	none
/////////////////////////////////////
DLL_API void F_API LineSetPattern(u32 val);

/////////////////////////////////////
// Name:	LineGetPatternScale
// Purpose:	Gets the stipple-pattern 
//			scale value.
// Output:	none
// Return:	1.0f is the default value 
//			and represents no scaling. 
//			A value less than 1.0f shrinks 
//			the pattern, and a value greater 
//			than 1.0 stretches the pattern.
/////////////////////////////////////
DLL_API f32 F_API LineGetPatternScale();

/////////////////////////////////////
// Name:	LineSetPatternScale
// Purpose:	Stretches the stipple 
//			pattern along the line 
//			direction. 
// Output:	1.0f is the default value 
//			and represents no scaling. 
//			A value less than 1.0f shrinks 
//			the pattern, and a value greater 
//			than 1.0 stretches the pattern.
// Return:	none
/////////////////////////////////////
DLL_API void F_API LineSetPatternScale(f32 val);

/////////////////////////////////////
// Name:	LineGetWidth
// Purpose:	Gets the thickness of 
//			the line. 
// Output:	none
// Return:	the thickness
/////////////////////////////////////
DLL_API f32 F_API LineGetWidth();

/////////////////////////////////////
// Name:	LineSetWidth
// Purpose:	Specifies the thickness 
//			of the line.
// Output:	thickness set
// Return:	none
/////////////////////////////////////
DLL_API void F_API LineSetWidth(f32 val);

#endif