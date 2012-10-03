#ifndef _gfx_i_h
#define _gfx_i_h

#include "common.h"

#include "STD\memTool.h"
#include "STD\pager.h"
#include "STD\log.h"
#include "STD\filesys.h"

#include "STD\parser.h"

#include "d3d9.h"
#include "D3dx9tex.h"
#include "d3dx9mesh.h"
#include "d3dx9core.h"

#include "d3d9types.h"
#include "d3dx9math.h"

#include "d3dstuff\dxutil.h"

//#include "cal3d/cal3d.h"

#include "GFX\gfx.h"

////////////////////////////////
// Defines
//

#define FOV				 (PI/4)

#define MAXLIGHT		 4

#define MAXBASETXT		 2

#define MAX_DECAL_VERTS			384
#define MAX_DECAL_FRAGMENTS		128

#define SHADOW_EXTRUDE			1500.0f

////////////////////////////////
// Flags
//

//flags for GFX
#define GFX_BEGINSCENESET	0x00000001l 	//did we begin scene?
#define GFX_DEVICELOST		0x00000002l		//did we lose device?
#define GFX_NOSTENCIL		0x00000004l		//device has no stencil support
#define GFX_LIGHTENABLE		0x00000008l		//light is enabled?
#define GFX_UPDATESTENCIL	0x00000010l		//do we want to refresh stencil (mostly for shadow)

//flags for scene objects
#define SCENE_SINGLEMTX		0x00000001l		//scene object uses only normal world matrix
#define SCENE_FORCEFVF		0x00000002l		//force the use of FVF for vertex buffer
#define SCENE_NOSHADOW		0x00000004l		//the scene object will not receive shadow
#define SCENE_SKYBOX		0x00000008l		//scene object is a skybox

//flags for texture
#define TXT_FILE			0x00000001l		//texture is loaded from file (name is filepath)
#define TXT_CLRKEY			0x00000002l		//texture has a color key

//flags for sprites
#define SPR_PAUSE			0x00000001l		//object is paused (no frame update)
#define SPR_PAUSE_ON_END	0x00000002l		//object will pause on state end frame

//flags for effects
#define EFFECT_FILE			0x00000001l		//effect is loaded from file
#define EFFECT_RESOURCE		0x00000002l		//effect is loaded from resource
#define EFFECT_TRANSPARENT	0x00000004l		//effect is transparent, set to transparent scene pool
#define EFFECT_TANGENT		0x00000008l		//effect uses tangents for each vertex
#define EFFECT_BINORMAL		0x00000010l		//effect uses binormals for each vertex
#define EFFECT_BASETEX0		0x00000020l		//effect uses the base texture 0 (so load the hTXT)
#define EFFECT_SCENE		0x00000040l		//effect is a scene

//flags for models
#define MODEL_TANGENT		0x00000001l		//model has tangents for each vertex
#define MODEL_BINORMAL		0x00000002l		//model has binormals for each vertex

//flags for objects
#define OBJECT_SHADOW		0x00000001l		//object will cast shadow
#define OBJECT_NORECSHADOW	0x00000002l		//object will not receive any shadow

//flags for shadows
#define SHADOW_OCC_CULLED	0x00000001l		//the occluder is culled but we still want to cast shadow
#define SHADOW_USE_DIR		0x00000002l		//the given lPos is a vector (used for outdoor shadow casting)


////////////////////////////////
// Enums
//

////////////////////////////////
// Types
//

//wrapper typedef so that we can upgrade easily
typedef LPDIRECT3D9						PGFXMAIN;
typedef LPDIRECT3DDEVICE9				PGFXDEVICE;
typedef D3DCAPS9						GFXCAPS;
typedef LPDIRECT3DTEXTURE9				PGFXTEXTURE;
typedef LPDIRECT3DBASETEXTURE9			PGFXBASETEXTURE;
typedef IDirect3DIndexBuffer9			GFXINDBUFF;
//typedef IDirect3DVertexBuffer9			GFXVTXBUFF;
typedef LPDIRECT3DVERTEXSHADER9			PGFXVERTEXSHADER;
typedef LPDIRECT3DVERTEXDECLARATION9	PGFXVERTEXDECLARATION;

typedef IDirect3DSurface9				GFXSURFACE;
typedef D3DVIEWPORT9					GFXVIEWPORT;
typedef D3DLIGHT9						GFXLIGHT;

////////////////////////////////
// Callbacks
//

//effect callback
typedef void (* EFFECTDATCB) (class IgfxEffect *effect, u32 wParam, s32 lParam);

////////////////////////////////
// Structs
//
typedef struct _GFXVTX {
	IDirect3DVertexBuffer9 *vtx;
	u32						stride;	 //size of one vertex element in bytes
	u32						fvf;	 //fvf format
	u32						count;	 //number of vertices
} GFXVTX;

typedef struct _GFXVTXBUFF {
	vector<GFXVTX>			*vtxBuff;

	PGFXVERTEXDECLARATION    decl;		 //vertex declaration for HLSL
} GFXVTXBUFF;

///////////////////////////////////////////////////////////////////////////////////////////////
// Texture Interface
///////////////////////////////////////////////////////////////////////////////////////////////
typedef class IgfxTexture *hITXT;

class IgfxTexture : public gfxTexture {
public:
	IgfxTexture(const tCHAR *name, u32 width, u32 height, u32 mipLevels, u32 usage, 
		GFXFORMAT fmt, TXTCSMCREATE func);
	
	IgfxTexture(const tCHAR *filename, u8 bClrKey, u32 srcClrKey);

	~IgfxTexture();

	void AddRef();
	s32  Release();	

	u32  BeginScene();
	void EndScene();

	u32  GetWidth();
	u32  GetHeight();
	u32  Lock(u32 lvl, const iRect *pRect, u32 flags, s32 *pitchOut, void **bitsOut);
	void Unlock(u32 lvl);
	
