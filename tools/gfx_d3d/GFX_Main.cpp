#include "gfx_i.h"

#include "GFX\gfx_math.h"

#define kDEPTHNEAR		 0.05f
#define kDEPTHFAR		 100000000.0f

gfxMode			  g_mode;

PGFXMAIN		  g_p3D = 0;
PGFXDEVICE		  g_p3DDevice = 0;

ID3DXMatrixStack *g_pWrldStack = 0;//the world matrix stack

//projection stuff
f32				  g_fov;			//the current fov

Matrix			  g_mtxProj[FXMTX_MAX];		//projection matrix
Matrix			  g_mtxView[FXMTX_MAX];		//view matrix (camera)

Matrix			  g_mtxViewProj[FXMTX_MAX];	//concatinated matrix of view and projection (view*proj)
											//this is set whenever we change mtxView or mtxProj

iRect			  g_CLIENTRECT = {0,0,0,0};//window rectangle (only useful in window mode)
FLAGS			  g_FLAGS = 0;

GFXCAPS				  g_d3dcaps;
D3DPRESENT_PARAMETERS g_d3dParam={0};

//time elapsed since last update (milliseconds)
f32					  g_time;

//time elapsed since we initialized GFX
f32					  g_timeElapse;

//this is set everytime we call CameraSetScene
Vec3D				  g_eyePt;
Vec3D				  g_eyeVec;

u32 g_numTri = 0;

iPoint g_txtLim={-1,-1}; //(x = min, y = max)

u8 GFXIsDepthFormatExisting(D3DDEVTYPE device, D3DFORMAT DepthFormat, D3DFORMAT AdapterFormat)
{
    HRESULT hr = g_p3D->CheckDeviceFormat( D3DADAPTER_DEFAULT,
                                          device,
                                          AdapterFormat,
                                          D3DUSAGE_DEPTHSTENCIL,
                                          D3DRTYPE_SURFACE,
                                          DepthFormat);

    return SUCCEEDED( hr );
}

static inline void _GFXSetDefaultStates()
{
#if 1
	//set up depth buffering
	//g_p3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_USEW);
	//g_p3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);

	//set up alpha blending for renderer
	g_p3DDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA); 
    g_p3DDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	g_p3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);

	/////////////////////////////////////////////////////////////////////////////////
	//set up base texture rendering/blending
	//this tells us to set texture color operation as modulate: COLORARG1 * COLORARG2
	g_p3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	g_p3DDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE); //use texture
    g_p3DDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_CURRENT); //use vtx color

	g_p3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	g_p3DDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE); //use texture
    g_p3DDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_CURRENT); //use vtx color

	//set filtering, TODO: check for support...
	g_p3DDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
	g_p3DDevice->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	g_p3DDevice->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);
	/////////////////////////////////////////////////////////////////////////////////

	//set material mode
	g_p3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
    g_p3DDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);
    g_p3DDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
    g_p3DDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);

	//g_p3DDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

	//g_p3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	g_p3DDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
#endif

	//g_p3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
}

//set the other matrix transformation for view*projection 
//(assuming view and projection matrix has been set already)
//this will set the normal,inverse,transpose of viewproj mtx
void GFXSetViewProjMtxFX()
{
	//set viewproj matrix (AB)^1 = B^1 A^1
	g_mtxViewProj[FXMTX_NORMAL] = g_mtxView[FXMTX_NORMAL]*g_mtxProj[FXMTX_NORMAL];

	g_mtxViewProj[FXMTX_INVERSE] = g_mtxProj[FXMTX_INVERSE]*g_mtxView[FXMTX_INVERSE];
	MtxTranspose(&g_mtxViewProj[FXMTX_NORMAL], &g_mtxViewProj[FXMTX_TRANSPOSE]);
	MtxTranspose(&g_mtxViewProj[FXMTX_INVERSE], &g_mtxViewProj[FXMTX_INVTRANSPOSE]);
}

