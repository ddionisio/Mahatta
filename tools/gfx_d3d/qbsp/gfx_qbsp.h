#ifndef _gfx_qbsp_h
#define _gfx_qbsp_h

#include "GFX\gfx_types.h"
#include "GFX\gfx_qbsp_flags.h"

//THIS IS FOR INTERNAL GFX USE ONLY!

//THIS IS THE QUAKE 3 BSP

#define BSP_MAX_TXT_NAME		64

#define	MAX_MAP_AREAS			0x100
#define	MAX_MAP_PORTALS			0x20000
#define	MAX_MAP_LEAFS			0x20000

#define	MAX_EDICTS				1024	// must change protocol to increase more

#define MAX_NULLROW				16384

#define MAX_CM_AREAS			(MAX_MAP_AREAS)
#define MAX_CM_PORTALS			(MAX_EDICTS)
#define MAX_CM_AREAPORTALS		(MAX_CM_PORTALS << 1)

#define MAX_DECALS				256

//#define SHOW_INVISIBLE

//flags for the QBSP
#define QBSP_FLAG_VTXLIGHTING	1

//flags for the level model
#define QBSP_MODEL_FLAG_DISABLE	1

// Lump types
typedef enum
{
    eLumpEntities,				// objects and triggers...
    eLumpTextures,				// texture info (file, etc)
    eLumpPlanes,			    // planes for each leaf
    eLumpNodes,					// the BSP nodes
    eLumpLeafs,					// the leafs of BSP nodes
    eLumpLeafFaces,				// leaf index to faces
    eLumpLeafBrushes,			// leaf index to brushes
    eLumpModels,				// stuff about models (non-static objects)
    eLumpBrushes,				// collision stuff
    eLumpBrushSides,			// more collision stuff
    eLumpVertices,				// these are all the vertices
    eLumpMeshVerts,				// vertex indices
    eLumpShaders,				// shader fx (not used for now...)
    eLumpFaces,					// all the faces
    eLumpLightmaps,				// lightmaps for shadows and stuff
    eLumpLightVolumes,			// more light stuff (fog effect, etc)
    eLumpVisData,				// PVS and cluster for culling faces
    eNumLumps
} eLumps;

//face types
typedef enum
{
	eFacePolygon=1,		//regular old polygon
	eFacePatch,			//parametrized curve surface
	eFaceMesh,			//static object
	eFaceBillboard		//billboard
} eFaceType;

// This is our BSP header structure
typedef struct _BSPHeader
{
    s8 ID[4];				// This should always be 'IBSP'
    s32 ver;				// This should be 0x2e for Quake 3 files
} BSPHeader;

//This is our BSP brush side structure
typedef struct _BSPBrushSide
{
    s32 plane;              // The plane index
    s32 textureID;          // The texture index
} BSPBrushSide;

//This is our BSP brush structure
typedef struct _BSPBrush
{
    s32 brushSide;           // The starting brush side for the brush 
    s32 numOfBrushSides;     // Number of brush sides for the brush
    s32 textureID;           // The texture index for the brush
} BSPBrush;

//This is the BSP model structure
typedef struct _BSPModel 
{
    Vec3D min;			    // The min position for the bounding box
    Vec3D max;		        // The max position for the bounding box. 
    s32 faceIndex;          // The first face index in the model 
    s32 numOfFaces;         // The number of faces in the model 
    s32 brushIndex;         // The first brush index in the model 
    s32 numOfBrushes;       // The number brushes for the model
} BSPModel; 

// This is our BSP lump structure
typedef struct _BSPLump
{
	s32 offset;					// The offset into the file for the start of this lump
	s32 length;					// The length in bytes for this lump
} BSPLump;

// This stores a node in the BSP tree
typedef struct _BSPNode
{
    s32 plane;					// The index into the planes array 
    s32 front;					// The child index for the front node 
    s32 back;					// The child index for the back node 
    s32 min[3];					// The bounding box min position. (int)
    s32 max[3];					// The bounding box max position. (int)
} BSPNode;