	u32  Blt(f32 x, f32 y, const iRect *srcRect, gfxBlt *pBlt);
	u32  StretchBlt(f32 x, f32 y, f32 w, f32 h, const iRect *srcRect, gfxBlt *pBlt);
	u32  Blt3D(const Matrix *wrldMtx, const Vec4D *pClr);
	u32  BltBillboard(const Vec3D & center, f32 size, f32 rot, const Vec4D *pClr);

	///////////////////////////////////////////
	//internal interface used by GFX

	static u32 sInitSys();
	static void sDestroySys();

	//set up the billboard to face the camera
	static void sBillboardPrepare(const Vec3D & camDir);

	//this is to draw the texture right away...
	//usu. used by effect scene
	static GFXVTXBUFF *sGetVtx3D();

	//get the default texture display FX
	static hEFFECT sGetDfltFX();

	static void sDestroyAll();

	/////////////////////////////////////
	// Purpose:	invalidate textures
	// Output:	textures invalidated
	// Return:	none
	/////////////////////////////////////
	static void sInvalidateAll();

	/////////////////////////////////////
	// Purpose:	restore textures
	// Output:	textures restored
	// Return:	none
	/////////////////////////////////////
	static void sRestoreAll();

	/////////////////////////////////////
	// Purpose:	this will update all 
	//			textures with animation
	// Output:	animation updated
	// Return:	none
	/////////////////////////////////////
	static void sAnimUpdateAll();

	static hITXT sFind(const tCHAR *name);

	static LPD3DXSPRITE sGetISpr(); //get the sprite interface

	/////////////////////////////////////
	// Purpose:	get the texture's image
	// Output:	none
	// Return:	the texture image
	/////////////////////////////////////
	PGFXBASETEXTURE GetImg();

	//use Texture Display's FX for blting
	void Blt3DEX(GFXVTXBUFF *vtxBuff, GFXPRIMITIVETYPE type, u32 numVtx, u32 numFaces,
		const Matrix * worldMtx, const Vec4D & clr);

private:
	u8 _AddToList(const tCHAR *name); //add us to our global texture list (returns TRUE if we are added)

	//load up the texture's image
	HRESULT Load(const tCHAR *filename);
	HRESULT _LoadAnim(const tCHAR *filename); //assuming the filename is an animation file

	//initialize the texture image
	u8 InitImage(u32 width, u32 height);

	struct txtVSVertex {
		f32 x, y, z;	 //model space vertex position
		f32 s, t, u;	 //texture coord
	};

	enum {
		VERTEXFLAG = (GFXFVF_XYZ | GFXFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0)), //xyz and texture

		VTXBUFF_USAGE =	GFXUSAGE_WRITEONLY,//D3DUSAGE_DYNAMIC//D3DUSAGE_SOFTWAREPROCESSING//
		VTXBUFF_POOL = GFXPOOL_MANAGED,//D3DPOOL_DEFAULT

		MAXVTX = 4
	};

	static map<wstring, hITXT>	  s_TXTLIST;			//list of textures and textures w/ animations
	static list<hITXT>			  s_TXTANIMLIST;		//list of textures w/ animation

	static LPD3DXSPRITE		      s_pTxtSprite;    //global sprite interface

	static GFXVTXBUFF			 *s_pTxtVtxBuff;   //global sprite 3d interface

	static GFXVTXBUFF			 *s_pTxtBBVtxBuff; //global Billboard 3d interface

	static hEFFECT				  s_txtEffect;	   //the 3d display effect

	const tCHAR		 *m_name;					//this is a reference to our texture table (as long as we are here, this pointer is valid)

	PGFXBASETEXTURE   m_texture;				//It's the d3d texture!

	PGFXBASETEXTURE  *m_txtAnim;			//array of textures for animation (if texture is null)
	u32				  m_curAnim;			//current animation index
	u32				  m_numAnim;			//number of animation
	f32				  m_animDC;				//animation delay counter
	f32				  m_animD;				//animation delay

	GFXSURFACE			 *m_pSurface;	//surface used for rendering onto this texture
	LPD3DXRENDERTOSURFACE m_renderer;	//render to surface interface (if texture is a render target)

	s32 m_ref;							//number of things refering to this

	D3DCOLOR m_srcClrKey;				//if this is used

	GFXRESOURCETYPE m_type;				//texture type: GFXRTYPE_<TEXTURE, VOLUMETEXTURE, CUBETEXTURE>
	u32 m_width;						//width of texture
	u32 m_height;						//height of texture
	u32	m_depth;						//used for volume textures

	//for non-file textures
	u32 m_mipLevels, m_usage;
	GFXFORMAT m_bpp;
	TXTCSMCREATE m_func;				//custom function called during texture create

	FLAGS	 m_flag;
};

//for 2D textures: width/height is used
//for volume textures: width/height/depth is used
//for cube textures: width is used
//width,height,depth,miplvl can be D3DX_DEFAULT
//fmt can be D3DFMT_UNKNOWN
//clrKey = 0 for no color key
PGFXBASETEXTURE _TextureLoad(const tCHAR *filename, D3DXIMAGE_INFO *pImgInfOut, u32 width, u32 height, u32 depth,
							 u32 miplvl, u32 fmt, u32 clrKey);

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
// Font Interface
///////////////////////////////////////////////////////////////////////////////////////////////
typedef class IgfxFont *hIFNT;

class IgfxFont : public gfxFont {
public:
	IgfxFont(const gfxFontDesc *pDesc);

	~IgfxFont();

	void AddRef();
	s32  Release();	

	u32  Resize(s32 size);
	u32  TextBox(const tCHAR *text, s32 count, const iRect *r, u32 flags, u32 color);
	u32  Printf(f32 x, f32 y, u32 color, u32 flags, const tCHAR *text, ...);
	void GetStrSize(const tCHAR *str, iPoint *pSize);

