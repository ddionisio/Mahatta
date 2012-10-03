#include "..//gfx_i.h"

#include "GFX\gfx_math.h"

#include "gfx_qbsp.h"
#include "gfx_qbsp_i.h"

#define QBSP_VTXBUFF_USAGE		GFXUSAGE_WRITEONLY//D3DUSAGE_DYNAMIC//D3DUSAGE_SOFTWAREPROCESSING//D3DUSAGE_DYNAMIC//
#define QBSP_VTXBUFF_POOL		GFXPOOL_MANAGED//D3DPOOL_DEFAULT//D3DPOOL_DEFAULT//

#define QBSP_INDBUFF_USAGE		GFXUSAGE_WRITEONLY
#define QBSP_INDBUFF_POOL		GFXPOOL_MANAGED

#define GAMMA_DEFAULT_LIGHTMAP 3
#define GAMMA_DEFAULT_VTX	   4

#define LIGHTMAP_USAGE		   0

#define QBSP_CURVEDETAIL_DEFAULT	6

gfxVtxQBSP		*g_tempVtx=0;	//this is allocated during load
								//and is destroyed after load
								//THIS IS ONLY USED DURING LOAD

u8				 g_nullrow[MAX_NULLROW];	 //null bytes for vis clusters

static inline void _QBSPGammaRGB(u8 & red, u8 & green, u8 & blue, f32 factor)
{
	f32 scale = 1.0f, temp = 0.0f;
	f32 r = 0, g = 0, b = 0;

	r = (f32)red;
	g = (f32)green;
	b = (f32)blue;

	// Multiply the factor by the RGB values, while keeping it to a 255 ratio
	r = r * factor / 255.0f;
	g = g * factor / 255.0f;
	b = b * factor / 255.0f;
	
	// Check if the the values went past the highest value
	if(r > 1.0f && (temp = (1.0f/r)) < scale) scale=temp;
	if(g > 1.0f && (temp = (1.0f/g)) < scale) scale=temp;
	if(b > 1.0f && (temp = (1.0f/b)) < scale) scale=temp;

	// Get the scale for this pixel and multiply it by our pixel values
	scale*=255.0f;		
	r*=scale;	g*=scale;	b*=scale;

	// Assign the new gamma'nized RGB values to our image
	red = (u8)r;
	green = (u8)g;
	blue = (u8)b;
}