// This stores a leaf (end node) in the BSP tree
typedef struct _BSPLeaf
{
    s32 cluster;				// The visibility cluster 
    s32 area;					// The area portal 
    s32 min[3];					// The bounding box min position. (int)
    s32 max[3];					// The bounding box max position. (int)
    s32 leafface;				// The first index into the face array 
    s32 numOfLeafFaces;			// The number of faces for this leaf 
    s32 leafBrush;				// The first index for into the brushes 
    s32 numOfLeafBrushes;		// The number of brushes for this leaf 
} BSPLeaf;

// This stores a splitter plane in the BSP tree
typedef struct _BSPPlane
{
    f32 a,b,c,d;
} BSPPlane;

// This stores the cluster data for the PVS's
typedef struct _BSPVisData
{
	s32 numOfClusters;			// The number of clusters
	s32 bytesPerCluster;		// The amount of bytes (8 bits) in the cluster's bitset
	u8 *pBitsets;				// The array of bytes that holds the cluster bitsets				
} BSPVisData;

// This is our BSP lightmap structure which stores the 128x128 RGB values
typedef struct _BSPLightmap
{
    u8 imageBits[128][128][3];   // The RGB data in a 128x128 image
} BSPLightmap;


// BSP vertex definition
typedef struct _BSPVertex
{
    Vec3D pos;					//x,y,z
    Vec2D txtCoord;				//u,v
    Vec2D lightmapCoord;		//u,v
    Vec3D vNormal;				//x,y,z
    u8    color[4];				//r,g,b,a
} BSPVertex;

// This is our BSP face structure
typedef struct _BSPFace
{
    s32 textureID;				// The index into the texture array 
    s32 effect;					// The index for the effects (or -1 = n/a) 
    s32 type;					// 1=polygon, 2=patch, 3=mesh, 4=billboard 
    s32 startVertIndex;			// The starting index into this face's first vertex 
    s32 numOfVerts;				// The number of vertices for this face 
    s32 meshVertIndex;			// The index into the first meshvertex 
    s32 numMeshVerts;			// The number of mesh vertices 
    
	s32 lightmapID;				// The texture index for the lightmap 
    s32 lMapCorner[2];			// The face's lightmap corner in the image 
    s32 lMapSize[2];			// The size of the lightmap section 
    
	Vec3D lMapPos;				// The 3D origin of lightmap. 

    Vec3D lMapVecs[2];			// The 3D space for s and t unit vectors. 
    Vec3D vNormal;				// The face normal. 

    s32 size[2];				// The bezier patch dimensions. 
} BSPFace;


// This is our BSP texture structure
typedef struct _BSPTexture
{
    s8 strName[BSP_MAX_TXT_NAME];			// The name of the texture w/o the extension 
    u32 flags;					// The surface flags (unknown) 
    s32 contents;				// The content flags (unknown)
} BSPTexture;

// each area has a list of portals that lead into other areas
// when portals are closed, other areas may not be visible or
// hearable even if the vis info says that it should be
typedef struct
{
	s32		portalnum;
	s32		otherarea;
} careaportal_t;

typedef struct
{
	s32		numareaportals;
	s32		areaportals[MAX_CM_AREAPORTALS];
	s32		floodnum;			// if two areas have equal floodnums, they are connected
	s32		floodvalid;
} carea_t;

//////////////////////////////////////////////////////////////////////
//OUR vertex definition for the map
#define GFXVERTEXQBSPFLAG (D3DFVF_XYZ | D3DFVF_NORMAL | /*D3DFVF_DIFFUSE |*/ D3DFVF_TEX2)

typedef struct _gfxVtxQBSP {
    Vec3D pt;		//vertex
	Vec3D n;		//normal
	//u32   color;    //the color!
	Vec2D sTex;	    //surface texture coord
	Vec2D lTex;		//lightmap texture coord
	//Vec2D dlTex;	//dynamic lightmap texture coord
} gfxVtxQBSP;

//used for patch surfaces
typedef struct _gfxPatch {
	f32                 level;				//Tesselation level

	s32					patchW,patchH;

	GFXVTXBUFF		   *vtx;
	u32					numVtx;

	GFXINDBUFF		   *ind;				//index buffer used
	u32					numInd;

	u32					numTri;				//number of triangles for each row

	u16				   *rowIndOfs;			//index offset for each row
	u32				   numRow;				//number of rows

	Vec3D			   min,max; 

} gfxPatch;