//set the other matrix transformation for projection 
//(assuming FXMTX_NORMAL has been set already)
static inline void _GFXSetProjMtxFX()
{
	//set up the projection matrix, we won't be doing anything else...
    g_p3DDevice->SetTransform(D3DTS_PROJECTION, (const D3DMATRIX*)&g_mtxProj[FXMTX_NORMAL]);

	MtxInverse(&g_mtxProj[FXMTX_NORMAL], &g_mtxProj[FXMTX_INVERSE]);
	MtxTranspose(&g_mtxProj[FXMTX_NORMAL], &g_mtxProj[FXMTX_TRANSPOSE]);
	MtxTranspose(&g_mtxProj[FXMTX_INVERSE], &g_mtxProj[FXMTX_INVTRANSPOSE]);

	GFXSetViewProjMtxFX();
}

/////////////////////////////////////
// Name:	GFXSetProj
// Purpose:	set scene projection.
//			if width/height == 0,
//			then we use screen size
// Output:	projection set
// Return:	none
/////////////////////////////////////
void F_API GFXSetProj(u32 width, u32 height, f32 fov)
{
	g_fov = fov;

	f32 w = width == 0 ? g_mode.width : (f32)width,
		h = height == 0 ? g_mode.height : (f32)height;

	//set up the projection matrix
	MtxPerspectiveFOV(fov, w/h, g_mode.depthNear, g_mode.depthFar, &g_mtxProj[FXMTX_NORMAL]);

	_GFXSetProjMtxFX();
}

/////////////////////////////////////
// Name:	GFXSetProjMtx
// Purpose:	set the projection matrix
// Output:	projection set
// Return:	none
/////////////////////////////////////
void F_API GFXSetProjMtx(const Matrix * pMtx)
{
	g_mtxProj[FXMTX_NORMAL] = *pMtx;

	_GFXSetProjMtxFX();
}

/////////////////////////////////////
// Name:	GFXGetProjMtx
// Purpose:	get the projection 
//			matrix
// Output:	mOut set
// Return:	none
/////////////////////////////////////
void F_API GFXGetProjMtx(Matrix *mOut)
{
	(*mOut) = g_mtxProj[FXMTX_NORMAL];
}

/////////////////////////////////////
// Name:	GFXGetViewMtx
// Purpose:	get the view 
//			matrix
// Output:	mOut set
// Return:	none
/////////////////////////////////////
void F_API GFXGetViewMtx(Matrix *mOut)
{
	(*mOut) = g_mtxView[FXMTX_NORMAL];
}

/////////////////////////////////////
// Name:	GFXGetViewProjMtx
// Purpose:	get the view*projection
//			matrix
// Output:	mOut set
// Return:	none
/////////////////////////////////////
void F_API GFXGetViewProjMtx(Matrix *mOut)
{
	(*mOut) = g_mtxViewProj[FXMTX_NORMAL];
}

/////////////////////////////////////
// Name:	GFXSetFS
// Purpose:	set the GFX's File System
// Output:	added paths for loading
// Return:	none
/////////////////////////////////////
void F_API GFXSetFS(hFMAIN fs)
{
	FSMainSet(fs);
}

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
u32 F_API GFXGetModeCount(GFXFORMAT fmt)
{
	if(!g_p3D) return 0;

	return g_p3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, (D3DFORMAT)fmt);
}

/////////////////////////////////////
// Name:	GFXEnumMode
// Purpose:	enumerate given format.
//			ind = [0, GFXGetModeCount(fmt)-1]
// Output:	pModeOut filled
// Return:	TRUE if mode is available
/////////////////////////////////////
u8 F_API GFXEnumMode(GFXFORMAT fmt, u32 ind, gfxMode *pModeOut)
{
	D3DDISPLAYMODE d3dMode={0};
	if(pModeOut && SUCCEEDED(g_p3D->EnumAdapterModes(D3DADAPTER_DEFAULT, (D3DFORMAT)fmt, ind, &d3dMode)))
	{
		pModeOut->width = d3dMode.Width;
		pModeOut->height = d3dMode.Height;
		pModeOut->fmt = (GFXFORMAT)d3dMode.Format;
		pModeOut->refresh = d3dMode.RefreshRate;

		pModeOut->depthNear = g_mode.depthNear;
		pModeOut->depthFar = g_mode.depthFar;

		return TRUE;
	}

	return FALSE;
}

