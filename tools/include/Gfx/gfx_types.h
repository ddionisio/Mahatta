#ifndef _gfx_types_h
#define _gfx_types_h

#include "GFX\gfx_math_types.h"

/////////////////////////////////////////////
// Defines
//

//return codes
#define GFXRET_BREAK               -1
#define GFXRET_FAILURE				0
#define GFXRET_SUCCESS				1

//usages
#define GFXUSAGE_RENDERTARGET       (0x00000001L)
#define GFXUSAGE_DEPTHSTENCIL       (0x00000002L)
#define GFXUSAGE_DYNAMIC            (0x00000200L)

//usages for vertex/index buffers

#define GFXUSAGE_WRITEONLY          (0x00000008L)
#define GFXUSAGE_SOFTWAREPROCESSING (0x00000010L)
#define GFXUSAGE_DONOTCLIP          (0x00000020L)
#define GFXUSAGE_POINTS             (0x00000040L)
#define GFXUSAGE_RTPATCHES          (0x00000080L)
#define GFXUSAGE_NPATCHES           (0x00000100L)

// Flexible vertex format bits

#define GFXFVF_RESERVED0        0x001
#define GFXFVF_POSITION_MASK    0x400E
#define GFXFVF_XYZ              0x002
#define GFXFVF_XYZRHW           0x004
#define GFXFVF_XYZB1            0x006
#define GFXFVF_XYZB2            0x008
#define GFXFVF_XYZB3            0x00a
#define GFXFVF_XYZB4            0x00c
#define GFXFVF_XYZB5            0x00e
#define GFXFVF_XYZW             0x4002

#define GFXFVF_NORMAL           0x010
#define GFXFVF_PSIZE            0x020
#define GFXFVF_DIFFUSE          0x040
#define GFXFVF_SPECULAR         0x080

#define GFXFVF_TEXCOUNT_MASK    0xf00
#define GFXFVF_TEXCOUNT_SHIFT   8
#define GFXFVF_TEX0             0x000
#define GFXFVF_TEX1             0x100
#define GFXFVF_TEX2             0x200
#define GFXFVF_TEX3             0x300
#define GFXFVF_TEX4             0x400
#define GFXFVF_TEX5             0x500
#define GFXFVF_TEX6             0x600
#define GFXFVF_TEX7             0x700
#define GFXFVF_TEX8             0x800

#define GFXFVF_LASTBETA_UBYTE4   0x1000
#define GFXFVF_LASTBETA_D3DCOLOR 0x8000

#define GFXFVF_RESERVED2         0x6000  // 2 reserved bits

//vector aabb returns
#define BBOX_IN			1
#define BBOX_OUT		0
#define BBOX_INTERSECT	2

/////////////////////////////////////////////
// Macros
//

