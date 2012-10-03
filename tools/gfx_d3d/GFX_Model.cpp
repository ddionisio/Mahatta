#include "cal3d/cal3d.h"
#include "gfx_i.h"

map<wstring, hIMDL>	IgfxModel::s_MDLLIST;

s8 g_mdlDfltFXBuff[] =
"texture BaseTxt : TEX0;"\
"float4x4 World : WORLD;"\
"float4x4 View : VIEW;"\
"float4x4 Projection : PROJECTION;"\
"float4 k_a : MATERIALAMBIENT = { 1.0f, 1.0f, 1.0f, 1.0f };"\
"float4 k_d : MATERIALDIFFUSE = { 1.0f, 1.0f, 1.0f, 1.0f };"\
"float4 k_s : MATERIALSPECULAR= { 1.0f, 1.0f, 1.0f, 1.0f };"\
"int    n   : MATERIALPOWER = 32;"\
"sampler Sampler = sampler_state {\n"\
"Texture = (BaseTxt);\n"\
"MipFilter = LINEAR;\n"\
"MinFilter = LINEAR;\n"\
"MagFilter = LINEAR; };\n"\
"technique Normal {\n"\
"pass P0 {\n"\
"WorldTransform[0] = (World);"\
"ViewTransform = (View);"\
"ProjectionTransform = (Projection);"\
"MaterialAmbient  = (k_a);\n"\
"MaterialDiffuse  = (k_d);\n"\
"MaterialSpecular = (k_s);\n"\
"MaterialPower = (n);\n"\
"cullmode = cw;\n"\
"SPECULARENABLE=TRUE;\n"\
"NORMALIZENORMALS=TRUE;\n"\
"Sampler[0] = (Sampler);\n"\
"ColorOp[0] = MODULATE;\n"\
"ColorArg1[0] = TEXTURE;\n"\
"ColorArg2[0] = DIFFUSE;\n"\
"AlphaOp[0] = MODULATE;\n"\
"AlphaArg1[0] = TEXTURE;\n"\
"AlphaArg2[0] = DIFFUSE;\n"\
"ColorOp[1] = DISABLE;\n"\
"AlphaOp[1] = DISABLE;\n"\
"VertexShader = NULL;\n"\
"PixelShader = NULL; } }";

IgfxModel::IgfxModel(const tCHAR *filename)
: m_ref(0), m_pCalCoreModel(0), m_pMesh(0), m_fxDflt(0), m_vtxBuff(0), m_indBuff(0), m_flag(0),
  m_fvf(0), m_lodSDist(0), m_lodDist(0), m_shadDist(0)
{
	s32 ret;

	//check extension and load the correct model format
	tCHAR *ext = GetExtension(filename);

	if(ext && wcsicmp(ext, L".X") == 0)
	{
		ret = LoadX(filename);
	}
	//only Cal3D for now
	else
	{
		ret = LoadCal3D(filename);
	}

	if(ret)
	{
		//add it to our effect table
		//better not be there!!!
		assert(s_MDLLIST.find(filename) == s_MDLLIST.end());

		s_MDLLIST[filename] = this;

		//find it again and get the name pointer
		map<wstring, hIMDL>::iterator it = s_MDLLIST.find(filename); //assert(it == s_MDLLIST.end());

		m_name = (*it).first.c_str();

		//load the default FX
		m_fxDflt = EffectSearchName(L"MDLDFLTFX");

		if(m_fxDflt)
			m_fxDflt->AddRef();
		else
		{
			//create default FX, should only happen once
			m_fxDflt = EffectCreate(L"MDLDFLTFX", g_mdlDfltFXBuff);
		}
	}
	else
	{ LogMsg(LOG_FILE, L"MDLLoad error loading", filename); }
}

IgfxModel::~IgfxModel()
{
	s_MDLLIST.erase(m_name);

	Cal3DDestroy();

	XDestroy();

	GFXDestroyVtxBuff(m_vtxBuff);
	SAFE_RELEASE(m_indBuff);

	SAFE_RELEASE(m_fxDflt);
}

void IgfxModel::AddRef()
{
	m_ref++;
}

s32 IgfxModel::Release()
{
	if(m_ref == 0)
		delete this;
	else
	{ m_ref--; return m_ref; }

	return 0;
}

/////////////////////////////////////
// Name:	MDLLoad
// Purpose:	load a model from given
//			filename
// Output:	model loaded
// Return:	new model
/////////////////////////////////////
hMDL F_API MDLLoad(const tCHAR *filename)
{
	hMDL newModel = MDLSearchName(filename);

	//do we have this model loaded already?
	if(newModel)
	{
		newModel->AddRef();
		return newModel;
	}

	newModel = (hMDL)new IgfxModel(filename);

	if(!newModel)
	{ LogMsg(LOG_FILE, L"MDLLoad unable to allocate", filename); return 0; }

	return newModel;
}

hIMDL IgfxModel::sFind(const tCHAR *name)
{
	map<wstring, hIMDL>::iterator it = s_MDLLIST.find(name);

	if(it != s_MDLLIST.end())
		return (*it).second;

	return 0;
}

/////////////////////////////////////
// Name:	MDLSearchName
// Purpose:	search model by name
// Output:	none
// Return:	the model
/////////////////////////////////////
hMDL F_API MDLSearchName(const tCHAR *name)
{
	return (hMDL)IgfxModel::sFind(name);
}