u32 IgfxQBSP::_LoadVert(hFILE fp, BSPLump *lumps)
{
	if(g_tempVtx)
	{ MemFree(g_tempVtx); g_tempVtx = 0; }

	BSPVertex vert;

	// Seek to the position in the file that stores the vertex information
	FSSeek(fp, lumps[eLumpVertices].offset, FS_SEEK_SET);

	//now allocate our vertices
	m_vtx = GFXCreateVtxBuff(sizeof(gfxVtxQBSP), m_numVtx, 
		QBSP_VTXBUFF_USAGE, GFXVERTEXQBSPFLAG, QBSP_VTXBUFF_POOL);

	if(!m_vtx)
	{ LogMsg(LOG_FILE, L"_QBSPLoadVert", L"Unable to create vertex buffer"); return FALSE; }

	//allocate temporary vertices
	g_tempVtx = (gfxVtxQBSP *)MemAlloc(sizeof(gfxVtxQBSP)*m_numVtx);

	gfxVtxQBSP *pVtx = g_tempVtx;//=m_vtx;

	//now fill our stuff
	//we will copy the temporary buffer to the actual buffer later
	for(u32 i = 0; i < m_numVtx; i++)
	{
		FSRead(&vert, 1, sizeof(BSPVertex), fp);

		pVtx[i].pt.x = vert.pos.x;

		// Swap the y and z values, and negate the new z so Y is up.
		pVtx[i].pt.y = vert.pos.z;
		pVtx[i].pt.z = -vert.pos.y;

		// Negate the V texture coordinate because it is upside down otherwise...
		pVtx[i].sTex = vert.txtCoord;
		
		//put lightmap
		pVtx[i].lTex = vert.lightmapCoord;
		
		pVtx[i].n.x = vert.vNormal.x;
		pVtx[i].n.y = vert.vNormal.z;
		pVtx[i].n.z = -vert.vNormal.y;

		//_QBSPGammaRGB(vert.color[0],vert.color[1],vert.color[2], GAMMA_DEFAULT_VTX);

		//pVtx[i].color = GFXCOLOR_RGBA(vert.color[0],vert.color[1],vert.color[2],vert.color[3]);

		//er...
		//pVtx[i].dlTex = pVtx[i].lTex;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////

#define MAX_CM_PATCH_VERTS 4096
#define CM_SUBDIV_LEVEL	   15

#define	PLANE_NORMAL_EPSILON	0.00001f
#define	PLANE_DIST_EPSILON		0.01f

static inline u8 _ComparePlanes(const Plane & p, const Vec3D & normal, f32 dist)
{
	if (fabsf(p.a - normal.x) < PLANE_NORMAL_EPSILON
		&& fabsf(p.b - normal.y) < PLANE_NORMAL_EPSILON
		&& fabsf(p.c - normal.z) < PLANE_NORMAL_EPSILON
		&& fabsf(p.d - dist) < PLANE_DIST_EPSILON )
		return TRUE;

	return FALSE;
}

static inline void _SnapVector(Vec3D * normal)
{
	if(fabsf(normal->x - 1.0f) < PLANE_NORMAL_EPSILON) 
	{ memset(normal, 0, sizeof(Vec3D)); normal->x = 1.0f; return; }
	if(fabsf(normal->x - -1.0f) < PLANE_NORMAL_EPSILON)
	{ memset(normal, 0, sizeof(Vec3D)); normal->x = -1.0f; return; }

	if(fabsf(normal->y - 1.0f) < PLANE_NORMAL_EPSILON) 
	{ memset(normal, 0, sizeof(Vec3D)); normal->y = 1.0f; return; }
	if(fabsf(normal->y - -1.0f) < PLANE_NORMAL_EPSILON)
	{ memset(normal, 0, sizeof(Vec3D)); normal->y = -1.0f; return; }

	if(fabsf(normal->z - 1.0f) < PLANE_NORMAL_EPSILON) 
	{ memset(normal, 0, sizeof(Vec3D)); normal->z = 1.0f; return; }
	if(fabsf(normal->z - -1.0f) < PLANE_NORMAL_EPSILON)
	{ memset(normal, 0, sizeof(Vec3D)); normal->z = -1.0f; return; }
}

static inline void _SnapPlane(Vec3D * normal, f32 *dist)
{
	_SnapVector(normal);

	if(fabsf(*dist - Q_rint(*dist)) < PLANE_DIST_EPSILON)
		*dist = (f32)Q_rint(*dist);
}

//returns new plane index (-1 if failed)
s32 IgfxQBSP::_AddPlane(const Plane & plane)
{
	Vec3D n(plane.a, plane.b, plane.c);

	//go through the planes and see if we already have this type of plane
	for(u32 i = 0; i < m_numPlanes; i++)
	{
		if(_ComparePlanes(m_planes[i], n, plane.d))
			return i;
	}

	//re-allocate planes
	m_planes = (Plane *)realloc(m_planes, sizeof(Plane)*(m_numPlanes+1));

	if(!m_planes) { //shit
		LogMsg(LOG_FILE, L"_AddPlane", L"unable to reallocate plane pool");
		return -1; }

	m_planes[m_numPlanes] = plane;
	PlaneCategorize(&m_planes[m_numPlanes]);
	m_numPlanes++;

	return m_numPlanes-1;
}

void IgfxQBSP::_AddBrushside(BSPBrush *brush, s32 planeInd, s32 txtID)
{
	//re-allocate brush sides
	m_brushSides = (BSPBrushSide *)realloc(m_brushSides, sizeof(BSPBrushSide)*(m_numBrushSides+1));

	if(!m_brushSides) { //shit
		LogMsg(LOG_FILE, L"_AddBrushside", L"unable to reallocate brush sides pool");
		return; }

	BSPBrushSide *side = &m_brushSides[m_numBrushSides];
	m_numBrushSides++;

	side->plane = planeInd;
	side->textureID = txtID;
	brush->numOfBrushSides++;
}

//returns num sides (0 if failed)
s32 IgfxQBSP::_CreateBrushFromPoints(const Vec3D *verts, s32 txtID)
{
	Vec3D			vec3_origin(0, 0, 0);
	s32				i, j, k;
	s32				axis, dir;
	Vec3D			mins, maxs;
	BSPBrushSide	*s;
	Vec3D			normal;
	f32				d, dist;
	Plane			mainplane;
	Vec3D			vec, vec2;
	BSPBrush		*brush;

	//allocate a new brush for the qbsp
	m_brushes = (BSPBrush *)realloc(m_brushes, sizeof(BSPBrush)*(m_numBrushes+1));

	if(!m_brushes) { //shit
		LogMsg(LOG_FILE, L"_CreateBrushFromPoints", L"unable to reallocate brush pool");
		return 0; }

	brush = &m_brushes[m_numBrushes];
	m_numBrushes++;

	// set default values for brush
	brush->numOfBrushSides = 0;
	brush->brushSide = (s32)m_numBrushSides;
	brush->textureID = txtID;

	// calculate plane for this triangle
	PlaneFromPoints(&verts[0], &verts[1], &verts[2], &mainplane);
	mainplane.d *= -1.0f; //consistency for all quake 3 map planes >:(

	if(_ComparePlanes(mainplane, vec3_origin, mainplane.d))
		return 0;

	_SnapPlane((Vec3D*)&mainplane, &mainplane.d);

	// calculate mins & maxs
	mins.x = mins.y = mins.z = 99999;
	maxs.x = maxs.y = maxs.z = -99999;

	for(i = 0; i < 3; i++)
		BoundsAddPoint(verts[i], mins, maxs);

	s32 planeInd;

	// add the axial planes
	for(axis = 0; axis < 3; axis++) 
	{
		for(dir = -1; dir <= 1; dir += 2) 
		{
			memset(&normal, 0, sizeof(Vec3D));
			((f32*)normal)[axis] = (f32)dir;

			if (dir == 1)
				dist = ((f32*)maxs)[axis];
			else
				dist = -((f32*)mins)[axis];

			if(!_ComparePlanes(mainplane, normal, dist))
			{
				planeInd = _AddPlane(Plane(normal.x,normal.y,normal.z,dist));

				if(planeInd >= 0)
					_AddBrushside(brush, planeInd, txtID);
			}
		}
	}

	// add front plane
	planeInd = _AddPlane(mainplane);
	_AddBrushside(brush, planeInd, txtID);

	// add the edge bevels
	for(i = 0; i < 3; i++) 
	{
		j = (i + 1) % 3;
		k = (i + 2) % 3;

		vec = verts[i] - verts[j];

		if(Vec3DNormal(&vec,&vec) < 0.5f)
			continue;

		_SnapVector(&normal);
		for( j = 0; j < 3; j++)
		{
			if (((f32*)vec)[j] == 1 || ((f32*)vec)[j] == -1)
				break;	// axial
		}

		if(j != 3)
			continue;	// only test non-axial edges

		// try the six possible slanted axials from this edge
		for(axis = 0; axis < 3; axis++) 
		{
			for(dir = -1; dir <= 1; dir += 2) 
			{
				// construct a plane
				memset(&vec2, 0, sizeof(Vec3D));
				
				((f32*)vec2)[axis] = (f32)dir;

				Vec3DCross(&vec, &vec2, &normal);
				if(Vec3DNormal(&normal, &normal) < 0.5f)
					continue;

				dist = Vec3DDot(&verts[i], &normal);

				s = &m_brushSides[brush->brushSide];
				for(j = 0; j < brush->numOfBrushSides; j++, s++) 
				{
					// if this plane has allready been used, skip it
					if(_ComparePlanes(m_planes[s->plane], normal, dist))
						break;
				}

				if(j != brush->numOfBrushSides)
					continue;

				// if third point of the triangle is
				// behind this plane, it is a proper edge bevel
				d = Vec3DDot(&verts[k], &normal) - dist;
				if ( d > 0.1 )
					continue;	// point in front: this plane isn't part of the outer hull

				// add this plane
				planeInd = _AddPlane(Plane(normal.x,normal.y, normal.z, dist));
				if(planeInd >= 0)
					_AddBrushside(brush, planeInd, txtID);
			}
		}
	}

	return brush->numOfBrushSides;
}

void IgfxQBSP::_FaceCreatePatch(gfxBSPFace *face, s32 numverts, const gfxVtxQBSP *verts, s32 *patch_cp)
{
    s32 step[2], size[2], flat[2], i, u, v;
	gfxVtxQBSP points[MAX_CM_PATCH_VERTS];
	Vec3D tverts[4], tverts2[4];

// find the degree of subdivision in the u and v directions
	PatchGetFlatness(CM_SUBDIV_LEVEL, verts, patch_cp, flat);

	step[0] = 1 << flat[0];
	step[1] = 1 << flat[1];
	size[0] = (patch_cp[0] >> 1) * step[0] + 1;
	size[1] = (patch_cp[1] >> 1) * step[1] + 1;

	if(size[0]*size[1] > MAX_CM_PATCH_VERTS) 
	{ LogMsg(LOG_FILE, L"_FaceCreatePatch", L"patch has too many vertices"); return; }

// fill in
	PatchEvaluate(verts, patch_cp, step, points);

	face->firstbrush = m_numBrushes;
	face->numbrushes = 0;

	face->mins.x = face->mins.y = face->mins.z = 99999;
	face->maxs.x = face->maxs.y = face->maxs.z = -99999;
		
// create a set of brushes
    for(v = 0; v < size[1]-1; v++) 
	{
		for(u = 0; u < size[0]-1; u++) 
		{
			i = v * size[0] + u;

			tverts[0] = points[i].pt;
			tverts[1] = points[i + size[0]].pt;
			tverts[2] = points[i + 1].pt;
			tverts[3] = points[i + size[0] + 1].pt;

			for(i = 0; i < 4; i++)
				BoundsAddPoint(tverts[i], face->mins, face->maxs);

			// create two brushes
			if(_CreateBrushFromPoints(tverts, face->textureID))
			{ face->numbrushes++; }

			tverts2[0] = tverts[2];
			tverts2[1] = tverts[1];
			tverts2[2] = tverts[3];

			if(_CreateBrushFromPoints(tverts2, face->textureID))
			{ face->numbrushes++; }
		}
    }

	face->mins.x -= 1; face->maxs.x += 1;
	face->mins.y -= 1; face->maxs.y += 1;
	face->mins.z -= 1; face->maxs.z += 1;
}

u32 IgfxQBSP::_LoadFaces(hFILE fp, f32 curveDetail, BSPLump *lumps)
{
	f32 cDetail = curveDetail > 0 ? curveDetail : QBSP_CURVEDETAIL_DEFAULT;

	f32 temp;
	BSPFace face;

	// Seek to the position in the file that stores the face information
	FSSeek(fp, lumps[eLumpFaces].offset, FS_SEEK_SET);

	// Read in all the face information
	for(u32 i = 0; i < m_numFaces; i++)
	{
		FSRead(&face, 1, sizeof(BSPFace), fp);

		memcpy(&m_faces[i], &face, sizeof(BSPFace));

		temp = -m_faces[i].vNormal.y;
		m_faces[i].vNormal.y = m_faces[i].vNormal.z;
		m_faces[i].vNormal.z = temp;

		m_faces[i].fragmentframe = 0;
		m_faces[i].numbrushes = 0;

		//loading patch?
		if(m_faces[i].type == eFacePatch)
		{
			s32 txtID = m_faces[i].textureID;
			if(txtID < 0 || txtID >= m_numTxt) continue;

			if(m_faces[i].size[0] <= 0 || m_faces[i].size[1] <= 0 ) continue;

			if(m_faces[i].numOfVerts <= 0 || m_faces[i].startVertIndex < 0 || (u32)m_faces[i].startVertIndex >= m_numVtx)
				continue;

			//create the render patch
			m_faces[i].patch = QBSPPatchCreate(g_tempVtx, m_faces[i].startVertIndex, m_faces[i].size, cDetail);

			if(!m_textures[txtID].contents || m_textures[txtID].contents & SURF_NONSOLID)
				continue;

			//create the brushes
			//FIXME!: messed up!
			_FaceCreatePatch(&m_faces[i], m_faces[i].numOfVerts, &g_tempVtx[m_faces[i].startVertIndex], m_faces[i].size);
		}
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////

static inline void _QBSPTxtGetPath(const tCHAR *dir, const tCHAR *name, const tCHAR *ext, tCHAR *out)
{
	if(dir)
	{
		wcscpy(out, dir);
		wcscat(out, L"\\");
		wcscat(out, name);
	}
	else
		wcscpy(out, name);

	wcscat(out, ext);
}
//.INI/.PNG/.JPEG/.TGA
#define QBSP_TXT_EXT		5
tCHAR *g_qbspTxtExt[QBSP_TXT_EXT] = {L".ini", L".png", L".jpg", L".tga", L".bmp"};

static u8 _QBSPValidateTexture(const tCHAR *mainpath, const tCHAR *txtName, tCHAR *strOut)
{
	hFILE fp;

	for(u8 i = 0; i < QBSP_TXT_EXT; i++)
	{
		_QBSPTxtGetPath(mainpath, txtName, g_qbspTxtExt[i], strOut);

		fp =FSOpen(strOut, L"rt");

		//found one
		if(fp)
		{ FSClose(fp); return TRUE; }
	}

	return FALSE;
}

#define STR_INVISIBLE			(L"textures/common/invisible")

u32 IgfxQBSP::_LoadTxts(hFILE fp, BSPLump *lumps, const tCHAR *mainPath)
{
	BSPTexture bspTxt;

	tCHAR buff[MAXCHARBUFF], txtName[BSP_MAX_TXT_NAME];

	// Seek to the position in the file that stores the texture information
	FSSeek(fp, lumps[eLumpTextures].offset, FS_SEEK_SET);

	hFILE fpTxt; //use for checking file extension

	//allocate textures
	m_textures = (gfxTxtQBSP*)MemAlloc(sizeof(gfxTxtQBSP)*m_numTxt);
	if(!m_textures)
	{ LogMsg(LOG_FILE, L"_QBSPLoadTxts", L"Unable to allocate textures"); return FALSE; }

	//go through and load up the textures
	for(u32 i = 0; i < m_numTxt; i++)
	{
		// Read in the texture information
		FSRead(&bspTxt, 1, sizeof(BSPTexture), fp);

		mbstowcs(txtName, (const char *)bspTxt.strName, BSP_MAX_TXT_NAME);

		m_textures[i].flags = bspTxt.flags;
		m_textures[i].contents = bspTxt.contents;

		///////////////////////////////////////////////
		//don't bother loading non-renderable textures
		if(TESTFLAGS(m_textures[i].flags, SURF_SKY))
			m_textures[i].flags |= SURF_NODRAW;

		if(TESTFLAGS(m_textures[i].flags, SURF_NODRAW))
			continue;

		////////////////////////////////////////
		//try .fx
		_QBSPTxtGetPath(mainPath, txtName, L".fx", buff);

		fpTxt = FSOpen(buff, L"rb");

		if(fpTxt)
		{
			FSClose(fpTxt);

			m_textures[i].fx = EffectLoad(buff);
		}
		else
		{
			////////////////////////////////////////
			// use the default FX
			m_textures[i].fx = m_dfltFX;
			m_textures[i].fx->AddRef();
		}

		////////////////////////////////////////
		//try .INI/.PNG/.JPEG/.TGA
		if(m_textures[i].fx && TESTFLAGS(((hIEFFECT)m_textures[i].fx)->GetFlag(), EFFECT_BASETEX0))
		{
			if(!_QBSPValidateTexture(mainPath, txtName, buff))
			{
				//must be special...to be continued!

				m_textures[i].texture=0;
				//TextureLoad("textures//winter_wood.jpg", FALSE, 0);
				SAFE_RELEASE(m_textures[i].fx);
				m_textures[i].fx=0;
				SETFLAG(m_textures[i].flags, SURF_NODRAW);
				continue;
			}

			////////////////////////////////////////
			//load the texture
			m_textures[i].texture = TextureLoad(buff, FALSE, 0);

			if(!m_textures[i].texture)
				SETFLAG(m_textures[i].flags, SURF_NODRAW);
		}
	}

	return TRUE;
}

u32 IgfxQBSP::_LoadPlanes(hFILE fp, BSPLump *lumps)
{
	//allocate the planes
	m_planes = (Plane*)MemAlloc(sizeof(Plane)*m_numPlanes);
	if(!m_planes)
	{ LogMsg(LOG_FILE, L"_QBSPLoadPlanes", L"Unable to allocate planes"); return FALSE; }

	BSPPlane bspPlane;

	// Seek to the planes lump in the file
	FSSeek(fp, lumps[eLumpPlanes].offset, FS_SEEK_SET);

	for(u32 i = 0; i < m_numPlanes; i++)
	{
		FSRead(&bspPlane, 1, sizeof(BSPPlane), fp);

		m_planes[i].a = bspPlane.a;
		m_planes[i].b = bspPlane.c;
		m_planes[i].c = -bspPlane.b;

		m_planes[i].d = bspPlane.d;

		PlaneCategorize(&m_planes[i]);
	}

	return TRUE;
}

u32 IgfxQBSP::_LoadMdls(hFILE fp, BSPLump *lumps)
{
	if(m_numModels > 0)
	{
		m_models = (gfxBSPModel*)MemAlloc(sizeof(gfxBSPModel)*m_numModels);
		if(!m_models)
		{ LogMsg(LOG_FILE, L"_QBSPLoadMdls", L"Unable to allocate models"); return FALSE; }

		// Seek to the models lump in the file
		FSSeek(fp, lumps[eLumpModels].offset, FS_SEEK_SET);

		BSPModel qbspMdl;

		for(u32 i = 0; i < m_numModels; i++)
		{
			FSRead(&qbspMdl, 1, sizeof(BSPModel), fp);

			//copy data stuff
			m_models[i].min.x = qbspMdl.min.x;

			//flip y/z
			m_models[i].min.y = MIN(qbspMdl.min.z, qbspMdl.max.z);
			m_models[i].min.z = MIN(-qbspMdl.min.y, -qbspMdl.max.y);

			m_models[i].max.x = qbspMdl.max.x;

			//flip y/z
			m_models[i].max.y = MAX(qbspMdl.min.z, qbspMdl.max.z);
			m_models[i].max.z = MAX(-qbspMdl.min.y, -qbspMdl.max.y);

			m_models[i].faceIndex = qbspMdl.faceIndex;
			m_models[i].numOfFaces = qbspMdl.numOfFaces;
			m_models[i].brushIndex = qbspMdl.brushIndex;
			m_models[i].numOfBrushes = qbspMdl.numOfBrushes;

			//set the transformation stuff
			m_models[i].wrldRot = m_models[i].wrldRot;

			if(i != 0) //do not look at world spawn
			{
				///////////////////////////////////////////
				//create the polyhedrons
				m_models[i].brushes = (ConvexPolyhedron *)MemAlloc(sizeof(ConvexPolyhedron)*m_models[i].numOfBrushes);
				assert(m_models[i].brushes);

				for(s32 b = 0, c = m_models[i].brushIndex; b < m_models[i].numOfBrushes; b++, c++)
				{
					m_models[i].brushes[b].planes   = (Plane*)MemAlloc(sizeof(Plane)*m_brushes[c].numOfBrushSides);
					m_models[i].brushes[b].tPlanes  = (Plane*)MemAlloc(sizeof(Plane)*m_brushes[c].numOfBrushSides);

					m_models[i].brushes[b].txtID    = (s32*)MemAlloc(sizeof(s32)*m_brushes[c].numOfBrushSides);
					m_models[i].brushes[b].numPlane = m_brushes[c].numOfBrushSides;
					
					s32 plInd;
					
					for(s32 d = 0, e = m_brushes[c].brushSide; d < m_brushes[c].numOfBrushSides; d++, e++)
					{
						m_models[i].brushes[b].txtID[d] = m_brushSides[e].textureID;
						
						plInd = m_brushSides[e].plane;
						m_models[i].brushes[b].planes[d] = m_planes[plInd];
						m_models[i].brushes[b].tPlanes[d] = m_planes[plInd];
					}
				}
				///////////////////////////////////////////

				m_models[i].wrldTrans = m_models[i].min + m_models[i].max;
				m_models[i].wrldTrans *= 0.5f;

				MtxTranslate(&m_models[i].wrldTrans, &m_models[i].wrldMtx[FXMTX_NORMAL]);

				//inverse translate vertices
				Vec3D invTrans(-m_models[i].wrldTrans);

				_ModelTranslate(g_tempVtx, &m_models[i], invTrans);

				//inverse translate the bbox
				m_models[i].min += invTrans;
				
				m_models[i].max += invTrans;

				m_models[i].radius = MAX(m_models[i].max.x, m_models[i].max.y);
				m_models[i].radius = MAX(m_models[i].radius, m_models[i].max.z);

				////////////////////////////////////////////////////////////
				//determine if we need to expand the shadow buffer
				s32 faceInd;
				u32 numSHInd=0;
				for(s32 f = 0; f < m_models[i].numOfFaces; f++)
				{
					faceInd = f+m_models[i].faceIndex;

					switch(m_faces[faceInd].type)
					{
					case eFaceMesh:
					case eFacePolygon:
						numSHInd += m_faces[faceInd].numMeshVerts;
						break;

					case eFacePatch:
						break;
					}
				}

				if(numSHInd > m_shVtxNum)
					m_shVtxNum = numSHInd;
				if(numSHInd > m_shIndNum)
					m_shIndNum = numSHInd;
				////////////////////////////////////////////////////////////
			}
			else
			{
				m_models[i].wrldTrans = m_models[i].wrldTrans;
				MtxIdentity(&m_models[i].wrldMtx[FXMTX_NORMAL]);
			}
		}
	}
	else
		m_models = 0;

	return TRUE;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
static inline void _ChangeGammaBits(u8 *pImage, s32 w, s32 h, s32 pitch, f32 factor)
{
	// Go through every pixel in the lightmap
	u8 *thisBit;

	for(s32 y = 0; y < h; y++)
	{
		thisBit = pImage + (y*pitch);

		for(s32 x = 0; x < w; x++)
		{
			_QBSPGammaRGB(thisBit[2], thisBit[1], thisBit[0], factor);

			thisBit += 4; //assume 32-bit
		}
	}
}

u32 IgfxQBSP::_LoadLightmaps(hFILE fp, const tCHAR *filename, BSPLump *lumps)
{
	u32 i;

	tCHAR lightmapName[MAXCHARBUFF];

	BSPLightmap lightmap;

	if(m_numLightMap > 0)
	{
		//allocate lightmap texture handles
		m_lightmaps = (hTXT*)MemAlloc(sizeof(hTXT)*m_numLightMap);
		if(!m_lightmaps)
		{ LogMsg(LOG_FILE, L"_QBSPLoadLightmaps", L"Unable to allocate faces"); return FALSE; }

		// Seek to the position in the file that stores the lightmap information
		FSSeek(fp, lumps[eLumpLightmaps].offset, FS_SEEK_SET);

		for(i = 0; i < m_numLightMap; i++)
		{
			swprintf(lightmapName, L"QBSPLightMap%d", i);

			//read lightmap data
			FSRead(&lightmap, 1, sizeof(BSPLightmap), fp);

			//assume 128x128
			m_lightmaps[i] = TextureCreate(lightmapName, 128, 128, 0, LIGHTMAP_USAGE, GFXFMT_A8R8G8B8, 0);

			//fill up the new texture
			s32 pitch;
			u8 *bitsOut, *thisBit;

			if(m_lightmaps[i]->Lock(0, 0, 0, &pitch, (void **)&bitsOut))
			{
				for(s32 y = 0; y < m_lightmaps[i]->GetHeight(); y++)
				{
					thisBit = bitsOut + (y*pitch);

					for(s32 x = 0; x < m_lightmaps[i]->GetWidth(); x++)
					{
						thisBit[3] = 255;
						thisBit[2] = lightmap.imageBits[y][x][0];
						thisBit[1] = lightmap.imageBits[y][x][1];
						thisBit[0] = lightmap.imageBits[y][x][2];

						thisBit+=4;
					}
				}

				_ChangeGammaBits(bitsOut, m_lightmaps[i]->GetWidth(), 
					m_lightmaps[i]->GetHeight(), pitch, GAMMA_DEFAULT_LIGHTMAP);

				m_lightmaps[i]->Unlock(0);
			}
			else
			{ LogMsg(LOG_FILE, L"_QBSPLoadLightmaps", L"Unable to lock lightmap texture!"); return FALSE; }
		}
	}
	else
	{
		vector<hTXT> lms;

		//let's see if we have external lightmaps
		tCHAR lmDir[MAXCHARBUFF]; wcscpy(lmDir, filename);

		tCHAR *pStr = GetExtension(lmDir);
		if(pStr) *pStr = 0;

		wcscat(lmDir, L"\\");

		tCHAR buff[MAXCHARBUFF]={0}, lmPath[MAXCHARBUFF]={0};

		hTXT txt;

		for(i = 0;;i++)
		{
			wsprintf(buff, L"lm_%.4d.tga", i);

			wcscpy(lmPath, lmDir);
			wcscat(lmPath, buff);

			//verify this file exists
			hFILE fp = FSOpen(lmPath, L"rb");
			if(fp)
			{
				FSClose(fp);

				txt = TextureLoad(lmPath, 0,0);

				if(txt)
				{
					//adjust gamma
					s32 pitch;
					u8 *bitsOut, *thisBit;
					if(txt->Lock(0, 0, 0, &pitch, (void **)&bitsOut))
					{
						_ChangeGammaBits(bitsOut, txt->GetWidth(), txt->GetHeight(), 
							pitch, GAMMA_DEFAULT_LIGHTMAP);

						txt->Unlock(0);
					}

					lms.push_back(txt);
				}
				else
					break;
			}
			else
				break;
		}

		//put it to the buffer
		if(lms.size() > 0)
		{
			m_numLightMap = lms.size();

			//allocate lightmap texture handles
			m_lightmaps = (hTXT*)MemAlloc(sizeof(hTXT)*lms.size());
			if(!m_lightmaps)
			{ LogMsg(LOG_FILE, L"_QBSPLoadLightmaps", L"Unable to allocate faces"); return FALSE; }

			for(i = 0; i < lms.size(); i++)
				m_lightmaps[i] = txt;

			lms.clear();
		}
	}

	return TRUE;
}

u32 IgfxQBSP::_LoadVertInd(hFILE fp, BSPLump *lumps)
{
	s32 ind;

	u16 *buff;

	//create the index buffer
	//create index buffer
	if(!GFXCreateIndBuff(sizeof(u16)*m_numMeshVtxInd, QBSP_INDBUFF_USAGE, 
		16, QBSP_INDBUFF_POOL, &m_meshIndBuff))
		return FALSE;

	// Seek to the mesh vertex indices lump in the file
	FSSeek(fp, lumps[eLumpMeshVerts].offset, FS_SEEK_SET);

	if(IndLock(m_meshIndBuff, 0,0, (void**)&buff, 0))//D3DLOCK_DISCARD))
	{
		for(u32 i = 0; i < m_numMeshVtxInd; i++)
		{
			FSRead(&ind, 1, sizeof(s32), fp);
			buff[i] = (u16)ind;
		}

		IndUnlock(m_meshIndBuff);
	}

	return TRUE;
}

u32 IgfxQBSP::_LoadEntities(hFILE fp, BSPLump *lumps, ENTITYPARSEFUNC parseFunc, void *userDat)
{
	// Seek to the entity lump in the file
	FSSeek(fp, lumps[eLumpEntities].offset, FS_SEEK_SET);

	s8 *strLump;
	tCHAR *wStrLump, *buff;

	tCHAR *entityStr;

	s32 strSize = 0;
	s32 entitySize = lumps[eLumpEntities].length/sizeof(s8);

	strLump  = (s8*)MemAlloc(sizeof(s8)*(entitySize+1));
	wStrLump = (tCHAR*)MemAlloc(sizeof(tCHAR)*(entitySize+1));

	FSRead(strLump, 1, sizeof(s8)*entitySize, fp);
	mbstowcs(wStrLump, (const char *)strLump, entitySize+1);

	buff = wStrLump;

	while(*buff != 0)
	{
		if(ParserCountStringBuff(buff, &strSize, '{', '}') == RETCODE_ENDREACHED && strSize > 0)
		{
			strSize++;
			entityStr = (tCHAR *)MemAlloc(sizeof(tCHAR)*strSize);

			ParserReadStringBuff(&buff, entityStr, strSize, '{', '}');
			ParserSkipCharBuff(&buff, '\n');

			//do whatever
			parseFunc(entityStr, (hQBSP)this, userDat);

			MemFree(entityStr);
		}
		else
		{ LogMsg(LOG_FILE, L"_QBSPLoadEntities", L"Error parsing!!!"); MemFree(strLump); MemFree(wStrLump); return FALSE; }
	}

	MemFree(strLump);
	MemFree(wStrLump);

	return TRUE;
}

u32 IgfxQBSP::_LoadLeaves(hFILE fp, BSPLump *lumps)
{
	BSPLeaf bspLeaf;

	// Seek to the position in the file that holds the leafs
	FSSeek(fp, lumps[eLumpLeafs].offset, FS_SEEK_SET);

	s32 j, k=0;

	gfxBSPFace *face=0;
	BSPBrush *brush;

	for(u32 i = 0; i < m_numLeaves; i++)
	{
		FSRead(&bspLeaf, 1, sizeof(BSPLeaf), fp);

		//copy stuff
		m_leaves[i].area = bspLeaf.area + 1;
		m_leaves[i].cluster = bspLeaf.cluster;
		m_leaves[i].leafBrush = bspLeaf.leafBrush;
		m_leaves[i].leafface = bspLeaf.leafface;
		m_leaves[i].numOfLeafBrushes = bspLeaf.numOfLeafBrushes;
		m_leaves[i].numOfLeafFaces = bspLeaf.numOfLeafFaces;

		// OR brushes' contents
		for ( j = 0; j < bspLeaf.numOfLeafBrushes; j++ ) {
			brush = &m_brushes[m_leafBrushes[bspLeaf.leafBrush + j]];
			m_leaves[i].contents |= m_textures[brush->textureID].contents;
		}

		// OR patches' contents
		for ( j = 0; j < bspLeaf.numOfLeafFaces; j++ ) {
			k = m_leafFaces[bspLeaf.leafface + j];
			face = &m_faces[k];

			//m_leaves[i].contents |= m_textures[face->textureID].contents;

			if ( face->numbrushes ) {
				m_leaves[i].contents |= m_textures[face->textureID].contents;
				//map_leafpatches[numleafpatches++] = k;
				//out->numleafpatches++;
			}
		}

		if ( m_leaves[i].area >= m_numareas ) {
			m_map_areas[m_leaves[i].area].floodnum = 0;
			m_map_areas[m_leaves[i].area].floodvalid = 0;
			m_map_areas[m_leaves[i].area].numareaportals = 0;
			m_numareas = m_leaves[i].area + 1;
		}

		m_leaves[i].min.x = (f32)bspLeaf.min[0];
		m_leaves[i].min.y = (f32)MIN(bspLeaf.min[2], bspLeaf.max[2]);
		m_leaves[i].min.z = (f32)MIN(-bspLeaf.min[1], -bspLeaf.max[1]);

		m_leaves[i].max.x = (f32)bspLeaf.max[0];
		m_leaves[i].max.y = (f32)MAX(bspLeaf.min[2], bspLeaf.max[2]);
		m_leaves[i].max.z = (f32)MAX(-bspLeaf.min[1], -bspLeaf.max[1]);

		m_leaves[i].loc = m_leaves[i].min+m_leaves[i].max;
		m_leaves[i].loc *= 0.5f;
	}

	return TRUE;
}

u32 IgfxQBSP::_CalcPHS()
{
	s32		rowbytes, rowwords;
	s32		i, j, k, l, index;
	s32		bitbyte;
	u32		*dest, *src;
	u8		*scan;
	s32		count;
	s32		numclusters;
	s32		size;

	if ( !m_clusters.numOfClusters ) {
		return FALSE;
	}

	//allocate PHS's buffer
	m_clustersPHS.numOfClusters = numclusters = m_clusters.numOfClusters;
	m_clustersPHS.bytesPerCluster = m_clusters.bytesPerCluster;

	// Allocate the memory for the cluster bitsets
	size = m_clusters.numOfClusters * m_clusters.bytesPerCluster;
	
	m_clustersPHS.pBitsets = (u8*)MemAlloc(sizeof(u8)*size);

	if ( !m_clustersPHS.pBitsets ) {
		LogMsg(LOG_FILE, L"QBSPCalcPHS", L"Unable to allocate PHS"); return FALSE;
	}

	rowwords = m_clusters.bytesPerCluster / sizeof(s32);
	rowbytes = m_clusters.bytesPerCluster;

	count = 0;
	scan = (u8 *)m_clusters.pBitsets;
	dest = (u32 *)(m_clustersPHS.pBitsets);

	for (i=0 ; i<numclusters ; i++, dest += rowwords, scan += rowbytes)
	{
		memcpy (dest, scan, rowbytes);
		for (j=0 ; j<rowbytes ; j++)
		{
			bitbyte = scan[j];
			if (!bitbyte)
				continue;
			for (k=0 ; k<8 ; k++)
			{
				if (! (bitbyte & (1<<k)) )
					continue;
				// OR this pvs row into the phs
				index = (j<<3) + k;
				if (index >= numclusters)
					LogMsg(LOG_FILE, L"QBSPCalcPHS", L"Bad bit in PVS"); // pad bits should be 0
				src = (u32 *)((u8 *)m_clusters.pBitsets) + index*rowwords;
				for (l=0 ; l<rowwords ; l++)
					dest[l] |= src[l];
			}
		}
		for (j=0 ; j<numclusters ; j++)
			if ( ((u8 *)dest)[j>>3] & (1<<(j&7)) )
				count++;
	}

	return TRUE;
}

static u8 _QBSPIsValid(const BSPHeader & inf)
{
	u32 cmp1 = MAKETAG_4(inf.ID[0], inf.ID[1], inf.ID[2], inf.ID[3]);

	if(cmp1 == MAKETAG_4('I', 'B', 'S', 'P') && inf.ver == 0x2e)
		return TRUE;

	return FALSE;
}

/////////////////////////////////////
// Name:	QBSPIsValid
// Purpose:	quickly check if given file
//			exists and is a valid 
//			QBSP map.
// Output:	none
// Return:	TRUE if exist and valid
/////////////////////////////////////
u8 F_API QBSPIsValid(const tCHAR *filename)
{
	u8 ret = FALSE;

	hFILE fp;

	BSPHeader header = {0};

	fp = FSOpen(filename, L"rb");

	if(fp)
	{
		FSRead(&header, 1, sizeof(BSPHeader), fp);

		ret = _QBSPIsValid(header);

		FSClose(fp);
	}

	return ret;
}

IgfxQBSP::IgfxQBSP(const tCHAR *filename, const tCHAR *txtPath, f32 curveDetail, ENTITYPARSEFUNC parseFunc, void *userDat)
: m_ref(0), m_status(0), m_numareas(0), m_numareaportals(0), m_dfltFX(0), 
  m_shVtxBuff(0), m_shIndBuff(0), m_shVtxNum(0), m_shIndNum(0)
{
	u32 i;

	hFILE fp;

	BSPHeader header = {0};
	BSPLump lumps[eNumLumps] = {0};

	fp = FSOpen(filename, L"rb");

	if(!fp)
	{ LogMsg(LOG_FILE, L"QBSPLoad: Unable to open qbsp map", filename); return; }

	/*************************************************************************/
	// allocate and get sizes first

	// Read in the header and lump data
	FSRead(&header, 1, sizeof(BSPHeader), fp);
	FSRead(&lumps, eNumLumps, sizeof(BSPLump), fp);

	//is this a valid file format?
	if(!_QBSPIsValid(header))
	{ FSClose(fp); LogMsg(LOG_FILE, L"QBSPLoad: Invalid File Format", filename); return; }

	////////////////////////////////////////
	//get size of vertices
	m_numVtx = lumps[eLumpVertices].length / sizeof(BSPVertex);
	////////////////////////////////////////

	////////////////////////////////////////
	//get size of faces
	m_numFaces = lumps[eLumpFaces].length / sizeof(BSPFace);

	//allocate faces
	m_faces = (gfxBSPFace*)MemAlloc(sizeof(gfxBSPFace)*m_numFaces);
	if(!m_faces)
	{ LogMsg(LOG_FILE, L"QBSPLoad", L"Unable to allocate faces"); goto DIEQUAKEDIE; }
	////////////////////////////////////////

	////////////////////////////////////////
	//get size of textures
	m_numTxt = lumps[eLumpTextures].length / sizeof(BSPTexture);
	////////////////////////////////////////

	////////////////////////////////////////
	//get size of lightmaps
	m_numLightMap = lumps[eLumpLightmaps].length / sizeof(BSPLightmap);
	////////////////////////////////////////

	////////////////////////////////////////
	//get size of nodes
	m_numNodes = lumps[eLumpNodes].length / sizeof(BSPNode);

	//allocate nodes (filled in later)
	m_nodes = (gfxBSPNode*)MemAlloc(sizeof(gfxBSPNode)*m_numNodes);
	if(!m_nodes)
	{ LogMsg(LOG_FILE, L"QBSPLoad", L"Unable to allocate nodes"); goto DIEQUAKEDIE; }
	////////////////////////////////////////

	////////////////////////////////////////
	//get size of leaves
	m_numLeaves = lumps[eLumpLeafs].length / sizeof(BSPLeaf);

	//allocate nodes (filled in later)
	m_leaves = (gfxBSPLeaf*)MemAlloc(sizeof(gfxBSPLeaf)*m_numLeaves);
	if(!m_leaves)
	{ LogMsg(LOG_FILE, L"QBSPLoad", L"Unable to allocate leaves"); goto DIEQUAKEDIE; }
	////////////////////////////////////////

	////////////////////////////////////////
	//get size of leaf faces
	m_numLeafFaces = lumps[eLumpLeafFaces].length / sizeof(int);

	//allocate nodes (filled in later)
	m_leafFaces = (s32*)MemAlloc(sizeof(s32)*m_numLeafFaces);
	if(!m_leafFaces)
	{ LogMsg(LOG_FILE, L"QBSPLoad", L"Unable to allocate leaf faces"); goto DIEQUAKEDIE; }
	////////////////////////////////////////

	////////////////////////////////////////
	//get size of planes
	m_numPlanes = lumps[eLumpPlanes].length / sizeof(BSPPlane);
	////////////////////////////////////////

	////////////////////////////////////////
	//get size of mesh vertices
	m_numMeshVtxInd = lumps[eLumpMeshVerts].length / sizeof(s32);
	////////////////////////////////////////

	////////////////////////////////////////
	//get size of models
	m_numModels = lumps[eLumpModels].length / sizeof(BSPModel);
	////////////////////////////////////////

	////////////////////////////////////////
	//get size of brushes
	m_numBrushes = lumps[eLumpBrushes].length / sizeof(BSPBrush);

	//allocate brushes (filled in later)
	m_brushes = (BSPBrush*)MemAlloc(sizeof(BSPBrush)*m_numBrushes);
	if(!m_brushes)
	{ LogMsg(LOG_FILE, L"QBSPLoad", L"Unable to allocate brushes"); goto DIEQUAKEDIE; }
	////////////////////////////////////////

	////////////////////////////////////////
	//get size of leaf brushes
	m_numLeafBrushes = lumps[eLumpLeafBrushes].length / sizeof(int);

	//allocate brushes (filled in later)
	m_leafBrushes = (s32*)MemAlloc(sizeof(s32)*m_numLeafBrushes);
	if(!m_leafBrushes)
	{ LogMsg(LOG_FILE, L"QBSPLoad", L"Unable to allocate leaf brushes"); goto DIEQUAKEDIE; }
	////////////////////////////////////////

	////////////////////////////////////////
	//get size of brush sides
	m_numBrushSides = lumps[eLumpBrushSides].length / sizeof(BSPBrushSide);

	//allocate brushes (filled in later)
	m_brushSides = (BSPBrushSide*)MemAlloc(sizeof(BSPBrushSide)*m_numBrushSides);
	if(!m_brushSides)
	{ LogMsg(LOG_FILE, L"QBSPLoad", L"Unable to allocate brush sides"); goto DIEQUAKEDIE; }
	////////////////////////////////////////

	/*************************************************************************/

	/*************************************************************************/
	//Now let's parse the files and fill in our map

	////////////////////////////////////////
	//Load up the vertices
	if(_LoadVert(fp, lumps) != TRUE)
	{ goto DIEQUAKEDIE; }
	////////////////////////////////////////

	////////////////////////////////////////
	//load up mesh vertex indices
	if(_LoadVertInd(fp, lumps) != RETCODE_SUCCESS)
	{ LogMsg(LOG_FILE, L"QBSPLoad", L"Unable to load vertex indices"); goto DIEQUAKEDIE; }
	////////////////////////////////////////

	////////////////////////////////////////
	//load up brushes
	// Seek to the brush lump in the file
	FSSeek(fp, lumps[eLumpBrushes].offset, FS_SEEK_SET);

	//read them all
	FSRead(m_brushes, m_numBrushes, sizeof(BSPBrush), fp);
	////////////////////////////////////////

	////////////////////////////////////////
	//load up leaf brushes
	// Seek to the leaf brush lump in the file
	FSSeek(fp, lumps[eLumpLeafBrushes].offset, FS_SEEK_SET);

	//read them all
	FSRead(m_leafBrushes, m_numLeafBrushes, sizeof(int), fp);
	////////////////////////////////////////

	////////////////////////////////////////
	//load up brush sides
	// Seek to the brush side lump in the file
	FSSeek(fp, lumps[eLumpBrushSides].offset, FS_SEEK_SET);

	//read them all
	FSRead(m_brushSides, m_numBrushSides, sizeof(BSPBrushSide), fp);
	////////////////////////////////////////

	//set the default effects
	CreateDfltFX();

	////////////////////////////////////////
	//load up textures
	if(_LoadTxts(fp, lumps, txtPath) != RETCODE_SUCCESS)
	{ goto DIEQUAKEDIE; }
	////////////////////////////////////////

	////////////////////////////////////////
	//load up lightmaps
	_LoadLightmaps(fp, filename, lumps);
	////////////////////////////////////////

	////////////////////////////////////////
	//load up planes
	if(_LoadPlanes(fp, lumps) != RETCODE_SUCCESS)
	{ LogMsg(LOG_FILE, L"QBSPLoad", L"Unable to load vertex indices"); goto DIEQUAKEDIE; }
	////////////////////////////////////////

	////////////////////////////////////////
	//load up vis
	if(lumps[eLumpVisData].length)
	{
		// Seek to the position in the file that holds the visibility lump
		FSSeek(fp, lumps[eLumpVisData].offset, FS_SEEK_SET);

		// Read in the number of vectors and each vector's size
		FSRead(&m_clusters.numOfClusters,	 1, sizeof(int), fp);
		FSRead(&m_clusters.bytesPerCluster, 1, sizeof(int), fp);

		// Allocate the memory for the cluster bitsets
		int size = m_clusters.numOfClusters * m_clusters.bytesPerCluster;
		
		m_clusters.pBitsets = (u8*)MemAlloc(sizeof(u8)*size);
		if(!m_clusters.pBitsets)
		{ LogMsg(LOG_FILE, L"QBSPLoad", L"Unable to allocate PVS"); goto DIEQUAKEDIE; }

		// Read in the all the visibility bitsets for each cluster
		FSRead(m_clusters.pBitsets, 1, sizeof(u8)*size, fp);
	}
	else
		m_clusters.pBitsets = 0;
	////////////////////////////////////////

	////////////////////////////////////////
	//load up faces
	if(_LoadFaces(fp, curveDetail, lumps) != TRUE)
	{ goto DIEQUAKEDIE; }
	////////////////////////////////////////

	////////////////////////////////////////
	// load up leaf faces
	// Seek to the leaf faces lump, then read it's data
	FSSeek(fp, lumps[eLumpLeafFaces].offset, FS_SEEK_SET);
	FSRead(m_leafFaces, m_numLeafFaces, sizeof(int), fp);
	////////////////////////////////////////

	////////////////////////////////////////
	//load up leaves
	_LoadLeaves(fp, lumps);
	////////////////////////////////////////

	////////////////////////////////////////
	//load up nodes
	BSPNode bspNode;

	// Seek to the position in the file that hold the nodes
	FSSeek(fp, lumps[eLumpNodes].offset, FS_SEEK_SET);

	for(i = 0; i < m_numNodes; i++)
	{
		FSRead(&bspNode, 1, sizeof(BSPNode), fp);

		//copy stuff
		m_nodes[i].child[1] = bspNode.back;
		m_nodes[i].child[0] = bspNode.front;
		m_nodes[i].plane = bspNode.plane;

		m_nodes[i].min.x = (f32)bspNode.min[0];
		m_nodes[i].min.y = (f32)MIN(bspNode.min[2], bspNode.max[2]);
		m_nodes[i].min.z = (f32)MIN(-bspNode.min[1], -bspNode.max[1]);

		m_nodes[i].max.x = (f32)bspNode.max[0];
		m_nodes[i].max.y = (f32)MAX(bspNode.min[2], bspNode.max[2]);
		m_nodes[i].max.z = (f32)MAX(-bspNode.min[1], -bspNode.max[1]);
	}
	////////////////////////////////////////

	////////////////////////////////////////
	//load up models
	if(_LoadMdls(fp, lumps) != RETCODE_SUCCESS)
	{ LogMsg(LOG_FILE, L"QBSPLoad", L"Unable to load models"); goto DIEQUAKEDIE; }
	////////////////////////////////////////

	////////////////////////////////////////
	//load up the entities
	
	if(parseFunc)
	{
		if(_LoadEntities(fp, lumps, parseFunc, userDat) != RETCODE_SUCCESS)
		{ LogMsg(LOG_FILE, L"QBSPLoad", L"Unable to load entities"); goto DIEQUAKEDIE; }
	}
	////////////////////////////////////////

	////////////////////////////////////////
	//calculate PHS
	_CalcPHS();

	////////////////////////////////////////

	//VIS stuff
	m_brushVis = QBSPVisBitCreate(m_numBrushes, m_numBrushVis);

	////////////////////////////////////////
	//initialize all portals as closed
	m_floodvalid = 0;
	memset (m_portalopen, 0, sizeof(m_portalopen));
	FloodAreaConnections();
	////////////////////////////////////////

	//something for the masses
	memset(g_nullrow, 255, MAX_NULLROW);

	//copy temporary vertices to actual vertices
	gfxVtxQBSP *pVtx;

	if(VtxLock(m_vtx, 0, 0,0, (void**)&pVtx, 0))
	{
		memcpy(pVtx, g_tempVtx, sizeof(gfxVtxQBSP)*m_numVtx);
		
		VtxUnlock(m_vtx, 0);
	}

	//set other stuff
	MtxIdentity(&m_wrldMtx);	

	//initialize decals
	memset(m_decals, 0, sizeof(m_decals));
	ClearDecals();

	//initialize shadow buffers
	if(m_shVtxNum)
	{
		m_shVtxBuff = (Vec3D*)MemAlloc(sizeof(Vec3D)*m_shVtxNum);
		GFXShadowInit(m_shVtxNum*7, m_shVtxNum*2, 0);
	}
	if(m_shIndNum)
		m_shIndBuff = (u16*)MemAlloc(sizeof(u16)*m_shIndNum);

DIEQUAKEDIE:
	FSClose(fp);

	//destroy temporary vertices
	if(g_tempVtx)
	{ MemFree(g_tempVtx); g_tempVtx = 0; }

	//set to lightmap by default
	//QBSPUseVertexLighting(newQBSP, FALSE);
}

void IgfxQBSP::AddRef()
{
	m_ref++;
}

/////////////////////////////////////
// Name:	QBSPLoad
// Purpose:	loads a .bsp file and
//			create a qbsp map
// Output:	none
// Return:	new qbsp, 0 if failure
/////////////////////////////////////
hQBSP F_API QBSPLoad(const tCHAR *filename, const tCHAR *txtPath, 
					 f32 curveDetail, ENTITYPARSEFUNC parseFunc, void *userDat)
{
	hQBSP newQBSP = (hQBSP) new IgfxQBSP(filename, txtPath, curveDetail, parseFunc, userDat);

	if(!newQBSP) { LogMsg(LOG_FILE, filename, L"Unable to allocate QBSP map"); }

	return newQBSP;
}