	///////////////////////////////////////////
	//internal interface used by GFX

	static void sInvalidateAll();
	static void sRestoreAll();

private:
	static list<hIFNT>		s_fntList;			//list of all fonts created

	LPD3DXFONT		 m_pD3DFont;				//The D3D Font

	gfxFontDesc		 m_fntDesc;				//the font description upon creation

	s32				 m_ref;
};

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
// Sprite Interface
///////////////////////////////////////////////////////////////////////////////////////////////
class IgfxSprite : public gfxSprite {
public:
	IgfxSprite(hTXT txt, u32 numFrame, const iPoint *frameSize, f32 delay, u32 numImgCol, 
		const iPoint *imgOffset, u32 numState, const gfxSprFrame *states);

	~IgfxSprite();

	void AddRef();
	s32  Release();	

	u8   IsStateEnd();
	void Pause(u8 bPause);
	void PauseOnStateEnd(u8 bPause);
	void UpdateFrame();
	void UpdateFrameAllState();
	u32  GetMaxState();
	u32  GetCurState();
	u32  SetState(u32 state);
	const iPoint & GetSize();
	u32  Blt(f32 x, f32 y, gfxBlt *pBlt);
	u32  StretchBlt(f32 x, f32 y, f32 w, f32 h, gfxBlt *pBlt);
	class gfxSprite * Duplicate();

private:
	hTXT		 m_theTexture;			//handle to texture this sprite use

	f32			 m_time;				//time accum.
	f32			 m_timeDelay;			//delay when to get to next frame

	u32			 m_numFrames;			//Number of state frames
	u32			 m_curState;			//Index of current state
	u32			 m_numStates;			//Number of sprite states
	iPoint		 m_frameSize;			//The size of each frame
	u32			 m_numImgCol;			//Mainly used for duplication
	iPoint		 m_imgOffset;			//Mainly used for duplication
	iRect	    *m_frameRects;			//Array of rects for each frames
	gfxSprFrame *m_states;				//Array of sprite states
	FLAGS		 m_status;				//sprite flags

	s32			 m_ref;
};

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
// FX Interface
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////
// FX Scene stuff
typedef struct _fxDepthSurf {
	wstring					name;

	Vec2D					vpSize;	//vp scale size

	D3DSURFACE_DESC			desc;	//used when reseting device
	GFXSURFACE			   *surf;
} fxDepthSurf;

typedef struct _fxRenderTxt {
	wstring					name;

	Vec2D					vpSize;	//vp scale size
	s32						mipLevel;

	D3DSURFACE_DESC			desc;	//used when reseting device
	PGFXTEXTURE				txt;
	GFXSURFACE			   *surf;
} fxRenderTxt;

typedef struct _gfxEffectScn {
	vector<fxDepthSurf>			depthSurfs;		//depth surfaces
	vector<fxRenderTxt>			renderTxts;		//render target textures

	s32							renderTxtInd;	//index to render texture target (if -1, use main)
												//this is the currently used render target texture

	u32							pass;			//current pass for the technique

	GFXSURFACE				   *pRTSurf, *pDepthSurf; //this is used during scene render

	hTXT						renderer;				//if not NULL, we use this instead of main renderer

	vector<class fxScnCommand *> commands; //scene commands
} gfxEffectScn;
///////////////////////////////////////////

//effect matrix semantic types
typedef enum {
	FXT_WORLD,
	FXT_VIEW,
	FXT_PROJ,
	FXT_WRLDVIEW,
	FXT_VIEWPROJ,
	FXT_WRLDVIEWPROJ,
	FXT_MAX
} FXMTXTypes;

typedef enum {
	FXMTX_NORMAL,
	FXMTX_INVERSE,
	FXMTX_TRANSPOSE,
	FXMTX_INVTRANSPOSE,
	FXMTX_MAX
} FXMTX;

typedef struct _gfxFXLH {
	GFXHANDLE	pos;
	GFXHANDLE	dir;

	GFXHANDLE	Diffuse;
    GFXHANDLE	Specular;
    GFXHANDLE	Ambient;
} gfxFXLH;

typedef struct _gfxFXAnimTxt {
	GFXHANDLE	var;	//the variable to receive the texture
	hTXT		txt;	//the animated texture
} gfxFXAnimTxt;

typedef class IgfxEffect *hIEFFECT;

class IgfxEffect : public gfxEffect {
public:
	IgfxEffect(const tCHAR *name, const s8 *buff);
	IgfxEffect(const tCHAR *filename);
	
	~IgfxEffect();
	
	void AddRef();
	s32  Release();	

	GFXHANDLE GetParam(GFXHANDLE parent, const tCHAR *name);
	u8		  GetValue(GFXHANDLE param, void *dat, u32 numBytes);
	u8		  SetValue(GFXHANDLE param, void *dat, u32 numBytes);
	u8		  SetTexture(GFXHANDLE param, hTXT txt);
	GFXHANDLE GetTechnique(const tCHAR *name);
	u32		  GetNextValidTechnique(GFXHANDLE tech, GFXHANDLE *techOut);
	u8		  ValidateTechnique(GFXHANDLE tech);
	u8		  SetTechnique(GFXHANDLE tech);
	void	  SetCallback(EFFECTFUNC fxCB);

	u32		  SceneRender();

	///////////////////////////////////////////
	//internal interface used by GFX

	/////////////////////////////////////
	// Purpose:	destroy all effect
	// Output:	effects destroyed
	// Return:	none
	/////////////////////////////////////
	static void sDestroyAll();

	/////////////////////////////////////
	// Purpose:	invalidates effects
	// Output:	effects invalidated
	// Return:	none
	/////////////////////////////////////
	static void sInvalidateAll();

	/////////////////////////////////////
	// Purpose:	restore effects
	// Output:	effects restored
	// Return:	none
	/////////////////////////////////////
	static void sRestoreAll();