/////////////////////////////////////
// Name:	GFXInit
// Purpose:	initialize the graphics
//			if pMode is NULL, uses
//			window mode
// Output:	graphics initialized
// Return:	TRUE if success 
/////////////////////////////////////
u32 F_API GFXInit(void *handle, gfxMode *pMode)
{
	//copy mode
	if(pMode)
		memcpy(&g_mode, pMode, sizeof(g_mode));
	else
	{
		g_mode.bVsync = FALSE;
		g_mode.bFullscrn = FALSE;
		g_mode.depthNear = kDEPTHNEAR;
		g_mode.depthFar  = kDEPTHFAR;
	}

	HWND	hwnd = (HWND)handle;

	//the very first thing to do
	//initialize the mac daddy
	g_p3D = Direct3DCreate9(D3D_SDK_VERSION);

	if(!g_p3D) //make sure it's successfully created
	{ LogMsg(LOG_FILE, L"GFXInit", L"Unable to create 3D Device"); return FALSE; }

	//default driver for now...

	//get capabilities
	memset(&g_d3dcaps, 0, sizeof(g_d3dcaps));
	if(FAILED(g_p3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &g_d3dcaps)))
		g_p3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, &g_d3dcaps);

	D3DPRESENT_PARAMETERS d3dParam={0}; 

	d3dParam.SwapEffect     = D3DSWAPEFFECT_COPY;//D3DSWAPEFFECT_FLIP;//D3DSWAPEFFECT_DISCARD;//
    d3dParam.hDeviceWindow  = hwnd;					 //our HWND
    d3dParam.BackBufferCount= 1;					 //use only 1
	d3dParam.PresentationInterval = g_mode.bVsync ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dParam.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;

	if(g_mode.bFullscrn)
	{
		d3dParam.Windowed          = FALSE;
		d3dParam.BackBufferWidth   = g_mode.width;
		d3dParam.BackBufferHeight  = g_mode.height;
		d3dParam.BackBufferFormat  = (D3DFORMAT)g_mode.fmt;
		d3dParam.FullScreen_RefreshRateInHz = g_mode.refresh;
	}
	else
	{
		D3DDISPLAYMODE		  d3dDisplayMode={0};
		//Get desktop display mode.
	    if(GFXCheckError(g_p3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3dDisplayMode), L"GFXInit"))
			return FALSE;

		d3dParam.Windowed          = TRUE;
		d3dParam.BackBufferFormat  = d3dDisplayMode.Format;

		//get client area
		RECT winRect; GetClientRect(hwnd, &winRect);
		g_mode.width = winRect.right-winRect.left;
		g_mode.height = winRect.bottom-winRect.top;
		g_mode.fmt = (GFXFORMAT)d3dDisplayMode.Format;
	}

	d3dParam.EnableAutoDepthStencil = TRUE;			 //depth/stencil buffering

	//////////////////////////////////////////////////////////////////////
	//get the best format there is for depth/stencil
	if(GFXIsDepthFormatExisting(g_d3dcaps.DeviceType, D3DFMT_D24S8, d3dParam.BackBufferFormat))
		d3dParam.AutoDepthStencilFormat = D3DFMT_D24S8;
	else if(GFXIsDepthFormatExisting(g_d3dcaps.DeviceType, D3DFMT_D24X4S4, d3dParam.BackBufferFormat))
		d3dParam.AutoDepthStencilFormat = D3DFMT_D24X4S4;
	else if(GFXIsDepthFormatExisting(g_d3dcaps.DeviceType, D3DFMT_D15S1, d3dParam.BackBufferFormat))
		d3dParam.AutoDepthStencilFormat = D3DFMT_D15S1;
	//no stencil??? try just having depth
	else if(GFXIsDepthFormatExisting(g_d3dcaps.DeviceType, D3DFMT_D32, d3dParam.BackBufferFormat))
	{
		d3dParam.AutoDepthStencilFormat = D3DFMT_D32;
		SETFLAG(g_FLAGS, GFX_NOSTENCIL);
	}
	else if(GFXIsDepthFormatExisting(g_d3dcaps.DeviceType, D3DFMT_D16, d3dParam.BackBufferFormat))
	{
		d3dParam.AutoDepthStencilFormat = D3DFMT_D16;
		SETFLAG(g_FLAGS, GFX_NOSTENCIL);
	}
	//what the hell?
	else
	{ LogMsg(LOG_FILE, L"GFXInit", L"Unable to find depth/stencil format"); return FALSE; }

	//////////////////////////////////////////////////////////////////////
	//determine vertex processing...
	u32 vertexFlag=D3DCREATE_HARDWARE_VERTEXPROCESSING;//D3DCREATE_MIXED_VERTEXPROCESSING;//

	if( g_d3dcaps.VertexShaderVersion < D3DVS_VERSION(1,1) )
		vertexFlag=D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	memcpy(&g_d3dParam, &d3dParam, sizeof(g_d3dParam));

	UINT adpToUse = D3DADAPTER_DEFAULT;
	D3DDEVTYPE deviceType = g_d3dcaps.DeviceType;