// OUR BSP face structure
typedef struct _gfxBSPFace
{
    s32 textureID;				// The index into the texture array 
    s32 effect;					// The index for the effects (or -1 = n/a) 
    s32 type;					// 1=polygon, 2=patch, 3=mesh, 4=billboard 
    s32 startVertIndex;			// The starting index into this face's first vertex 
    s32 numOfVerts;				// The number of vertices for this face 
    s32 meshVertIndex;			// The index into the first meshvertex 
    s32 numMeshVerts;			// The number of mesh vertices 
    s32 lightmapID;				// The texture index for the lightmap 
    s32 lMapCorner[2];			// The face's lightmap corner in the image 
    s32 lMapSize[2];			// The size of the lightmap section 
    Vec3D lMapPos;				// The 3D origin of lightmap. 
    
	Vec3D mins,maxs;			// The 3D space for s and t unit vectors. (patch bounds...lMapVecs)
    
	Vec3D vNormal;				// The face normal. 
    
	s32 size[2];				// The bezier patch dimensions. 

	//our own stuff here:

	s32	fragmentframe;			//used to determine if this face has already been processed
	s32 firstbrush;				//the first brush (used for patches)
	s32 numbrushes;				//number of brushes (used for patches)

	gfxPatch	*patch;			//used for patches
} gfxBSPFace;

//OUR texture definition for the map
typedef struct _gfxTxtQBSP {
	hEFFECT fx;					// the effect to use
	hTXT	texture;			// texture buffer (if fx == NULL)

	u32		flags;				// The surface flags (unknown) 
    u32		contents;			// The content flags (unknown)
} gfxTxtQBSP;

//OUR node definition
typedef struct _gfxBSPNode
{
    s32 plane;					// The index into the planes array
	s32 child[2];				// the front (0) and back (1) child
    Vec3D min;					// The bounding box min position.
    Vec3D max;					// The bounding box max position.
} gfxBSPNode;

//OUR leaf (end node) definition
typedef struct _gfxBSPLeaf
{
    s32 cluster;				// The visibility cluster 
    s32 area;					// The area portal 
    Vec3D min;					// The bounding box min position.
    Vec3D max;					// The bounding box max position.
	Vec3D loc;					// The center location of this leaf
    s32 leafface;				// The first index into the face array 
    s32 numOfLeafFaces;			// The number of faces for this leaf 
    s32 leafBrush;				// The first index for into the brushes 
    s32 numOfLeafBrushes;		// The number of brushes for this leaf 

	//our stuff:

	u32	  contents;				// stuff
} gfxBSPLeaf;

//OUR BSP model structure
typedef struct _ConvexPolyhedron {
	Plane	 *planes;		// The group of planes used by this polyhedron. (numPlane)
							// This is the model space planes

	Plane	 *tPlanes;		// This is similar to planes, but transformed

	s32		 *txtID;		// The texture IDs (numPlane)
	s32		  numPlane;
} ConvexPolyhedron;

typedef struct _gfxBSPModel 
{
    Vec3D min;			// The min position for the bounding box
    Vec3D max;			// The max position for the bounding box. 
	f32 radius;			// The radius of the model

    s32 faceIndex;          // The first face index in the model 
    s32 numOfFaces;         // The number of faces in the model 
    s32 brushIndex;         // The first brush index in the model 
    s32 numOfBrushes;       // The number brushes for the model

	ConvexPolyhedron *brushes; //The brushes this model uses (numOfBrushes)

	Vec3D        wrldTrans;	 //world translation
	Vec3D		 wrldRot;	 //world rotation (RADIANS: along x,y,z)

	Matrix		 wrldMtx[FXMTX_MAX];    	 //world matrix

	hLIGHT		 lights[MAXLIGHT]; //light list for getting nearest lights

	u32		 status;			 //model status
} gfxBSPModel;

/////////////////////
//Decal
typedef struct _gfxBSPDecal
{
	struct _gfxBSPDecal *prev, *next;

	f32			time;

	f32			die;				// stop lighting after this time
	f32			fadetime;
	f32			fadefreq;
	u8			fadealpha;
	
	Vec4D		color_cur;
	Vec4D		color;

	hTXT		txt;			//if fx == NULL, use this
	hEFFECT		fx;

	GFXVTXBUFF	*vtx;
	u32			 vtxNum;
} gfxBSPDecal;
//////////////////////////////////////////////////////////////////////

typedef class IgfxQBSP *hIQBSP;