	static hIEFFECT sFind(const tCHAR *name);

	/////////////////////////////////////
	// Purpose:	set the global params
	//			ie. All common params
	//			to vtx. buffers
	//			(e.g.: projection mtx...etc.)
	// Output:	effect params set
	// Return:	TRUE if success
	/////////////////////////////////////
	u32 SetGlobalParams();

	/////////////////////////////////////
	// Purpose:	set params for effect
	//			we are assuming a matrix array
	//			of size FXMTX_MAX
	//			if bSingleMtx, then there is
	//			only the normal world mtx
	// Output:	effect params set
	// Return:	TRUE if success
	/////////////////////////////////////
	u32 SetParams(const Matrix * wrldMtx, u8 bSingleMtx, const gfxMaterial & mtrl, 
		const hLIGHT *pLights, void *userDat, EFFECTDATCB fxCB, u32 wParam, s32 lParam);

	//get the depth surface index
	s32 GetDepthSurfInd(const tCHAR *name);

	//get the render target texture index
	s32 GetRenderTxtInd(const tCHAR *name);

	//get the LOD tech based on given distance square
	GFXHANDLE GetTechLOD(f32 distSq);

	/////////////////////////////////////
	// Purpose:	set texture from given fx's
	//			base texture.
	//			Maximum is MAXBASETXT
	// Output:	value in fx set
	// Return:	TRUE if variable is found
	//			and is set
	/////////////////////////////////////
	u8 SetBaseTexture(u32 ind, hTXT txt);

	//be careful of using this, just used by scene FX
	inline gfxEffectScn * GetScn() { return m_pScn; }

	//be careful of using this, just used by scene FX
	inline LPD3DXEFFECT   GetFX() { return m_effect; }

	inline void CallUserCB(void *dat) { if(m_fxCB) { m_fxCB((hEFFECT)this, dat); } }

	inline FLAGS GetFlag() { return m_flag; }

	inline GFXHANDLE GetDfltTechnique() { return m_dfltTech; }

private:
	void _SetCompatTechnique();
	u8 _ValidateAllTechnique();
	void _InitSceneStuffFile(hFILE fp);
	void _InitSceneStuffBuff();
	void _CreateCommon(const tCHAR *name);

	//parse effect parameters
	u32 _ParseParam();

	//returns TRUE if given tech is compatible with the card,
	//o.w. FALSE, you should look at the next technique
	u8 _CheckCompatibility(GFXHANDLE tech);

	//parse the scene stuff from buffer
	u32 _SceneParseBuff(const s8 *buff);

	//parse the scene stuff from file
	u32 _SceneParseFile(const tCHAR *filepath);

	//parse the given buffer to scene commands
	u32 _SceneParseCommand(const tCHAR *buff);

	//used by SetParam
	void _SetLightParam(const gfxFXLH & lh, hLIGHT l);

	static map<wstring, hIEFFECT>  s_FXLIST;	//list of effects

	const tCHAR *m_name;		//this is a reference to our texture table (as long as we are here, this pointer is valid)

	LPD3DXEFFECT m_effect;		//the effect

	gfxEffectScn *m_pScn;		//if this Effect is a scene FX, then this is not NULL

	s32 m_ref;					//number of things refering to this

	//handles
	GFXHANDLE	m_mtx[FXT_MAX][FXMTX_MAX];	//matrices

	gfxFXLH		m_lDir;					//light direction handles
	gfxFXLH		m_lPoint;				//light point handles (currently only one...)

	GFXHANDLE	m_hMatDiffuse;        /* Diffuse color RGBA == xyzw */
    GFXHANDLE	m_hMatAmbient;        /* Ambient color RGB */
    GFXHANDLE	m_hMatSpecular;       /* Specular 'shininess' */
    GFXHANDLE	m_hMatEmissive;       /* Emissive color RGB */
    GFXHANDLE   m_hMatPower;          /* Sharpness if specular highlight */

	GFXHANDLE	m_hTime;				//global timer
	GFXHANDLE	m_hTimeElapse;		//global time elapse since gfxInit

	GFXHANDLE	m_hVPPixelSize;		//viewport pixel size

	GFXHANDLE	m_hBaseTxt[MAXBASETXT];	//base textures, use this for setting textures
										//for arbitrary FXs (so you don't have to worry much)

	vector<GFXHANDLE>  m_pLodTech;		//LOD techniques
	f32				   m_lodDist;		//the LOD distance measurement (squared)

	GFXHANDLE   m_dfltTech;			//default validated FX technique. (used when there are no LOD techs)

	EFFECTFUNC  m_fxCB;				//FX parameter callback (for setting user variables in effect)

	vector<gfxFXAnimTxt>	m_pAnimTxts;		//for animated textures (so we can swap textures)

	//for non-file effects
	const s8 *m_buff;						//source buffer...

	FLAGS m_flag;
};

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

typedef struct _gfxLightItm {
	gfxLight	lDat;
	f32			dist;
	f32			prior;		//priority for shadows, 0 is lowest
} gfxLightItm;

///////////////////////////////////////////////////////////////////////////////////////////////
// Scene Stuff
///////////////////////////////////////////////////////////////////////////////////////////////

//the output data when SHADOWCB is called
typedef struct _gfxShadowInfo {
	Vec3D *vOut;	//the vertex buffer
	u32  numVtx;	//number of vertices

	u16 *iOut;		//index buffer out

	u32  numFace;	//number of faces
} gfxShadowInfo;

//Shadow callback for retrieving vertices and indices (bLock is when we retrieve)
typedef void (* SHADOWCB) (void *usrDat, u32 usrParam, u8 bLock, gfxShadowInfo *pSIOut);