#ifdef SHIPPING_VERSION
#else
	//This is for NVPerfHUD
	for(UINT adp = 0; adp < g_p3D->GetAdapterCount(); adp++)
	{
		D3DADAPTER_IDENTIFIER9 Identifier;
		HRESULT res = g_p3D->GetAdapterIdentifier(adp, 0, &Identifier);
		if(strcmp(Identifier.Description, "NVIDIA NVPerfHUD") == 0)
		{
			adpToUse = adp;
			deviceType = D3DDEVTYPE_REF;
			break;
		}
	}
#endif

	//create the device interface
	if(GFXCheckError(
		g_p3D->CreateDevice(adpToUse,			   //primary for now...
                          deviceType,				   //depends
                          hwnd,								   //the window
						  vertexFlag,
                          //D3DCREATE_SOFTWARE_VERTEXPROCESSING, //software for now...
						  //D3DCREATE_HARDWARE_VERTEXPROCESSING,
						  //D3DCREATE_MIXED_VERTEXPROCESSING,
                          &g_d3dParam,
                          &g_p3DDevice), L"GFXInit"))
						  return FALSE;

	memset(&g_d3dcaps, 0, sizeof(g_d3dcaps));
	g_p3DDevice->GetDeviceCaps(&g_d3dcaps);

	if(g_d3dcaps.MaxActiveLights > 8)
		g_d3dcaps.MaxActiveLights = 8;

	////////////////////////////
	// Initialize Matrix system

	//create the world stack
	D3DXCreateMatrixStack(0, &g_pWrldStack);
	g_pWrldStack->LoadIdentity();

	MtxIdentity(&g_mtxProj[FXMTX_NORMAL]);

	//set the view matrix to identity
	MtxIdentity(&g_mtxView[FXMTX_NORMAL]);
	MtxIdentity(&g_mtxView[FXMTX_INVERSE]);
	MtxIdentity(&g_mtxView[FXMTX_TRANSPOSE]);
	MtxIdentity(&g_mtxView[FXMTX_INVTRANSPOSE]);
	
	_GFXSetProjMtxFX();

	//initialize texture system
	IgfxTexture::sInitSys();

	//default camera
	g_p3DDevice->SetTransform(D3DTS_VIEW, (const D3DMATRIX*)&g_mtxView);

	//set default projection
	GFXSetProj(0,0, FOV);

	//set default states
	_GFXSetDefaultStates();
	LightSetAmbient(0xffffffff);

	//create screenshot thing
	GFXScreenCreate(g_mode.width, g_mode.height);

	//initialize particle system buffer
	ParSysInit();

	//initialize scene buffer
	GFXSceneSetBufferSize(256);

	//initialize shadow system
	GFXShadowInit(0, 0, 256);
	GFXShadowInitShade();

	//initialize line interface
	GFXLineInit();

	g_FLAGS = 0;

	g_timeElapse = 0;

	//initialize some flags
	SETFLAG(g_FLAGS, GFX_UPDATESTENCIL);

	return TRUE;
}