//The GFX QBSP map structure
class IgfxQBSP : public gfxQBSP {
public:
	IgfxQBSP(const tCHAR *filename, const tCHAR *txtPath, f32 curveDetail, ENTITYPARSEFUNC parseFunc, void *userDat);
	~IgfxQBSP();

	void AddRef();
	s32  Release();

	s32  FindLeaf(const Vec3D & pt);
	hEFFECT GetEffect(u32 txtInd);
	hTXT GetTexture(u32 txtInd);
	u32  GetTextureFlags(u32 txtInd);
	u32  GetTextureContents(u32 txtInd);
	void WritePortalState(hFILE f);
	void ReadPortalState(hFILE f);
	u8   InPVS(const Vec3D & p1, const Vec3D & p2);
	u8   InPHS(const Vec3D & p1, const Vec3D & p2);
	u32	 GetNumModels();
	void RayModelGetBBoxMin(u32 modelInd, Vec3D *pOut);
	void RayModelGetBBoxMax(u32 modelInd, Vec3D *pOut);
	f32  RayModelGetRadius(u32 modelInd);
	u8   ModelCollision(u32 modelInd, const Vec3D *mins, const Vec3D *maxs, const Vec3D &pt1, const Vec3D &pt2, gfxTrace *pTrace, f32 testT);
	void ModelTranslate(u32 modelInd, const Vec3D & trans, u8 bAdd);
	void ModelRotate(u32 modelInd, const Vec3D & rot, u8 bAdd);
	void ModelRotateAxis(u32 modelInd, const Vec3D & axis, f32 rot);
	void ModelEnable(u32 modelInd, u8 bEnable);
	void ModelGetTranslate(s32 modelInd, Vec3D *ptOut);
	void ModelGetRotate(s32 modelInd, Vec3D *rotOut);
	u32  ModelRender(s32 modelInd, const Vec3D & camPos, u8 bShadow);
	void Collision(const Vec3D *mins, const Vec3D *maxs, const Vec3D &pt1, const Vec3D &pt2, gfxTrace *pTrace, f32 testT);
	u32  Render();
	u32  UpdateEnt(gfxEnt *ent, const Vec3D & mins, const Vec3D & maxs);
	void EntSetAreaPortalState(gfxEnt *ent, const Vec3D & mins, const Vec3D & maxs, u8 open);
	u32  PrepareFrame();
	u8   CullEnt(gfxEnt *ent);
	void ClearDecals();
	void SpawnDecal(const Vec3D & origin, const Vec3D & dir, f32 orient, f32 radius, const Vec4D & clr, f32 die, f32 fadetime, u8 fadealpha, hTXT txt, hEFFECT fx);

	////////////////////////////////////
	// Internal GFX Interface

	inline s32 ClusterIsVisible(s32 current, s32 test)
	{
		// Make sure we have valid memory and that the current cluster is > 0.
		// If we don't have any memory or a negative cluster, return a visibility (1).
		if(!m_clusters.pBitsets || current < 0) return 1;

		// Use binary math to get the 8 bit visibility set for the current cluster
		BYTE visSet = m_clusters.pBitsets[(current*m_clusters.bytesPerCluster) + (test / 8)];

		// Now that we have our vector (bitset), do some bit shifting to find if
		// the "test" cluster is visible from the "current" cluster, according to the bitset.

		// Return the result ( either 1 (visible) or 0 (not visible) )
		s32 r = visSet & (1 << ((test) & 7));
		return r;
	}

	u32 CreateDfltFX();

	//
	// Useful BSP data for culling and such
	//

	u32 GetNumLeaves();
	s32	ClusterSize();
	s32	NumClusters();
	s32	LeafContents(s32 leafnum);
	s32	LeafCluster(s32 leafnum);
	s32	LeafArea(s32 leafnum);
	s32	BoxLeafnums(const Vec3D & mins, const Vec3D & maxs, s32 *list, s32 listsize, s32 *topnode);

	/*
	===============================================================================

	PVS / PHS

	===============================================================================
	*/

	u8	*ClusterPVS (s32 cluster);

	u8	*ClusterPHS (s32 cluster);

	/*
	=============
	CM_HeadnodeVisible

	Returns true if any leaf under headnode has a cluster that
	is potentially visible
	=============
	*/
	u8 HeadnodeVisible(s32 nodenum, u8 *visbits);