//this is the struct you want to pass in GFXShadowAdd(...)
typedef struct _ShadowDat {
	Vec3D			 lPos;		//light position in OBJECT SPACE!

	iRect			 scissorR;	//the scissor rectangle

	const Matrix	*pWrldMtx;	//pointer to world matrix

	f32				 extrude;	//extrude distance (for finite shadow cast)

	SHADOWCB		 cb;		//shadow callback
	void			*usrDat;	//shadow callback user data
	u32				 usrParam;	//shadow callback user param

	FLAGS			 flag;		//shadow flags
} ShadowDat;

//used by Scene stuff
typedef struct _SceneGeometry {
	hIEFFECT		fx;		//the effect to apply
	GFXHANDLE		tech;	//the effect technique

	f32				camDist;

	ID3DXBaseMesh  *pMesh;	//if this is not NULL, we use this for rendering
	u32				meshSub;//index to mesh's subset

	GFXVTXBUFF	   *pVtx;	//the vertex buffer to set
	GFXINDBUFF	   *pInd;	//the index buffer to set

	GFXPRIMITIVETYPE primType;

	u32				startVtx;
	u32				startInd;
	u32				numVtx;
	u32				numFaces;

	gfxMaterial		*pMat;	//material to set

	Matrix			*wrld;	//the world matrix to set (of size FXMTX_MAX) or a single matrix, flag has to be set

	EFFECTDATCB		fxCB;	//the effect callback
	u32				wParam;	//used by fxCB
	s32				lParam; //used by fxCB

	void		   *userDat;

	//light sets?
	const hLIGHT   *pLights;

	FLAGS			flag;
} SceneGeometry;

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
// Model Interface
///////////////////////////////////////////////////////////////////////////////////////////////

//used for Cal3D shadows and some other stuff...
typedef struct _gfxCal3D { 
	Vec3D			   *shadowVtx;		 //the shadow mesh vertex buffer
	u32					numVtx;			 //number of shadow vertex

	u8				   *shadowInd;		 //the shadow mesh index buffer
	u32					numInd;			 //number of shadow indices

	s32					shadowID;		 //the mesh ID in cal3d that has shadow (-1 for none, this is not rendered as a model)

	//u32					numFace;
} gfxCal3D;

typedef struct _gfxMeshSub {
	gfxMaterial			mat;
	hTXT				txt;	
	hEFFECT				fx;		//if txt is NULL, this is used
} gfxMeshSub;

//we will only use this for simple static meshes
typedef struct _gfxMesh {
	gfxMeshSub		   *subset;					 //the sub-sets (numSubset)
	u32					numSubset;

	ID3DXBaseMesh	   *mesh;					 //the actual mesh data

    LPD3DXPMESH		   *pPMeshes;				 //if not NULL, we use progressive mesh for LOD, mesh will be NULL.
    u32                 cPMeshes;
    u32                 iPMeshCur;


	ID3DXBaseMesh	   *shMesh;					 //the shadow mesh

	Vec3D				mins, maxs;				 //bound-box
} gfxMesh;

typedef class IgfxModel *hIMDL;

class IgfxModel : public gfxModel {
public:
	IgfxModel(const tCHAR *filename);

	~IgfxModel();

	void AddRef();
	s32  Release();

	s32		GetAnimID(const tCHAR *name);
	s32		GetNumAnim();
	s32		GetBoneID(const tCHAR *name);
	u32		GetNumMaterial();
	hEFFECT GetEffect(u32 ind);
	void	SetEffect(u32 ind, hEFFECT fx);
	hTXT	GetTxt(u32 ind, u32 mapInd);
	void	SetTxt(u32 ind, u32 mapInd, hTXT txt);
	void	GetMaterial(u32 ind, gfxMaterial *pMtrl);
	void	SetMaterial(u32 ind, const gfxMaterial *pMtrl);

	///////////////////////////////////////////
	//internal interface used by GFX

	/////////////////////////////////////
	// Purpose:	load cal3D model
	// Output:	model loaded to given model
	// Return:	TRUE if success
	/////////////////////////////////////
	u32 LoadCal3D(const tCHAR *filename);

	u32 Cal3DGetNumMat();
	hEFFECT Cal3DGetEffect(u32 ind);
	void Cal3DSetEffect(u32 ind, hEFFECT fx);
	hTXT Cal3DGetTxt(u32 ind, u32 mapInd);
	void Cal3DSetTxt(u32 ind, u32 mapInd, hTXT txt);
	void Cal3DGetMtrl(u32 ind, gfxMaterial *pMtrl);
	void Cal3DSetMtrl(u32 ind, const gfxMaterial *pMtrl);

	void Cal3DDestroy();

	/////////////////////////////////////
	// Purpose:	load the X file to model mesh
	// Output:	pMesh set in mdl
	// Return:	TRUE if success
	/////////////////////////////////////
	u32 LoadX(const tCHAR *filename);

	u32 XGetNumMat();
	hEFFECT XGetEffect(u32 ind);
	void XSetEffect(u32 ind, hEFFECT fx);
	hTXT XGetTxt(u32 ind, u32 mapInd);
	void XSetTxt(u32 ind, u32 mapInd, hTXT txt);
	void XGetMtrl(u32 ind, gfxMaterial *pMtrl);
	void XSetMtrl(u32 ind, const gfxMaterial *pMtrl);

	void XDestroy();

	//compute and get the LOD mesh to display
	ID3DXBaseMesh *XGetLOD(f32 distCamSQ);

	//get the Cal3D Core Model Interface
	inline class CalCoreModel *GetCalCoreModel() { return m_pCalCoreModel; }

	//get the .X file mesh
	inline gfxMesh			  *GetXMesh() { return m_pMesh; }

	//the model matrix
	inline const Matrix & GetMtx() { return m_mdlMtx; }

	//the LOD distance measurement (squared)
	inline f32 GetLODDist() { return m_lodDist; }

	//the shadow LOD distance (squared)
	inline f32 GetShadDist() { return m_shadDist; }