/////////////////////////////////////
// Purpose:	forcibly destroy all 
//			models
// Output:	models destroyed
// Return:	none
/////////////////////////////////////
void IgfxModel::sDestroyAll()
{
	map<wstring, hIMDL>::iterator iNext;
	for(map<wstring, hIMDL>::iterator i = s_MDLLIST.begin(); i != s_MDLLIST.end(); i = iNext)
	{
		iNext = i; iNext++;
		if((*i).second) delete (*i).second;
	}

	s_MDLLIST.clear();
}

/////////////////////////////////////
// Purpose:	get the animation ID of
//			given name
// Output:	none
// Return:	the ID, -1 if not found
/////////////////////////////////////
s32 IgfxModel::GetAnimID(const tCHAR *name)
{
	if(!m_pCalCoreModel)
		return -1;

	char sName[MAXCHARBUFF]={0}; wcstombs(sName, name, MAXCHARBUFF);
	return m_pCalCoreModel->getAnimId(sName);
}

/////////////////////////////////////
// Purpose:	get the number of animations
//			of the given mdl
// Output:	none
// Return:	number of animations
/////////////////////////////////////
s32 IgfxModel::GetNumAnim()
{
	if(!m_pCalCoreModel)
		return 0;

	return m_pCalCoreModel->getCoreAnimationCount();
}

/////////////////////////////////////
// Purpose:	get the bone ID of given
//			name
// Output:	none
// Return:	the ID, -1 if not found
/////////////////////////////////////
s32 IgfxModel::GetBoneID(const tCHAR *name)
{
	if(!m_pCalCoreModel)
		return -1;

	char sName[MAXCHARBUFF]={0}; wcstombs(sName, name, MAXCHARBUFF);
	return m_pCalCoreModel->getBoneId(sName);
}

/////////////////////////////////////
// Purpose:	get number of material
// Output:	none
// Return:	no. of material
/////////////////////////////////////
u32 IgfxModel::GetNumMaterial()
{
	if(m_pCalCoreModel)
		return Cal3DGetNumMat();
	else if(m_pMesh)
		return XGetNumMat();

	return 0;
}

/////////////////////////////////////
// Purpose:	get the FX of given mdl
//			and material index
// Output:	none
// Return:	the FX
/////////////////////////////////////
hEFFECT IgfxModel::GetEffect(u32 ind)
{
	if(m_pCalCoreModel)
		return Cal3DGetEffect(ind);
	else if(m_pMesh)
		return XGetEffect(ind);

	return 0;
}

/////////////////////////////////////
// Purpose:	set the model's FX
// Output:	this will add ref to fx
// Return:	none
/////////////////////////////////////
void IgfxModel::SetEffect(u32 ind, hEFFECT fx)
{
	if(m_pCalCoreModel)
		Cal3DSetEffect(ind, fx);
	else if(m_pMesh)
		XSetEffect(ind, fx);
}

/////////////////////////////////////
// Purpose:	get the texture of given
//			mdl, material index and map
//			index.
// Output:	
// Return:	
/////////////////////////////////////
hTXT IgfxModel::GetTxt(u32 ind, u32 mapInd)
{
	if(m_pCalCoreModel)
		return Cal3DGetTxt(ind, mapInd);
	else if(m_pMesh)
		return XGetTxt(ind, mapInd);

	return 0;
}

/////////////////////////////////////
// Purpose:	set texture of given mdl,
//			material index and map index
// Output:	txt ref add
// Return:	none
/////////////////////////////////////
void IgfxModel::SetTxt(u32 ind, u32 mapInd, hTXT txt)
{
	if(m_pCalCoreModel)
		Cal3DSetTxt(ind, mapInd, txt);
	else if(m_pMesh)
		XSetTxt(ind, mapInd, txt);
}

/////////////////////////////////////
// Purpose:	get the model's material
// Output:	pMtrl set
// Return:	none
/////////////////////////////////////
void IgfxModel::GetMaterial(u32 ind, gfxMaterial *pMtrl)
{
	if(m_pCalCoreModel)
		Cal3DGetMtrl(ind, pMtrl);
	else if(m_pMesh)
		XGetMtrl(ind, pMtrl);
}

/////////////////////////////////////
// Purpose:	set the model's material
// Output:	material set
// Return:	none
/////////////////////////////////////
void IgfxModel::SetMaterial(u32 ind, const gfxMaterial *pMtrl)
{
	if(m_pCalCoreModel)
		Cal3DSetMtrl(ind, pMtrl);
	else if(m_pMesh)
		XSetMtrl(ind, pMtrl);
}

//Internal GFX functions:

void IgfxModel::sInvalidateAll()
{
	hIMDL mdl;

	for(map<wstring, hIMDL>::iterator i = s_MDLLIST.begin(); i != s_MDLLIST.end(); ++i)
	{
		mdl = (*i).second;

		if(!mdl->m_pCalCoreModel)
			continue;

		GFXDestroyVtxBuff(mdl->m_vtxBuff);
		SAFE_RELEASE(mdl->m_indBuff);
	}
}

void IgfxModel::sRestoreAll()
{
	hIMDL mdl;

	for(map<wstring, hIMDL>::iterator i = s_MDLLIST.begin(); i != s_MDLLIST.end(); ++i)
	{
		mdl = (*i).second;

		if(!mdl->m_pCalCoreModel)
			continue;

		mdl->_Cal3DCreateBuffer();
	}
}