	/*
	===============================================================================

	AREAPORTALS

	===============================================================================
	*/

	void AddAreaPortal(s32 portalnum, s32 area, s32 otherarea );

	/*
	====================
	FloodAreaConnections


	====================
	*/
	void	FloodAreaConnections ();

	void	SetAreaPortalState (s32 portalnum, u8 open);

	u8	AreasConnected (s32 area1, s32 area2);

	/*
	=================
	CM_WriteAreaBits

	Writes a length byte followed by a bit vector of all the areas
	that area in the same flood as the area parameter

	This is used by the client refreshes to cull visibility
	=================
	*/
	s32 WriteAreaBits (u8 *buffer, s32 area);

	/*
	=================
	CM_MergeAreaBits
	=================
	*/
	void MergeAreaBits (u8 *buffer, s32 area);

	//This is used for getting a clipped quad from the map
	//for decals
	s32 GetClippedFragments(const Vec3D & origin, f32 radius, const Vec3D axis[3], 
								s32 maxfverts, Vec3D *fverts, s32 maxfragments, 
								gfxBSPFragment *fragments);

	/////////////////////////////////////
	// Purpose:	this will render the
	//			decals (clearing out
	//			those that expire as well)
	// Output:	decals added to scene list
	// Return:	none
	/////////////////////////////////////
	void RenderDecals();

	inline hTXT FaceGetTexture(s32 ind) { if(m_faces[ind].textureID >= 0) { return m_textures[m_faces[ind].textureID].texture; } return 0; }

	//lightmap texture
	inline hTXT FaceGetLMTexture(s32 ind) { if(m_faces[ind].lightmapID >= 0) { return m_lightmaps[m_faces[ind].lightmapID]; } return 0; }

	inline const gfxBSPModel & GetModel(s32 ind) { return m_models[ind]; }

	inline const gfxBSPFace & GetFace(s32 ind) { return m_faces[ind]; }

	inline GFXVTXBUFF *GetVtxBuff() { return m_vtx; }
	inline GFXINDBUFF *GetIndBuff() { return m_meshIndBuff; }

	//the shadow vertex buffer
	inline Vec3D * GetSHVtxBuff() { return m_shVtxBuff;	}

	//the shadow index buffer
	inline u16 * GetSHIndBuff() { return m_shIndBuff; }

private:
	void _FloodArea_r(carea_t *area, int floodnum);

	void _FatPVS(const Vec3D & org);
	void _FatPHS(s32 cluster);

	void _MergePVS(const Vec3D & org);
	void _MergePHS(s32 cluster);

	u8 _CullEntity(gfxEnt *ent, u8 *bitvector);

	void _CollBrush(s32 brushIndex);
	void _CollRecursive(s32 node, f32 p1f, f32 p2f, const Vec3D &p1, const Vec3D &p2);

	gfxBSPDecal * _DecalAlloc();
	void _DecalFree(gfxBSPDecal *dl);

	///////////////////// Loading Begin

	u32 _LoadVert(hFILE fp, BSPLump *lumps);

	s32 _AddPlane(const Plane & plane);
	void _AddBrushside(BSPBrush *brush, s32 planeInd, s32 txtID);
	
	//returns num sides (0 if failed)
	s32 _CreateBrushFromPoints(const Vec3D *verts, s32 txtID);

	void _FaceCreatePatch(gfxBSPFace *face, s32 numverts, const gfxVtxQBSP *verts, s32 *patch_cp);

	u32 _LoadFaces(hFILE fp, f32 curveDetail, BSPLump *lumps);
	u32 _LoadTxts(hFILE fp, BSPLump *lumps, const tCHAR *mainPath);
	u32 _LoadPlanes(hFILE fp, BSPLump *lumps);
	u32 _LoadMdls(hFILE fp, BSPLump *lumps);
	u32 _LoadLightmaps(hFILE fp, const tCHAR *filename, BSPLump *lumps);
	u32 _LoadVertInd(hFILE fp, BSPLump *lumps);
	u32 _LoadEntities(hFILE fp, BSPLump *lumps, ENTITYPARSEFUNC parseFunc, void *userDat);
	u32 _LoadLeaves(hFILE fp, BSPLump *lumps);