	//the default FX
	inline hEFFECT GetEffectDflt() { return m_fxDflt; }

	//vertex buffer pool
	inline GFXVTXBUFF *GetVtxBuff() { return m_vtxBuff;	}

	//index buffer pool
	inline GFXINDBUFF *GetIndBuff() { return m_indBuff;	}

	//flags
	inline FLAGS GetFlag() { return m_flag; }

	/////////////////////////////////////
	// Purpose:	forcibly destroy all 
	//			models
	// Output:	models destroyed
	// Return:	none
	/////////////////////////////////////
	static void sDestroyAll();

	static void sInvalidateAll();
	static void sRestoreAll();

	static hIMDL sFind(const tCHAR *name);

private:
	u32 _Cal3DCreateBuffer();

	//load some extra information regarding .X files
	void _XLoadTrans(const tCHAR *filename);

	static map<wstring, hIMDL>	s_MDLLIST;				 //list of models

	const tCHAR		   *m_name;					 //this is a reference to our model table (as long as we are here, this pointer is valid)

	class CalCoreModel *m_pCalCoreModel;		 //cal3D core model pointer

	gfxMesh			   *m_pMesh;				 //this is used if pCalCoreModel is NULL

	hEFFECT				m_fxDflt;				 //the default model FX

	GFXVTXBUFF		   *m_vtxBuff;				 //vertex buffer pool
	GFXINDBUFF		   *m_indBuff;				 //index buffer pool

	s32					m_ref;					 //number of things refering to this

	u32					m_fvf;					 //fvf upon creation

	f32					m_lodSDist;				 //the LOD starting distance measurement (squared)
	f32					m_lodDist;				 //the LOD distance measurement (squared)
	f32					m_shadDist;				 //the shadow LOD distance (squared)

	Matrix				m_mdlMtx;				 //the model space matrix

	FLAGS				m_flag;
};

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
// Object Interface
///////////////////////////////////////////////////////////////////////////////////////////////

//materials for object for individual customization
//all derived from model
typedef struct _gfxObjMat {
	hTXT				txt;
	hEFFECT				fx;
	gfxMaterial			mat;
} gfxObjMat;

typedef class IgfxObject *hIOBJ;

class IgfxObject : public gfxObject {
public:
	IgfxObject(hIMDL mdl);
	~IgfxObject();

	void AddRef();
	s32  Release();

	void Translate(const Vec3D & trans, u8 bAdd);
	void GetTranslate(Vec3D *pOut);
	void Scale(const Vec3D & scale, u8 bAdd);
	void GetScale(Vec3D *pOut);
	void SetAxisMtx(const Matrix *mtx);
	void GetAxisMtx(Matrix *mtxOut);
	void GetWorldMtx(Matrix *pMtx);
	u8	 MixerBlendCycle(s32 animID, f32 weight, f32 delay);
	u8   MixerClearCycle(s32 animID, f32 delay);
	u8   MixerExecuteAction(s32 animID, f32 delayIn, f32 delayOut, f32 weight, u8 bAutolock);
	f32  MixerGetDuration();
	f32  MixerGetTime();
	f32  MixerGetScale();
	void MixerSetScale(f32 s);
	f32  AnimGetDuration(s32 animID);
	f32  AnimGetTime(s32 animID);
	f32  AnimGetScale(s32 animID);
	void AnimSetScale(s32 animID, f32 s);
	hMDL GetModel();
	void BoneGetRotate(s32 boneID, Quaternion *pQ);
	void BoneGetRotateAbs(s32 boneID, Quaternion *pQ);
	void BoneGetRotateBoneSpace(s32 boneID, Quaternion *pQ);
	void BoneGetTrans(s32 boneID, Vec3D *pLoc);
	void BoneGetTransAbs(s32 boneID, Vec3D *pLoc);
	void BoneGetTransBoneSpace(s32 boneID, Vec3D *pLoc);
	void BoneGetTransformMtx(s32 boneID, Matrix *pMtx);
	void BoneSetTrans(s32 boneID, const Vec3D & loc);
	void BoneSetRotate(s32 boneID, const Quaternion & q);
	void GetAABB(Vec3D *pMin, Vec3D *pMax);
	void GetAABBWorld(Vec3D *pMin, Vec3D *pMax);
	void Update();
	u8   IsAttached();
	void RemoveFromParent();
	void RemoveChild(class gfxObject *child);
	u32  AddChild(class gfxObject *child, s32 parentJointInd);
	u32  Display(hEFFECT fxOverride, void *fxCBUserDat);
	u32  DisplaySkyBox(hEFFECT fxOverride, void *fxCBUserDat);
	void SetAlpha(f32 alphaVal);
	void ResetMaterial();
	hEFFECT GetEffect(u32 ind);
	void SetEffect(u32 ind, hEFFECT fx);
	hTXT GetTxt(u32 ind);
	void SetTxt(u32 ind, hTXT txt);
	void GetMaterial(u32 ind, gfxMaterial *pMtrl);
	void SetMaterial(u32 ind, const gfxMaterial *pMtrl);
	void EnableShadow(u8 bYes);
	void ReceiveShadow(u8 bYes);

	///////////////////////////////////////////
	//internal interface used by GFX
	
	//get the Cal3D CalModel interface
	inline class CalModel *GetCalModel() { return m_pCalModel; }

private:
	void _ComputeWrldMtx();

	//this is only used by Display and DisplaySkyBox
	void _RenderMDL(hEFFECT fxOverride, f32 lod_lvl, SceneGeometry & g);

	hIMDL				m_mdl;					 //the model this object uses
	class CalModel	   *m_pCalModel;			 //the cal3d model instance

	gfxObjMat		   *m_pObjMats;				 //material objects

	Vec3D				m_wrldTrans;
	Vec3D				m_wrldScl;

	Matrix				m_axis;					 //this is the axis of the obj in matrix form (we use the upperleft 3x3 portion)