/////////////////////////////////////
// Name:	GFXGetMode
// Purpose:	get the current mode
// Output:	pModeOut filled
// Return:	none
/////////////////////////////////////
void F_API GFXGetMode(gfxMode *pModeOut)
{
	memcpy(pModeOut, &g_mode, sizeof(g_mode));
}

/////////////////////////////////////
// Name:	GFXSetMode
// Purpose:	set the graphics mode
// Output:	graphics mode set,
//			resets all shaders,
//			textures and models
// Return:	TRUE if success
/////////////////////////////////////
u32 F_API GFXSetMode(gfxMode *pMode)
{
	g_mode = *pMode;

	if(g_mode.bFullscrn)
	{
		g_d3dParam.Windowed          = FALSE;
		
		if(g_mode.refresh > 0)
			g_d3dParam.FullScreen_RefreshRateInHz = g_mode.refresh;
	}

	g_d3dParam.BackBufferWidth   = g_mode.width;
	g_d3dParam.BackBufferHeight  = g_mode.height;
	g_d3dParam.BackBufferFormat  = (D3DFORMAT)g_mode.fmt;

	/*else
	{
		D3DDISPLAYMODE		  d3dDisplayMode={0};
		//Get desktop display mode.
	    if(GFXCheckError(g_p3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3dDisplayMode), L"GFXSetMode"))
			return FALSE;

		g_d3dParam.Windowed          = TRUE;
		g_d3dParam.BackBufferFormat  = d3dDisplayMode.Format;

		//get client area
		if(hwnd)
		{
			RECT winRect; GetClientRect(hwnd, &winRect);
			g_mode.width = winRect.right-winRect.left;
			g_mode.height = winRect.bottom-winRect.top;
		}

		g_mode.fmt = (GFXFORMAT)d3dDisplayMode.Format;
	}*/

	g_d3dParam.PresentationInterval = g_mode.bVsync ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE;

	return GFXReset();
}

/////////////////////////////////////
// Name:	GFXReset
// Purpose:	reset the device
// Output:	resets all shaders,
//			textures and models
// Return:	TRUE if success
/////////////////////////////////////
u32 F_API GFXReset()
{
	if(g_d3dParam.Windowed == TRUE)
	{
		D3DDISPLAYMODE		  d3dDisplayMode={0};
		//Get desktop display mode.
	    if(GFXCheckError(g_p3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3dDisplayMode), L"GFXInit"))
			return RETCODE_FAILURE;

		g_d3dParam.BackBufferFormat  = d3dDisplayMode.Format;
	}

	//destroy the particle system buffer
	ParSysDestroy();

	IgfxTexture::sDestroySys();

	//invalidate stuff
	IgfxFont::sInvalidateAll();
	IgfxTexture::sInvalidateAll();
	IgfxEffect::sInvalidateAll();
	IgfxModel::sInvalidateAll();
	GFXShadowInvalidate();
	GFXLineInvalidate();

	//reset
	HRESULT hr = g_p3DDevice->Reset(&g_d3dParam);

	GFXCheckError(hr, L"GFXReset");

	//initialize texture system
	IgfxTexture::sInitSys();

	//set default projection
	GFXSetProj(0,0, g_fov);

	//set default states
	_GFXSetDefaultStates();

	//reset fog stuff
	FogOnReset();

	//reset light stuff
	LightOnReset();

	//restore stuff
	IgfxFont::sRestoreAll();
	IgfxTexture::sRestoreAll();
	IgfxEffect::sRestoreAll();
	IgfxModel::sRestoreAll();
	GFXShadowRestore();
	GFXLineRestore();

	//create screenshot thing
	GFXScreenCreate(g_mode.width, g_mode.height);

	//recreate the particle system buffer
	ParSysInit();

	//in case we changed screen resolution
	GFXShadowInitShade();

	return TRUE;
}