	u32 _CalcPHS();

	///////////////////// Loading End

	void _ModelTransformPlanes(gfxBSPModel *mdl);

	//translate the actual raw vertex data and plane
	void _ModelTranslate(gfxVtxQBSP *pVtx, gfxBSPModel *mdl, const Vec3D & trans);

	void _RenderFace(s32 i, const Matrix *pWrldMtx, FLAGS flag, GFXHANDLE fxTech, 
		const hLIGHT *pLights, f32 lodDistSq);

	void _RenderLeaf(const Vec3D &pos, gfxBSPLeaf *pLeaf, u8 bCheckFrustum, const Matrix *pWrldMtx);

	void _RecursiveRender(s32 node, const Vec3D &pos, u8 bCheckFrustum);

	void _BoxLeafnums_r(s32 nodenum);

	void _RecursiveFragmentNode(s32 node, const Vec3D & origin, f32 radius, const Vec3D & normal);

	GFXVTXBUFF			   *m_vtx;		//all the map vertices
	u32						m_numVtx;	//number of vertices

	gfxBSPFace			   *m_faces;	//all the faces (numFaces)
	u32					    m_numFaces;	//number of faces

	u32					   *m_brushVis;	   //has brush been processed yet?
	u32						m_numBrushVis; //number of brush vis

	gfxTxtQBSP			   *m_textures;		//all the textures (numTxt)
	u32						m_numTxt;		//number of textures

	hTXT				   *m_lightmaps;	 //special hTXT containing lightmap (128x128) (numLightMap)
	u32						m_numLightMap;	 //number of light maps

	gfxBSPNode			   *m_nodes;		 //the nodes (numNodes)
	u32						m_numNodes;

	gfxBSPLeaf			   *m_leaves;		 //edge nodes (numLeaves)

	u32						m_numLeaves;	 //number of leaves

	Plane				   *m_planes;		 //plane equations (numPlanes)
	u32						m_numPlanes;	 //number of planes

	s32					   *m_leafFaces;	 //leaf faces... (numLeafFaces)
	u32						m_numLeafFaces;  //number of leaf faces

	//unsigned short		   *meshVtxInd;	 //mesh vertex indices (numMeshVtxInd)
	GFXINDBUFF			   *m_meshIndBuff;   //mesh vertex indices (numMeshVtxInd)
	u32						m_numMeshVtxInd; //number of mesh vertex indices

	gfxBSPModel			   *m_models;		 //all non-static models (numModels)
	u32						m_numModels;	 //number of models

	BSPBrush			   *m_brushes;		 //all the brushes for collision (numBrushes)
	u32						m_numBrushes;	 //number of brushes

	s32					   *m_leafBrushes;   //The index into the brush array (numLeafBrushes)
	u32						m_numLeafBrushes;//number of leaf brushes

	BSPBrushSide		   *m_brushSides;	 //Brush sides refered by brushes (numBrushSides)
	u32						m_numBrushSides; //number of brush sides

	////
	BSPVisData				m_clusters;		//the PVS clusters
	BSPVisData				m_clustersPHS;  //the PHS clusters

	s32						m_numareaportals;							//default to 1
	careaportal_t			m_map_areaportals[MAX_CM_AREAPORTALS];

	s32						m_numareas;								//default to 1
	carea_t					m_map_areas[MAX_CM_AREAS];

	s32						m_floodvalid;

	u8						m_portalopen[MAX_CM_PORTALS];				//boolean to determine which portals are open

	////

	Matrix				    m_wrldMtx;	 //the world matrix! (DO NOT CHANGE!!! SHOULD ALWAYS BE IDENTITY!)

	hEFFECT					m_dfltFX;		 //the default effect for faces with no FXs
	GFXHANDLE				m_mdlFXTech;	 //the FX default technique for models in the level

	gfxBSPDecal				m_decals[MAX_DECALS];
	gfxBSPDecal				m_decals_headnode, *m_decals_free;

	Vec3D				   *m_shVtxBuff;	 //the shadow vertex buffer
	u32						m_shVtxNum;		 //number of shadow vertex
	u16					   *m_shIndBuff;	 //the shadow index buffer
	u16					    m_shIndNum;		 //number of shadow index

	u32						m_status;		 //some stuff: vertex lighting/lightmap

	s32						m_ref;
};

#endif