	Matrix			    m_wrldMtx[FXMTX_MAX];	 //the world matrix

	list<hOBJ>		    m_objNode;				 //list of children attached to this object
	hIOBJ				m_parent;				 //the parent of this object
	s32					m_parentBoneID;			 //the bone ID this child is attached to (-1 for none)

	f32					m_alpha;				 //material alpha

	hLIGHT	            m_lights[MAXLIGHT];		 //this is used for getting the lights near this object

	s32					m_ref;					 //number of things refering to this

	FLAGS				m_flag;
};

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

//Background FX
typedef s32 (* BKEFFECT) (struct _gfxBkFX *thisBKFX, u32 message, LPARAM dumbParam, WPARAM otherParam);

typedef struct _gfxBkFX {
	s32 Type;		  //Specifies effect type
	void *Data;		  //Context to which effect is applied

	f32			 time;					//time accum.
	f32			 timeDelay;				//delay of fade

	BKEFFECT Effect;  //Callback function for effect
	FLAGS Status;	  //Indicates status of effect
} gfxBkFX;

typedef struct _gfxBSPFragment {
	s32				firstvert;
	s32				numverts;		// can't exceed MAX_POLY_VERTS
} gfxBSPFragment;

typedef struct _gfxDecal {
	Vec3D			origin;
	Vec3D			axis[3];

	Vec4D			clr;
	
	hTXT			txt;

	Vec3D		    verts[MAX_DECAL_VERTS];

	gfxBSPFragment  fragments[MAX_DECAL_FRAGMENTS];
	s16			    numfragments;
} gfxDecal;

////////////////////////////////
// Globals
//

extern gfxMode				 g_mode;

extern PGFXMAIN				 g_p3D;
extern PGFXDEVICE			 g_p3DDevice;

extern ID3DXMatrixStack		*g_pWrldStack;

//projection stuff
extern f32					 g_fov;			//the current fov

extern Matrix				 g_mtxProj[FXMTX_MAX];		//projection matrix
extern Matrix				 g_mtxView[FXMTX_MAX];		//view matrix (camera)

extern Matrix				 g_mtxViewProj[FXMTX_MAX];	//concatinated matrix 
														//of view and projection (view*proj)
														//this is set whenever we change mtxView or mtxProj

extern iRect				 g_CLIENTRECT;
extern FLAGS				 g_FLAGS;

//time elapsed since last update (milliseconds)
extern f32				     g_time;
extern f32				     g_timeElapse;

extern GFXCAPS				 g_d3dcaps;
extern D3DPRESENT_PARAMETERS g_d3dParam;

//this is set everytime we call CameraSetScene
extern Vec3D				 g_eyePt;
extern Vec3D				 g_eyeVec;

extern u32					 g_numTri;

extern iPoint				 g_txtLim; //(x = min, y = max)

////////////////////////////////
// Functions
//

//
// Main
//

u8 GFXCheckError(HRESULT hr, const tCHAR *header);

//set the other matrix transformation for view*projection 
//(assuming view and projection matrix has been set already)
//this will set the normal,inverse,transpose of viewproj mtx
void GFXSetViewProjMtxFX();

//check if given texture width and height is valid for loading,
//this will spit out the correct w and h
//(for 2D textures)
void GFXValidateTxtSize(u32 *w, u32 *h);

//for volume
void GFXValidateVolTxtSize(u32 *w, u32 *h, u32 *depth);

//for cube
void GFXValidateCubeTxtSize(u32 *size);

//
// Buffers
//

//add a new stream to the given buffer
u32 GFXAppendVtxBuff(GFXVTXBUFF *vtx, u32 size, u32 count, u32 usage, u32 FVF, GFXPOOL pool);

GFXVTXBUFF *GFXCreateVtxBuff(u32 size, u32 count, u32 usage, u32 FVF, GFXPOOL pool);

void GFXDestroyVtxBuff(GFXVTXBUFF *vtx);

//size can only be 16 or 32, other values will default to 16
u32 GFXCreateIndBuff(u32 len, u32 usage, u32 size, GFXPOOL pool, GFXINDBUFF **indBuffOut);

//Vertex

u32 VtxGetCount(GFXVTXBUFF *vtx, u32 ind);
u32 VtxGetStride(GFXVTXBUFF *vtx, u32 ind);

//for entire buffer, set ofs and size to 0
u32 VtxLock(GFXVTXBUFF *vtx, u32 ind, u32 ofs, u32 size, void **dat, u32 flags);

void VtxUnlock(GFXVTXBUFF *vtx, u32 ind);

void VtxSetDecl(GFXVTXBUFF *vtx, D3DVERTEXELEMENT9 *pDecl);

//force to use the first stream with it's FVF
void VtxSetFVF(GFXVTXBUFF *vtx);

//set the stream source, decl/fvf of given vtx
void VtxSet(GFXVTXBUFF *vtx);

void VtxUnset(GFXVTXBUFF *vtx);

//Index

//for entire buffer, set ofs and size to 0
u32 IndLock(GFXINDBUFF *ind, u32 ofs, u32 size, void **dat, u32 flags);

void IndUnlock(GFXINDBUFF *ind);

//
// Camera
//

//this will get the farthest distance (squared) of given AABB from current camera loc/vec
f32 CameraGetBoundFarDistSq(const Vec3D & min, const Vec3D & max, const Matrix * pWrldMtx);

//
// Frustrum
//

/////////////////////////////////////
// Name:	FrustrumCalculate
// Purpose:	calculate the viewing
//			frustrum
// Output:	frustrum plane calculated
// Return:	none
/////////////////////////////////////
void FrustrumCalculate();

//
// Lights
//

//call this after the device has been restored
void LightOnReset();

void LightCullView(const Vec3D &eye);