// maps unsigned 8 bits/channel to D3DCOLOR
#define GFXCOLOR_ARGB(a,r,g,b) \
    ((u32)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define GFXCOLOR_RGBA(r,g,b,a) GFXCOLOR_ARGB(a,r,g,b)
#define GFXCOLOR_XRGB(r,g,b)   GFXCOLOR_ARGB(0xff,r,g,b)

#define GFXCOLOR_XYUV(y,u,v)   GFXCOLOR_ARGB(0xff,y,u,v)
#define GFXCOLOR_AYUV(a,y,u,v) GFXCOLOR_ARGB(a,y,u,v)

// maps floating point channels (0.f to 1.f range) to D3DCOLOR
#define GFXCOLOR_COLORVALUE(r,g,b,a) \
    GFXCOLOR_RGBA((u32)((r)*255.f),(u32)((g)*255.f),(u32)((b)*255.f),(u32)((a)*255.f))

inline u32 GFX_F2DW( f32 f ){ return *((u32*)&f); }

/////////////////////////////////////////////
// Types
//

/////////////////////////////////////////////
// Flags
//

//used by GFXClear
#define GFXCLEAR_TARGET            0x00000001l  /* Clear target surface */
#define GFXCLEAR_ZBUFFER           0x00000002l  /* Clear target z buffer */
#define GFXCLEAR_STENCIL           0x00000004l  /* Clear stencil planes */
#define GFXCLEAR_ALL			   (GFXCLEAR_TARGET | GFXCLEAR_ZBUFFER | GFXCLEAR_STENCIL)

//used by GFXBlt
#define GFXBLT_FLIPH			   0x00000001l	//flip image horizontal
#define GFXBLT_FLIPV			   0x00000002l	//flip image vertical
#define GFXBLT_ROTABS			   0x00000004l	//rotation is done at 0,0

//used by font
// FontCreate(...) flags, not used anywhere else
#define GFXFONT_BOLD        0x00000001
#define GFXFONT_ITALIC      0x00000002
#define GFXFONT_ANTIALIAS   0x00000004

// Font rendering flags for FontTextBox(...)
#define GFXDT_CENTER                0x00000001	//Centers text horizontally in the rectangle.
#define GFXDT_EXPANDTABS            0x00000040  //Expands tab characters. The default number of characters per tab is eight.
#define GFXDT_LEFT                  0x00000000	//Aligns text to the left. 
#define GFXDT_NOCLIP                0x00000100	//Draws without clipping. ID3DXFont::DrawText is somewhat faster when DT_NOCLIP is used. 
#define GFXDT_RIGHT                 0x00000002	//Aligns text to the right. 
#define GFXDT_RTLREADING            0x00020000	//Displays text in right-to-left reading order for bi-directional text when a Hebrew or Arabic font is selected. The default reading order for all text is left-to-right. 
#define GFXDT_SINGLELINE            0x00000020	//Displays text on a single line only. Carriage returns and line feeds do not break the line. 
#define GFXDT_TOP                   0x00000000	//Top-justifies text. 
#define GFXDT_VCENTER               0x00000004	//Centers text vertically (single line only). 
#define GFXDT_WORDBREAK             0x00000010	//Breaks words. Lines are automatically broken between words if a word would extend past the edge of the rectangle specified by the pRect parameter. A carriage return/line feed sequence also breaks the line. 

#define GFXDT_BOTTOM                0x00000008
#define GFXDT_TABSTOP               0x00000080
#define GFXDT_EXTERNALLEADING       0x00000200
#define GFXDT_NOPREFIX              0x00000800
#define GFXDT_INTERNAL              0x00001000

////////////////////////////////////////////
// Enums
//

typedef enum _GFXFORMAT
{
    GFXFMT_UNKNOWN              =  0,

    GFXFMT_R8G8B8               = 20,
    GFXFMT_A8R8G8B8             = 21,
    GFXFMT_X8R8G8B8             = 22,
    GFXFMT_R5G6B5               = 23,
    GFXFMT_X1R5G5B5             = 24,
    GFXFMT_A1R5G5B5             = 25,
    GFXFMT_A4R4G4B4             = 26,
    GFXFMT_R3G3B2               = 27,
    GFXFMT_A8                   = 28,
    GFXFMT_A8R3G3B2             = 29,
    GFXFMT_X4R4G4B4             = 30,
    GFXFMT_A2B10G10R10          = 31,
    GFXFMT_A8B8G8R8             = 32,
    GFXFMT_X8B8G8R8             = 33,
    GFXFMT_G16R16               = 34,
    GFXFMT_A2R10G10B10          = 35,
    GFXFMT_A16B16G16R16         = 36,

    GFXFMT_A8P8                 = 40,
    GFXFMT_P8                   = 41,

    GFXFMT_L8                   = 50,
    GFXFMT_A8L8                 = 51,
    GFXFMT_A4L4                 = 52,

    GFXFMT_V8U8                 = 60,
    GFXFMT_L6V5U5               = 61,
    GFXFMT_X8L8V8U8             = 62,
    GFXFMT_Q8W8V8U8             = 63,
    GFXFMT_V16U16               = 64,
    GFXFMT_A2W10V10U10          = 67,

    GFXFMT_UYVY                 = MAKETAG_4('U', 'Y', 'V', 'Y'),
    GFXFMT_R8G8_B8G8            = MAKETAG_4('R', 'G', 'B', 'G'),
    GFXFMT_YUY2                 = MAKETAG_4('Y', 'U', 'Y', '2'),
    GFXFMT_G8R8_G8B8            = MAKETAG_4('G', 'R', 'G', 'B'),
    GFXFMT_DXT1                 = MAKETAG_4('D', 'X', 'T', '1'),
    GFXFMT_DXT2                 = MAKETAG_4('D', 'X', 'T', '2'),
    GFXFMT_DXT3                 = MAKETAG_4('D', 'X', 'T', '3'),
    GFXFMT_DXT4                 = MAKETAG_4('D', 'X', 'T', '4'),
    GFXFMT_DXT5                 = MAKETAG_4('D', 'X', 'T', '5'),

    GFXFMT_D16_LOCKABLE         = 70,
    GFXFMT_D32                  = 71,
    GFXFMT_D15S1                = 73,
    GFXFMT_D24S8                = 75,
    GFXFMT_D24X8                = 77,
    GFXFMT_D24X4S4              = 79,
    GFXFMT_D16                  = 80,

    GFXFMT_D32F_LOCKABLE        = 82,
    GFXFMT_D24FS8               = 83,


    GFXFMT_L16                  = 81,

    GFXFMT_VERTEXDATA           =100,
    GFXFMT_INDEX16              =101,
    GFXFMT_INDEX32              =102,

    GFXFMT_Q16W16V16U16         =110,

    GFXFMT_MULTI2_ARGB8         = MAKETAG_4('M','E','T','1'),

    // Floating point surface formats

    // s10e5 formats (16-bits per channel)
    GFXFMT_R16F                 = 111,
    GFXFMT_G16R16F              = 112,
    GFXFMT_A16B16G16R16F        = 113,

    // IEEE s23e8 formats (32-bits per channel)
    GFXFMT_R32F                 = 114,
    GFXFMT_G32R32F              = 115,
    GFXFMT_A32B32G32R32F        = 116,

    GFXFMT_CxV8U8               = 117,


    GFXFMT_FORCE_DWORD          =0x7fffffff
} GFXFORMAT;

typedef enum _GFXPOOL {
    GFXPOOL_DEFAULT = 0,
    GFXPOOL_MANAGED = 1,
    GFXPOOL_SYSTEMMEM = 2,
    GFXPOOL_SCRATCH = 3,
    GFXPOOL_FORCE_DWORD = 0x7fffffff
} GFXPOOL;

typedef enum _GFXLIGHTTYPE {
    GFXLIGHT_POINT          = 1,
    GFXLIGHT_SPOT           = 2,
    GFXLIGHT_DIRECTIONAL    = 3,
    GFXLIGHT_FORCE_DWORD    = 0x7fffffff, /* force 32-bit size enum */
} GFXLIGHTTYPE;

//primitive render types
typedef enum _GFXPRIMITIVETYPE {
    GFXPT_POINTLIST             = 1,
    GFXPT_LINELIST              = 2,
    GFXPT_LINESTRIP             = 3,
    GFXPT_TRIANGLELIST          = 4,
    GFXPT_TRIANGLESTRIP         = 5,
    GFXPT_TRIANGLEFAN           = 6,
    GFXPT_FORCE_DWORD           = 0x7fffffff, /* force 32-bit size enum */
} GFXPRIMITIVETYPE;

//fog modes
typedef enum _GFXFOGMODE {
    GFXFOG_NONE = 0,
    GFXFOG_EXP = 1,
    GFXFOG_EXP2 = 2,
    GFXFOG_LINEAR = 3,
    GFXFOG_FORCE_DWORD = 0x7fffffff
} GFXFOGMODE;

typedef enum {
	GFXRTYPE_TEXTURE = 3,
    GFXRTYPE_VOLUMETEXTURE = 4,
    GFXRTYPE_CUBETEXTURE = 5
} GFXRESOURCETYPE;

////////////////////////////////////////////
// Structs
//

typedef struct _gfxMode {
	u8	bFullscrn;	//TRUE for fullscreen

	u32 width;		//scrn width
	u32 height;		//scrn height
	GFXFORMAT fmt;	//format (bpp)
	u32 refresh;	//refresh rate (usu. 0)

	u8	bVsync;		//TRUE to use vsync, FALSE o.w.

	f32 depthNear;	//depth near (usu. 1.0f)
	f32 depthFar;	//depth far (usu. 1000.0f)
} gfxMode;

//for using FontCreateIndirect
typedef struct _gfxFontDesc {
    u32 Height;
    u32 Width;
    u32 Weight;
    u32 MipLevels;
    u8  Italic;
    u8  CharSet;
    u8  OutputPrecision;
    u8  Quality;
    u8  PitchAndFamily;
    tCHAR FaceName[32];
} gfxFontDesc;

typedef struct _gfxSprFrame {
	u32 firstframe; //Index of first frame
	u32 lastframe;	//Index of last frame
	u32 curFrame;	//Index of current frame
} gfxSprFrame;

typedef struct _gfxBlt {
	u32	clr;	//the color modulated
	f32 rot;	//rotation in radians
	f32 rotOfsX;//rotation offset in X (usu. 0)
	f32 rotOfsY;//rotation offset in Y (usu. 0)
	u32 flags;	//flags specifying behavior (GFXBLT_*)
} gfxBlt;

typedef struct _gfxCamera {
	Vec3D vEye;		//the eye position
	Vec3D vTarget;	//the eye target
	Vec3D vDir;		//the eye direction (forward vector)
	Vec3D vUp;		//the 'up' vector (always the same)
} gfxCamera;

typedef struct _gfxMaterial {
    Vec4D		Diffuse;        /* Diffuse color RGBA == xyzw */
    Vec4D		Ambient;        /* Ambient color RGB */
    Vec4D		Specular;       /* Specular 'shininess' */
    Vec4D		Emissive;       /* Emissive color RGB */
    f32	        Power;          /* Sharpness if specular highlight */
} gfxMaterial;

typedef struct _gfxLight {
    GFXLIGHTTYPE    Type;            /* Type of light source */
    Vec4D			Diffuse;         /* Diffuse color of light */
    Vec4D			Specular;        /* Specular color of light */
    Vec4D			Ambient;         /* Ambient color of light */
    Vec3D			Position;         /* Position in world space */
    Vec3D			Direction;        /* Direction in world space */
    f32           	Range;            /* Cutoff range */
    f32           	Falloff;          /* Falloff */
    f32           	Attenuation0;     /* Constant attenuation */
    f32           	Attenuation1;     /* Linear attenuation */
    f32           	Attenuation2;     /* Quadratic attenuation */
    f32           	Theta;            /* Inner angle of spotlight cone */
    f32           	Phi;              /* Outer angle of spotlight cone */
} gfxLight;

#define MAX_ENT_CLUSTERS	16

//For objects who want to associate with the QBSP
//Used for culling objects from QBSP and setting
//area portals
typedef struct _gfxEnt {
	s32		number;					 //ID of sorts for area portalization
	s32		num_clusters;			 //if -1, use headnode instead
	s32		clusternums[MAX_ENT_CLUSTERS];
	s32		headnode;				 //unused if num_clusters != -1
	s32		areanum, areanum2;		 //used for portalization
} gfxEnt;

//GFX QBSP collision data
typedef struct _gfxTrace {
	u32		txtInd;					 //texture index in QBSP
	u8		bAllSolid;				 //true if plane is not valid
	u8		bStartSolid;			 //true if initial point was in solid area
	f32		t;						 //time parameter (1.0 = no hit)
	
	Vec3D	norm;					 //the normal of plane
	f32		d;						 //the plane element 'd', plane distance from origin

	s32		leafIndex;				 //the leaf index of collision
	s32		brushIndex;				 //the brush index of collision (-1 for none)
} gfxTrace;

//particle system data for updating particles
typedef struct _gfxParInfo
{
	Vec3D	emitter_pos;		// the new position of the emitter
	u8		disable;			// true to disable the emitter temporarily
} gfxParInfo;

/////////////////////////////////////////////////////////////////
// Callbacks
//

//texture custom create
typedef void (* TXTCSMCREATE) (class gfxTexture *txt);

//parse an entity from QBSP map
typedef void (* ENTITYPARSEFUNC) (tCHAR *entityStr, class gfxQBSP *qbsp, void *userDat);

//Effect set parameters callback
typedef void (* EFFECTFUNC) (class gfxEffect *fx, void *userDat);

//light cull callback, return TRUE if culled, FALSE o.w.
typedef u8 (* LIGHTCULLFUNC) (const Vec3D & pt, const gfxLight & light);

//particle collision 
//bCollideOBJ is true if we want to collide with objects
//you are required to fill: Vec3D *pImpact, Vec3D *pNorm, f32 *pT
typedef void (* PARCOLLFUNC) (const Vec3D & pt1, const Vec3D & pt2, 
							  u8 bCollideOBJ, Vec3D *pImpact, Vec3D *pNorm, f32 *pT);

/////////////////////////////////////////////
// Handles
//

typedef const char			*GFXHANDLE;

typedef class gfxTexture {
public:
	virtual void AddRef() = 0;
	virtual s32  Release() = 0;	

	virtual u32  BeginScene() = 0;
	virtual void EndScene() = 0;

	virtual u32  GetWidth() = 0;
	virtual u32  GetHeight() = 0;
	virtual u32  Lock(u32 lvl, const iRect *pRect, u32 flags, s32 *pitchOut, void **bitsOut) = 0;
	virtual void Unlock(u32 lvl) = 0;
	
	virtual u32  Blt(f32 x, f32 y, const iRect *srcRect, gfxBlt *pBlt) = 0;
	virtual u32  StretchBlt(f32 x, f32 y, f32 w, f32 h, const iRect *srcRect, gfxBlt *pBlt) = 0;
	virtual u32  Blt3D(const Matrix *wrldMtx, const Vec4D *pClr) = 0;
	virtual u32  BltBillboard(const Vec3D & center, f32 size, f32 rot, const Vec4D *pClr) = 0;
} *hTXT;

typedef class gfxFont {
public:
	virtual void AddRef() = 0;
	virtual s32  Release() = 0;	

	virtual u32  Resize(s32 size) = 0;
	virtual u32  TextBox(const tCHAR *text, s32 count, const iRect *r, u32 flags, u32 color) = 0;
	virtual u32  Printf(f32 x, f32 y, u32 color, u32 flags, const tCHAR *text, ...) = 0;
	virtual void GetStrSize(const tCHAR *str, iPoint *pSize) = 0;
} *hFNT;

typedef class gfxSprite {
public:
	virtual void AddRef() = 0;
	virtual s32  Release() = 0;	

	virtual u8   IsStateEnd() = 0;
	virtual void Pause(u8 bPause) = 0;
	virtual void PauseOnStateEnd(u8 bPause) = 0;
	virtual void UpdateFrame() = 0;
	virtual void UpdateFrameAllState() = 0;
	virtual u32  GetMaxState() = 0;
	virtual u32  GetCurState() = 0;
	virtual u32  SetState(u32 state) = 0;
	virtual const iPoint & GetSize() = 0;
	virtual u32  Blt(f32 x, f32 y, gfxBlt *pBlt) = 0;
	virtual u32  StretchBlt(f32 x, f32 y, f32 w, f32 h, gfxBlt *pBlt) = 0;
	virtual class gfxSprite * Duplicate() = 0;
} *hSPRITE;

typedef class gfxEffect {
public:
	virtual void AddRef() = 0;
	virtual s32  Release() = 0;	

	virtual GFXHANDLE GetParam(GFXHANDLE parent, const tCHAR *name) = 0;
	virtual u8		  GetValue(GFXHANDLE param, void *dat, u32 numBytes) = 0;
	virtual u8		  SetValue(GFXHANDLE param, void *dat, u32 numBytes) = 0;
	virtual u8		  SetTexture(GFXHANDLE param, hTXT txt) = 0;
	virtual GFXHANDLE GetTechnique(const tCHAR *name) = 0;
	virtual u32		  GetNextValidTechnique(GFXHANDLE tech, GFXHANDLE *techOut) = 0;
	virtual u8		  ValidateTechnique(GFXHANDLE tech) = 0;
	virtual u8		  SetTechnique(GFXHANDLE tech) = 0;
	virtual void	  SetCallback(EFFECTFUNC fxCB) = 0;

	virtual u32		  SceneRender() = 0;
} *hEFFECT;

typedef struct _gfxLightItm	*hLIGHT;

typedef class gfxModel {
public:
	virtual void AddRef() = 0;
	virtual s32  Release() = 0;

	virtual s32		GetAnimID(const tCHAR *name) = 0;
	virtual s32		GetNumAnim() = 0;
	virtual s32		GetBoneID(const tCHAR *name) = 0;
	virtual u32		GetNumMaterial() = 0;
	virtual hEFFECT GetEffect(u32 ind) = 0;
	virtual void	SetEffect(u32 ind, hEFFECT fx) = 0;
	virtual hTXT	GetTxt(u32 ind, u32 mapInd) = 0;
	virtual void	SetTxt(u32 ind, u32 mapInd, hTXT txt) = 0;
	virtual void	GetMaterial(u32 ind, gfxMaterial *pMtrl) = 0;
	virtual void	SetMaterial(u32 ind, const gfxMaterial *pMtrl) = 0;
} *hMDL;

typedef class gfxObject	{
public:
	virtual void AddRef() = 0;
	virtual s32  Release() = 0;

	virtual void Translate(const Vec3D & trans, u8 bAdd) = 0;
	virtual void GetTranslate(Vec3D *pOut) = 0;
	virtual void Scale(const Vec3D & scale, u8 bAdd) = 0;
	virtual void GetScale(Vec3D *pOut) = 0;
	virtual void SetAxisMtx(const Matrix *mtx) = 0;
	virtual void GetAxisMtx(Matrix *mtxOut) = 0;
	virtual void GetWorldMtx(Matrix *pMtx) = 0;
	virtual u8	 MixerBlendCycle(s32 animID, f32 weight, f32 delay) = 0;
	virtual u8   MixerClearCycle(s32 animID, f32 delay) = 0;
	virtual u8   MixerExecuteAction(s32 animID, f32 delayIn, f32 delayOut, f32 weight, u8 bAutolock) = 0;
	virtual f32  MixerGetDuration() = 0;
	virtual f32  MixerGetTime() = 0;
	virtual f32  MixerGetScale() = 0;
	virtual void MixerSetScale(f32 s) = 0;
	virtual f32  AnimGetDuration(s32 animID) = 0;
	virtual f32  AnimGetTime(s32 animID) = 0;
	virtual f32  AnimGetScale(s32 animID) = 0;
	virtual void AnimSetScale(s32 animID, f32 s) = 0;
	virtual hMDL GetModel() = 0;
	virtual void BoneGetRotate(s32 boneID, Quaternion *pQ) = 0;
	virtual void BoneGetRotateAbs(s32 boneID, Quaternion *pQ) = 0;
	virtual void BoneGetRotateBoneSpace(s32 boneID, Quaternion *pQ) = 0;
	virtual void BoneGetTrans(s32 boneID, Vec3D *pLoc) = 0;
	virtual void BoneGetTransAbs(s32 boneID, Vec3D *pLoc) = 0;
	virtual void BoneGetTransBoneSpace(s32 boneID, Vec3D *pLoc) = 0;
	virtual void BoneGetTransformMtx(s32 boneID, Matrix *pMtx) = 0;
	virtual void BoneSetTrans(s32 boneID, const Vec3D & loc) = 0;
	virtual void BoneSetRotate(s32 boneID, const Quaternion & q) = 0;
	virtual void GetAABB(Vec3D *pMin, Vec3D *pMax) = 0;
	virtual void GetAABBWorld(Vec3D *pMin, Vec3D *pMax) = 0;
	virtual void Update() = 0;
	virtual u8   IsAttached() = 0;
	virtual void RemoveFromParent() = 0;
	virtual void RemoveChild(class gfxObject *child) = 0;
	virtual u32  AddChild(class gfxObject *child, s32 parentJointInd) = 0;
	virtual u32  Display(hEFFECT fxOverride, void *fxCBUserDat) = 0;
	virtual u32  DisplaySkyBox(hEFFECT fxOverride, void *fxCBUserDat) = 0;
	virtual void SetAlpha(f32 alphaVal) = 0;
	virtual void ResetMaterial() = 0;
	virtual hEFFECT GetEffect(u32 ind) = 0;
	virtual void SetEffect(u32 ind, hEFFECT fx) = 0;
	virtual hTXT GetTxt(u32 ind) = 0;
	virtual void SetTxt(u32 ind, hTXT txt) = 0;
	virtual void GetMaterial(u32 ind, gfxMaterial *pMtrl) = 0;
	virtual void SetMaterial(u32 ind, const gfxMaterial *pMtrl) = 0;
	virtual void EnableShadow(u8 bYes) = 0;
	virtual void ReceiveShadow(u8 bYes) = 0;

} *hOBJ;

typedef class gfxQBSP {
public:
	virtual void AddRef() = 0;
	virtual s32  Release() = 0;

	virtual s32  FindLeaf(const Vec3D & pt) = 0;
	virtual hEFFECT GetEffect(u32 txtInd) = 0;
	virtual hTXT GetTexture(u32 txtInd) = 0;
	virtual u32  GetTextureFlags(u32 txtInd) = 0;
	virtual u32  GetTextureContents(u32 txtInd) = 0;
	virtual void WritePortalState(hFILE f) = 0;
	virtual void ReadPortalState(hFILE f) = 0;
	virtual u8   InPVS(const Vec3D & p1, const Vec3D & p2) = 0;
	virtual u8   InPHS(const Vec3D & p1, const Vec3D & p2) = 0;
	virtual u32	 GetNumModels() = 0;
	virtual void RayModelGetBBoxMin(u32 modelInd, Vec3D *pOut) = 0;
	virtual void RayModelGetBBoxMax(u32 modelInd, Vec3D *pOut) = 0;
	virtual f32  RayModelGetRadius(u32 modelInd) = 0;
	virtual u8   ModelCollision(u32 modelInd, const Vec3D *mins, const Vec3D *maxs, const Vec3D &pt1, const Vec3D &pt2, gfxTrace *pTrace, f32 testT) = 0;
	virtual void ModelTranslate(u32 modelInd, const Vec3D & trans, u8 bAdd) = 0;
	virtual void ModelRotate(u32 modelInd, const Vec3D & rot, u8 bAdd) = 0;
	virtual void ModelRotateAxis(u32 modelInd, const Vec3D & axis, f32 rot) = 0;
	virtual void ModelEnable(u32 modelInd, u8 bEnable) = 0;
	virtual void ModelGetTranslate(s32 modelInd, Vec3D *ptOut) = 0;
	virtual void ModelGetRotate(s32 modelInd, Vec3D *rotOut) = 0;
	virtual u32  ModelRender(s32 modelInd, const Vec3D & camPos, u8 bShadow) = 0;
	virtual void Collision(const Vec3D *mins, const Vec3D *maxs, const Vec3D &pt1, const Vec3D &pt2, gfxTrace *pTrace, f32 testT) = 0;
	virtual u32  Render() = 0;
	virtual u32  UpdateEnt(gfxEnt *ent, const Vec3D & mins, const Vec3D & maxs) = 0;
	virtual void EntSetAreaPortalState(gfxEnt *ent, const Vec3D & mins, const Vec3D & maxs, u8 open) = 0;
	virtual u32  PrepareFrame() = 0;
	virtual u8   CullEnt(gfxEnt *ent) = 0;
	virtual void ClearDecals() = 0;
	virtual void SpawnDecal(const Vec3D & origin, const Vec3D & dir, f32 orient, f32 radius, const Vec4D & clr, f32 die, f32 fadetime, u8 fadealpha, hTXT txt, hEFFECT fx) = 0;

} *hQBSP;

typedef struct _gfxBkFX		*hBKFX;	//handle to background FX

#endif