/////////////////////////////////////
// Name:	GFXDestroy
// Purpose:	destroy the graphics
// Output:	graphics destroyed
// Return:	none
/////////////////////////////////////
void F_API GFXDestroy()
{
	GFXShadowDestroy();
	GFXShadowDestroyShade();

	ParSysShutdown(); //destroy all stray particles
	ParSysDestroy();

	GFXScreenDestroy();

	IgfxModel::sDestroyAll();

	//destroy texture system
	IgfxTexture::sDestroySys();

	IgfxEffect::sDestroyAll();

	IgfxTexture::sDestroyAll();

	GFXSceneDestroyBuffer();

	GFXLineDestroy();

	SAFE_RELEASE(g_pWrldStack);
	SAFE_RELEASE(g_p3DDevice);
	SAFE_RELEASE(g_p3D);
}

/////////////////////////////////////
// Name:	GFXClear
// Purpose:	clears the screen depending
//			on given flags: GFXCLEAR_*
// Output:	screen cleared
// Return:	TRUE if success
/////////////////////////////////////
u32 F_API GFXClear(u32 flags, s32 numRect, const iRect* pRects, 
			  u32 color, f32 Z, u32 stencil)
{
	if(!TESTFLAGS(g_FLAGS, GFX_UPDATESTENCIL))
		CLEARFLAG(flags, GFXCLEAR_STENCIL);

	HRESULT hr =
	g_p3DDevice->Clear(numRect,
                       (D3DRECT*)pRects,
                       flags, //what to clear
                       color, //the color
                       Z,  //depth buffer value to clear
                       stencil);   //stencil buffer

	return SUCCEEDED(hr);
}

/////////////////////////////////////
// Name:	GFXBegin
// Purpose:	readies device to render
//			stuff.
// Output:	shit happens
// Return:	none
/////////////////////////////////////
void F_API GFXBegin()
{
	if(!TESTFLAGS(g_FLAGS, GFX_BEGINSCENESET))
	{
		g_p3DDevice->BeginScene();
		SETFLAG(g_FLAGS, GFX_BEGINSCENESET);
	}
}

/////////////////////////////////////
// Name:	GFXEnd
// Purpose:	call this after finish
//			with rendering stuff
// Output:	more shit happens
// Return:	none
/////////////////////////////////////
void F_API GFXEnd()
{
	if(TESTFLAGS(g_FLAGS, GFX_BEGINSCENESET))
	{
		g_p3DDevice->EndScene();
		CLEARFLAG(g_FLAGS, GFX_BEGINSCENESET);
	}
}

static inline u8 _GFXCheckDeviceLost()
{
	HRESULT hr;

	if(TESTFLAGS(g_FLAGS, GFX_DEVICELOST))
	{
		hr = g_p3DDevice->TestCooperativeLevel();

		if(hr == D3DERR_DEVICELOST) //let's try again later
			return TRUE;

		//do we need to reset?
		if(hr == D3DERR_DEVICENOTRESET)
		{
			//reset
			GFXReset();
		}

		CLEARFLAG(g_FLAGS, GFX_DEVICELOST);

		return FALSE;
	}

	return FALSE;
}

/////////////////////////////////////
// Name:	GFXPresent
// Purpose:	render the scene to screen
//			if handle is not null, then
//			scene is rendered to that
//			handle
// Output:	scene rendered
// Return:	TRUE if success
/////////////////////////////////////
u32 F_API GFXPresent(void *handle)
{
	HRESULT hr = g_p3DDevice->Present(0, 0, (HWND)handle, 0);

	if(hr == D3DERR_DEVICELOST) //uh oh...
		SETFLAG(g_FLAGS, GFX_DEVICELOST);
	else if(FAILED(hr))
	{
		GFXCheckError(hr, L"GFXRender");
		return FALSE;
	}

	return TRUE;
}

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
void F_API GFXUpdate(f32 time)
{
	_GFXCheckDeviceLost();

	g_time = time;
	g_timeElapse += time;

	//alternate shadow update
	/*if(TESTFLAGS(g_FLAGS, GFX_UPDATESTENCIL))
		CLEARFLAG(g_FLAGS,GFX_UPDATESTENCIL);
	else
		SETFLAG(g_FLAGS, GFX_UPDATESTENCIL);*/

	IgfxTexture::sAnimUpdateAll();
}