/////////////////////////////////////
// Name:	LightGetAllNearest
// Purpose:	add lights that are near
//			given world AABB to given list
// Output:	lights added to given list
// Return:	none
/////////////////////////////////////
void LightGetAllNearest(const Vec3D & mins, const Vec3D &maxs, hLIGHT *pLights);

/////////////////////////////////////
// Name:	LightEnableList
// Purpose:	add lights from list
//			to graphics device
// Output:	lights set
// Return:	
/////////////////////////////////////
void LightEnableList(const hLIGHT *pLights);

/////////////////////////////////////
// Name:	LightDisableList
// Purpose:	remove lights from list
//			from graphics device
// Output:	lights unset
// Return:	
/////////////////////////////////////
void LightDisableList();

//get the light we will use for shadows (NULL for none)
hLIGHT _LightGetShadow(hLIGHT lights[MAXLIGHT]);

//
// Scene
//

/////////////////////////////////////
// Name:	GFXSceneDestroyBuffer
// Purpose:	destroy the scene buffer
// Output:	scene buffer destroyed
// Return:	none
/////////////////////////////////////
void GFXSceneDestroyBuffer();

/////////////////////////////////////
// Name:	GFXSceneSetBufferSize
// Purpose:	set the scene buffer size
// Output:	scene buffer set
// Return:	TRUE if successful
/////////////////////////////////////
u32 GFXSceneSetBufferSize(u32 size);

/////////////////////////////////////
// Name:	GFXSceneAddMesh
// Purpose:	append a scene mesh to 
//			render (pWrldMtx must be
//			an array of size FXMTX_MAX)
//			flags are: SCENE_*
// Output:	scene buffer updated
// Return:	TRUE if success
/////////////////////////////////////
u32 GFXSceneAddMesh(hEFFECT fx, GFXHANDLE fxTech, ID3DXBaseMesh *pMesh, u32 meshSub,
					const Matrix *pWrldMtx, FLAGS flag, const gfxMaterial *pMtrl, 
					const hLIGHT *pLights, EFFECTDATCB fxCB, u32 wParam, 
					s32 lParam, void *userDat);

/////////////////////////////////////
// Name:	GFXSceneAdd
// Purpose:	append a scene to render
//			(pWrldMtx must be an array 
//			of size FXMTX_MAX)
//			flags are: SCENE_*
// Output:	scene buffer updated
// Return:	TRUE if success
/////////////////////////////////////
u32 GFXSceneAdd(const SceneGeometry & geo);

//just sort the scene, no rendering
void _GFXSceneSort();

//render the scene with no sorting
void _GFXSceneRenderNoSort();

//render the scene with FX override and no sorting
void _GFXSceneRenderNoSortFXOvride(hIEFFECT fx, u32 pass);

//
// SkyBox
//

/////////////////////////////////////
// Name:	SkyBoxRender
// Purpose:	renders the skybox
// Output:	sky box rendered
// Return:	success if success
/////////////////////////////////////
u32 SkyBoxRender();

//
// Screen thing
//

/////////////////////////////////////
// Name:	GFXScreenCreate
// Purpose:	create the surface screen
//			and the textures
// Output:	initializes the surface
//			and textures
// Return:	none
/////////////////////////////////////
void GFXScreenCreate(u32 w, u32 h);

/////////////////////////////////////
// Name:	GFXScreenDestroy
// Purpose:	destroy the buffer screen
//			and textures
// Output:	stuff destroyed
// Return:	none
/////////////////////////////////////
void GFXScreenDestroy();

//
// Fog stuff
//

//call this after the device has been restored
void FogOnReset();

//
// Particle system
//

/////////////////////////////////////
// Name:	ParSysInit
// Purpose:	initializes the vertex
//			buffers and indices and
//			default FX
// Output:	stuff initialized
// Return:	TRUE if success
/////////////////////////////////////
u32 ParSysInit();

/////////////////////////////////////
// Name:	ParSysDestroy
// Purpose:	destroy all buffers and
//			default FX
// Output:	all buffers destroyed and FX
// Return:	none
/////////////////////////////////////
void ParSysDestroy();

//
// Shadow interface
//

//just call this in graphics termination
void GFXShadowDestroyShade();

//call this on graphics init and whenever we change screen size
u32 GFXShadowInitShade();

void GFXShadowDestroy();

/////////////////////////////////////
// Name:	GFXShadowInit
// Purpose:	this will create the shadow
//			pool data.  if each count 
//			is greater than the current, 
//			then we re-allocate
//			the pool to the new size
// Output:	shadow data initialized
// Return:	TRUE if success
/////////////////////////////////////
u32 GFXShadowInit(u32 vtxCount, u32 edgeCount, u32 shadowCount);

void GFXShadowInvalidate();
void GFXShadowRestore();

/////////////////////////////////////
// Name:	GFXShadowAdd
// Purpose:	add an occluder to cast
//			shadow.
//			lPos is light in obj space
//			callback is for retrieving
//			vertex/index buffer when
//			building shadow vol.
//			for SHADOW_USE_DIR, 
//			we assume that the dir is 
//			normalized
// Output:	occluder added
// Return:	none
/////////////////////////////////////
void GFXShadowAdd(const ShadowDat *pShad);

/////////////////////////////////////
// Name:	GFXShadowClear
// Purpose:	clear out occluders
//			used only by GFXSceneClear()
// Output:	numShadow set to 0
// Return:	none
/////////////////////////////////////
void GFXShadowClear();

//build all shadows from occluders and render them to stencil
//this assumes that depth buffer has been filled already by the scene
//so this is done after scene is rendered
u32 GFXShadowExecute();

//darken areas that are in shadow, this is after stencil has been updated
//so you call this anytime after GFXShadowExecute, so long as stencil data
//is not changed
void GFXShadowRenderShade();


//
// Line
//

void GFXLineDestroy();
s32 GFXLineInit();
void GFXLineInvalidate();
void GFXLineRestore();

#endif