/////////////////////////////////////
// Name:	GFXPickRay
// Purpose:	create a ray from given 
//			screen loc
// Output:	ray pos&dir is filled
// Return:	none
/////////////////////////////////////
void F_API GFXPickRay(s32 x, s32 y, Vec3D *pRayPos, Vec3D *pRayDir)
{
    //Compute the vector of the pick ray in screen space
	Vec3D v((((2.0f*x)/g_mode.width)-1)/g_mtxProj[FXMTX_NORMAL]._11,
		   -(((2.0f*y)/g_mode.height)-1)/g_mtxProj[FXMTX_NORMAL]._22,
			1.0f);

    // Transform the screen space pick ray into 3D space
    pRayDir->x = v.x*g_mtxView[FXMTX_INVERSE]._11 + v.y*g_mtxView[FXMTX_INVERSE]._21 + v.z*g_mtxView[FXMTX_INVERSE]._31;
    pRayDir->y = v.x*g_mtxView[FXMTX_INVERSE]._12 + v.y*g_mtxView[FXMTX_INVERSE]._22 + v.z*g_mtxView[FXMTX_INVERSE]._32;
    pRayDir->z = v.x*g_mtxView[FXMTX_INVERSE]._13 + v.y*g_mtxView[FXMTX_INVERSE]._23 + v.z*g_mtxView[FXMTX_INVERSE]._33;
    pRayPos->x = g_mtxView[FXMTX_INVERSE]._41;
    pRayPos->y = g_mtxView[FXMTX_INVERSE]._42;
    pRayPos->z = g_mtxView[FXMTX_INVERSE]._43;
}

/////////////////////////////////////
// Name:	GFXProjectPt
// Purpose:	project the given 3d loc.
//			to 2d screen space
// Output:	pOut set
// Return:	none
/////////////////////////////////////
void F_API GFXProjectPt(const Vec3D *pLoc, Vec3D *pOut)
{
	//D3DVIEWPORT9 vp; g_p3DDevice->GetViewport(&vp);

	Vec4D cP; Vec3DTransform(pLoc, &g_mtxViewProj[FXMTX_NORMAL], &cP);

	pOut->x = (1 + (cP.x/cP.w))*g_mode.width/2;
	pOut->y = (1 - (cP.y/cP.w))*g_mode.height/2;
	pOut->z = cP.z/cP.w;

	
	/*D3DXVec3Project((D3DXVECTOR3 *)pOut,
    (CONST D3DXVECTOR3 *)pLoc,
    &vp,
    (CONST D3DXMATRIX *)&g_mtxProj[FXMTX_NORMAL],
    (CONST D3DXMATRIX *)&g_mtxView[FXMTX_NORMAL],
    &D3DXMATRIX(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1));*/
}

/////////////////////////////////////
// Name:	GFXUpdateClientRect
// Purpose:	update the client rect
//			(use this on WM_MOVE or
//			whenever the window is
//			changed
// Output:	client rect set
// Return:	none
/////////////////////////////////////
void F_API GFXUpdateClientRect(const iRect *pRect)
{
	if(pRect)
		memcpy(&g_CLIENTRECT, pRect, sizeof(g_CLIENTRECT));
}

/////////////////////////////////////
// Name:	GFXRenderBoundBox
// Purpose:	display AABB
// Output:	lines representing the
//			AABB displayed
// Return:	none
/////////////////////////////////////
void F_API GFXRenderBoundBox(const Vec3D *min, const Vec3D *max, u32 clr)
{
	struct bbVtx {
		f32 pt[3];
		f32 norm[3];
		u32 clr;
	};

	bbVtx pts[24]= {{min->x,min->y,min->z, 0,1,0, clr}, {min->x,min->y,max->z, 0,1,0, clr}, 
	                {min->x,min->y,max->z, 0,1,0, clr}, {max->x,min->y,max->z, 0,1,0, clr},
					{max->x,min->y,max->z, 0,1,0, clr}, {max->x,min->y,min->z, 0,1,0, clr},
					{max->x,min->y,min->z, 0,1,0, clr}, {min->x,min->y,min->z, 0,1,0, clr},

					{min->x,max->y,min->z, 0,1,0, clr}, {min->x,max->y,max->z, 0,1,0, clr}, 
	                {min->x,max->y,max->z, 0,1,0, clr}, {max->x,max->y,max->z, 0,1,0, clr},
					{max->x,max->y,max->z, 0,1,0, clr}, {max->x,max->y,min->z, 0,1,0, clr},
					{max->x,max->y,min->z, 0,1,0, clr}, {min->x,max->y,min->z, 0,1,0, clr},
					
					{min->x,min->y,min->z, 0,1,0, clr}, {min->x,max->y,min->z, 0,1,0, clr}, 
	                {min->x,min->y,max->z, 0,1,0, clr}, {min->x,max->y,max->z, 0,1,0, clr},
					{max->x,min->y,max->z, 0,1,0, clr}, {max->x,max->y,max->z, 0,1,0, clr},
					{max->x,min->y,min->z, 0,1,0, clr}, {max->x,max->y,min->z, 0,1,0, clr}};

	u32 lightEnable;
	g_p3DDevice->GetRenderState(D3DRS_LIGHTING,(DWORD*)&lightEnable);

	g_p3DDevice->SetRenderState(D3DRS_LIGHTING,FALSE);

	Matrix wMtx; MtxIdentity(&wMtx);
	g_p3DDevice->SetTransform(D3DTS_WORLD, (const D3DMATRIX*)&wMtx);

	g_p3DDevice->SetVertexShader( NULL );
	g_p3DDevice->SetFVF(GFXFVF_XYZ | GFXFVF_NORMAL | GFXFVF_DIFFUSE);
	g_p3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, 12, pts, sizeof(bbVtx));

	g_p3DDevice->SetRenderState(D3DRS_LIGHTING,lightEnable);
}

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
u32 F_API GFXSetTextureLimit(s32 min, s32 max)
{
	g_txtLim.x = min;
	g_txtLim.y = max;

	return TRUE;
}

static inline void _GetLimitTxtSize(u32 *w, u32 *h, u32 *depth)
{
	if(g_txtLim.y != -1)
	{
		//check first for width
		if(w)
		{
			if(h)
			{
				while(*w > g_txtLim.y)
				{
					if(g_txtLim.x == -1 || ((*h)>>1) > g_txtLim.x)
					{
						*w >>= 1;
						*h >>= 1;
					}
					else
						break;
				}
			}
			else
			{
				while(*w > g_txtLim.y)
					*w >>= 1;
			}
		}

		//now for height
		if(h)
		{
			if(w)
			{
				while(*h > g_txtLim.y)
				{
					if(g_txtLim.x == -1 || ((*w)>>1) > g_txtLim.x)
					{
						*w >>= 1;
						*h >>= 1;
					}
					else
						break;
				}
			}
			else
			{
				while(*h > g_txtLim.y)
					*h >>= 1;
			}
		}

		//for depth
		if(depth)
		{
			while(*depth > g_txtLim.y)
				*depth >>= 1;
		}
	}
}

//check if given texture width and height is valid for loading,
//this will spit out the correct w and h
//(for 2D textures)
void GFXValidateTxtSize(u32 *w, u32 *h)
{
	_GetLimitTxtSize(w, h, 0);

	//get the appropriate size in terms of D3D
	D3DXCheckTextureRequirements(g_p3DDevice, w, h, 0, 0, 0, D3DPOOL_DEFAULT);
}

//for volume
void GFXValidateVolTxtSize(u32 *w, u32 *h, u32 *depth)
{
	_GetLimitTxtSize(w, h, depth);

	//get the appropriate size in terms of D3D
	D3DXCheckVolumeTextureRequirements(g_p3DDevice, w, h, depth, 0, 0, 0, D3DPOOL_DEFAULT);
}

//for cube
void GFXValidateCubeTxtSize(u32 *size)
{
	_GetLimitTxtSize(size, 0, 0);

	//get the appropriate size in terms of D3D
	D3DXCheckCubeTextureRequirements(g_p3DDevice, size, 0, 0, 0, D3DPOOL_DEFAULT);
}