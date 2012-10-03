#include "gfx_i.h"

#include "GFX\gfx_math.h"

LPD3DXSPRITE		     IgfxTexture::s_pTxtSprite=0; //global sprite interface

GFXVTXBUFF			    *IgfxTexture::s_pTxtVtxBuff=0; //global sprite 3d interface

GFXVTXBUFF			    *IgfxTexture::s_pTxtBBVtxBuff=0; //global Billboard 3d interface

hEFFECT					 IgfxTexture::s_txtEffect=0;		//the 3d display effect

s8 g_txtFXBuff[] = "bool transparent = true;\n"\
					 "texture BaseTex : TEX0;\n"\
					 "float4x4 TransMtx : WORLDVIEWPROJECTION;\n"\
					 "float4 Diffuse : MATERIALDIFFUSE = {1,1,1,1};\n"\
					 "struct VS_OUTPUT\n"\
					 "{\n"\
					 "float4 Pos : POSITION;\n"\
					 "float4 Diff : COLOR0;\n"\
					 "float2 Tex : TEXCOORD0;\n"\
					 "};\n"\
					 "VS_OUTPUT VS(float3 Pos : POSITION, float2 Tex : TEXCOORD0) {\n"\
					 "VS_OUTPUT Out = (VS_OUTPUT)0;\n"\
					 "Out.Pos  = mul(float4(Pos, 1), TransMtx);\n"\
					 "Out.Diff = Diffuse;\n"\
					 "Out.Tex  = Tex; return Out; }\n"\
					 "sampler Sampler = sampler_state {\n"\
					 "Texture = (BaseTex);\n"\
					 "MipFilter = LINEAR;\n"\
					 "MinFilter = LINEAR;\n"\
					 "MagFilter = LINEAR; };\n"\
					 "technique TVertexShaderOnly {\n"\
					 "pass P0 {\n"\
					 "cullmode = none;\n"\
					 "ZWRITEENABLE = false;\n"\
					 "Sampler[0] = (Sampler);\n"\
					 "ColorOp[0] = MODULATE;\n"\
					 "ColorArg1[0] = TEXTURE;\n"\
					 "ColorArg2[0] = DIFFUSE;\n"\
					 "AlphaOp[0] = MODULATE;\n"\
					 "AlphaArg1[0] = TEXTURE;\n"\
					 "AlphaArg2[0] = DIFFUSE;\n"\
					 "ColorOp[1] = DISABLE;\n"\
					 "AlphaOp[1] = DISABLE;\n"\
					 "VertexShader = compile vs_1_1 VS();\n"\
					 "PixelShader = NULL; } }\n";

void Txt3DFXCallback(hIEFFECT effect, u32 wParam, s32 lParam)
{
	hTXT txt = (hTXT)wParam;

	effect->SetBaseTexture(0, txt);
}

u32 IgfxTexture::sInitSys()
{
	//create sprite interface
	D3DXCreateSprite(g_p3DDevice, &s_pTxtSprite);

	//create effect
	s_txtEffect = EffectCreate(L"Texture3DBlt", g_txtFXBuff);

	//create the texture 3d vtx
	s_pTxtVtxBuff = GFXCreateVtxBuff(sizeof(txtVSVertex), MAXVTX, VTXBUFF_USAGE, VERTEXFLAG, (GFXPOOL)VTXBUFF_POOL);

	if(!s_pTxtVtxBuff)
	{ LogMsg(LOG_FILE, L"IgfxTexture::sInitSys", L"Error creating texture 3d vtx"); return FALSE; }

	//create the texture billboard vtx
	s_pTxtBBVtxBuff = GFXCreateVtxBuff(sizeof(txtVSVertex), MAXVTX, VTXBUFF_USAGE, VERTEXFLAG, (GFXPOOL)VTXBUFF_POOL);

	if(!s_pTxtBBVtxBuff)
	{ LogMsg(LOG_FILE, L"IgfxTexture::sInitSys", L"Error creating texture billboard vtx"); return FALSE; }

	//////////////////////////////////////////////////
	//now let's set the vertex declaration
	D3DVERTEXELEMENT9 Declaration[3];
    D3DVERTEXELEMENT9 End = D3DDECL_END();

	Declaration[0].Stream = 0;
	Declaration[0].Offset = 0;
	Declaration[0].Type = D3DDECLTYPE_FLOAT3;
	Declaration[0].Method = D3DDECLMETHOD_DEFAULT;
	Declaration[0].Usage = D3DDECLUSAGE_POSITION;
	Declaration[0].UsageIndex = 0;

	Declaration[1].Stream = 0;
	Declaration[1].Offset = sizeof(f32)*3;
	Declaration[1].Type = D3DDECLTYPE_FLOAT3;
	Declaration[1].Method = D3DDECLMETHOD_DEFAULT;
	Declaration[1].Usage = D3DDECLUSAGE_TEXCOORD;
	Declaration[1].UsageIndex = 0;

	Declaration[2] = End;

	VtxSetDecl(s_pTxtVtxBuff, Declaration);
	VtxSetDecl(s_pTxtBBVtxBuff, Declaration);
	//////////////////////////////////////////////////

	//set the vertex data
	txtVSVertex *pTVtx, *pTBBVtx;

	VtxLock(s_pTxtVtxBuff, 0, 0,0, (void**)&pTVtx, 0);
	VtxLock(s_pTxtBBVtxBuff, 0, 0,0, (void**)&pTBBVtx, 0);

	/*

		d-------c
		|	    |
		|       |
		|       |
		a-------b

	*/

	pTVtx[0].x = -1.0f;  pTVtx[0].y = -1.0f; pTVtx[0].z = 0.0f;
	
	pTVtx[1].x = 1.0f;  pTVtx[1].y = -1.0f; pTVtx[1].z = 0.0f;
	
	pTVtx[2].x = 1.0f; pTVtx[2].y = 1.0f; pTVtx[2].z = 0.0f;
	
	pTVtx[3].x = -1.0f; pTVtx[3].y = 1.0f; pTVtx[3].z = 0.0f;

	//Top Left
	pTVtx[0].s = 0;
	pTVtx[0].t = 1;
	pTVtx[0].u = 1;

	//Bottom Left
	pTVtx[1].s = 1;
	pTVtx[1].t = 1;
	pTVtx[1].u = 1;

	//Bottom Right
	pTVtx[2].s = 1;
	pTVtx[2].t = 0;
	pTVtx[2].u = 1;

	//Top Right
	pTVtx[3].s = 0;
	pTVtx[3].t = 0;
	pTVtx[3].u = 1;

	memcpy(pTBBVtx, pTVtx, sizeof(txtVSVertex)*4);

	VtxUnlock(s_pTxtVtxBuff, 0);
	VtxUnlock(s_pTxtBBVtxBuff, 0);

	return TRUE;
}

void IgfxTexture::sDestroySys()
{
	SAFE_RELEASE(s_pTxtSprite);

	GFXDestroyVtxBuff(s_pTxtVtxBuff); s_pTxtVtxBuff = 0;
	GFXDestroyVtxBuff(s_pTxtBBVtxBuff); s_pTxtBBVtxBuff = 0;

	SAFE_RELEASE(s_txtEffect);
}

Vec3D g_txtDir;

//set up the billboard to face the camera
void IgfxTexture::sBillboardPrepare(const Vec3D & camDir)
{
	g_txtDir = -camDir;

	txtVSVertex *pTVtx;

	Vec3D right(g_mtxView[FXMTX_NORMAL]._11, g_mtxView[FXMTX_NORMAL]._21, g_mtxView[FXMTX_NORMAL]._31);
	Vec3D up(g_mtxView[FXMTX_NORMAL]._12, g_mtxView[FXMTX_NORMAL]._22, g_mtxView[FXMTX_NORMAL]._32);

	if(VtxLock(s_pTxtBBVtxBuff, 0,0,0,(void**)&pTVtx,0))
	{
		/*

			d-------c
			 |	   |
			 |     |
			 |     |
			a-------b

		a = center - (right + up) * size;
		b = center + (right - up) * size;
		c = center + (right + up) * size;
		d = center - (right - up) * size;

		*/

		pTVtx[0].x =  -(right.x+up.x);
		pTVtx[0].y =  -(right.y+up.y);
		pTVtx[0].z =  -(right.z+up.z);

		pTVtx[1].x =  (right.x-up.x);
		pTVtx[1].y =  (right.y-up.y);
		pTVtx[1].z =  (right.z-up.z);

		pTVtx[2].x =  (right.x+up.x);
		pTVtx[2].y =  (right.y+up.y);
		pTVtx[2].z =  (right.z+up.z);

		pTVtx[3].x =  -(right.x-up.x);
		pTVtx[3].y =  -(right.y-up.y);
		pTVtx[3].z =  -(right.z-up.z);

		VtxUnlock(s_pTxtBBVtxBuff, 0);
	}
}

LPD3DXSPRITE IgfxTexture::sGetISpr()
{
	return s_pTxtSprite;
}

/////////////////////////////////////
// Name:	TextureBegin
// Purpose:	before blting anything
//			you need to call this
// Output:	textures ready to be
//			blted
// Return:	none
/////////////////////////////////////
void F_API TextureBegin()
{
	IgfxTexture::sGetISpr()->Begin(0);
}

/////////////////////////////////////
// Name:	TextureEnd
// Purpose:	after you're done blting
//			textures, call this
// Output:	
// Return:	none
/////////////////////////////////////
void F_API TextureEnd()
{
	IgfxTexture::sGetISpr()->End();
}

/////////////////////////////////////
// Purpose:	blt given texture
//			make sure you call 
//			TextureBegin first
// Output:	texture blted on screen
// Return:	TRUE if success
/////////////////////////////////////
u32 IgfxTexture::Blt(f32 x, f32 y, const iRect *srcRect, gfxBlt *pBlt)
{
	Vec3D vect(x,y,0);

	D3DXVECTOR3 center(0,0,0);

	PGFXTEXTURE texture = (PGFXTEXTURE)GetImg();

	if(!texture) return FALSE;

	if(pBlt)
	{
		Vec3D scale(1,1,1);
		Vec3D rotCenter(pBlt->rotOfsX,pBlt->rotOfsY,0);

		f32 sW, sH;
	
		if(srcRect)
		{ sW = (f32)(srcRect->right - srcRect->left); sH = (f32)(srcRect->bottom - srcRect->top); }
		else
		{ sW = (f32)m_width; sH = (f32)m_height; }

		if(TESTFLAGS(pBlt->flags, GFXBLT_FLIPH))
		{ scale.x = -1; center.x += sW; }

		if(TESTFLAGS(pBlt->flags, GFXBLT_FLIPV))
		{ scale.y = -1; center.y += sH; }

		if(pBlt->rot != 0 && !TESTFLAGS(pBlt->flags, GFXBLT_ROTABS))
		{ rotCenter.x -= sW/2; rotCenter.y -= sH/2; }

		Matrix scaleRotOfsM(scale.x, 0,       0,       rotCenter.x,
			                0,       scale.y, 0,       rotCenter.y,
							0,       0,       scale.z, rotCenter.z,
							0,       0,       0,       1);

		Matrix rotRotOfsInvTransM; MtxRotateZ(pBlt->rot, &rotRotOfsInvTransM);
		
		rotRotOfsInvTransM._41 = -rotCenter.x;
		rotRotOfsInvTransM._42 = -rotCenter.y;
		rotRotOfsInvTransM._43 = -rotCenter.z;

		Matrix transM; MtxTranslate(&vect, &transM);

		s_pTxtSprite->SetTransform((D3DXMATRIX*)&Matrix(scaleRotOfsM*rotRotOfsInvTransM*transM));
		s_pTxtSprite->Draw(texture, (const RECT*)srcRect, &center, 0, pBlt->clr);

		//g_pTxtSprite->Draw(texture, (const RECT*)srcRect, &scale, &rotCenter, pBlt->rot, &vect, pBlt->clr);
	}
	else
	{
		Matrix mtxIden; MtxIdentity(&mtxIden);
		s_pTxtSprite->SetTransform((D3DXMATRIX*)&mtxIden);
		s_pTxtSprite->Draw(texture, (const RECT*)srcRect, &center, (D3DXVECTOR3*)&vect, 0xffffffff);
	}

	s_pTxtSprite->Flush();

	return TRUE;
}

/////////////////////////////////////
// Purpose:	blt given texture
//			make sure you call 
//			TextureBegin first
// Output:	texture blted on screen
// Return:	TRUE if success
/////////////////////////////////////
u32 IgfxTexture::StretchBlt(f32 x, f32 y, f32 w, f32 h, const iRect *srcRect, gfxBlt *pBlt)
{
	Vec3D vect(x,y,0);

	D3DXVECTOR3 center(0,0,0);

	PGFXTEXTURE texture = (PGFXTEXTURE)GetImg();

	f32 sW, sH;
	
	if(srcRect)
	{ sW = (f32)(srcRect->right - srcRect->left); sH = (f32)(srcRect->bottom - srcRect->top); }
	else
	{ sW = (f32)m_width; sH = (f32)m_height; }

	Vec3D scale(w/sW,h/sH,0);

	if(pBlt)
	{
		Vec3D rotCenter(pBlt->rotOfsX,pBlt->rotOfsY, 0);

		if(TESTFLAGS(pBlt->flags, GFXBLT_FLIPH))
		{ scale.x *= -1; center.x += sW; }

		if(TESTFLAGS(pBlt->flags, GFXBLT_FLIPV))
		{ scale.y *= -1; center.y += sH; }

		if(pBlt->rot != 0 && !TESTFLAGS(pBlt->flags, GFXBLT_ROTABS))
		{ rotCenter.x -= w/2; rotCenter.y -= h/2; }

		Matrix scaleRotOfsM(scale.x, 0,       0,       rotCenter.x,
			                0,       scale.y, 0,       rotCenter.y,
							0,       0,       scale.z, rotCenter.z,
							0,       0,       0,       1);

		Matrix rotRotOfsInvTransM; MtxRotateZ(pBlt->rot, &rotRotOfsInvTransM);
		
		rotRotOfsInvTransM._41 = -rotCenter.x;
		rotRotOfsInvTransM._42 = -rotCenter.y;
		rotRotOfsInvTransM._43 = -rotCenter.z;

		Matrix transM; MtxTranslate(&vect, &transM);

		s_pTxtSprite->SetTransform((D3DXMATRIX*)&Matrix(scaleRotOfsM*rotRotOfsInvTransM*transM));
		s_pTxtSprite->Draw(texture, (const RECT*)srcRect, &center, 0, pBlt->clr);
	}
	else
	{
		Matrix scaleRotOfsM(scale.x, 0,       0,       vect.x,
			                0,       scale.y, 0,       vect.y,
							0,       0,       scale.z, vect.z,
							0,       0,       0,       1);

		s_pTxtSprite->SetTransform((D3DXMATRIX*)&scaleRotOfsM);
		s_pTxtSprite->Draw(texture, (const RECT*)srcRect, &center, 0, 0xffffffff);
	}

	s_pTxtSprite->Flush();

	return TRUE;
}

GFXVTXBUFF * IgfxTexture::sGetVtx3D()
{
	return s_pTxtVtxBuff;
}

//use Texture Display's FX for blting
void IgfxTexture::Blt3DEX(GFXVTXBUFF *vtxBuff, GFXPRIMITIVETYPE type, u32 numVtx, u32 numFaces,
						  const Matrix * worldMtx, const Vec4D & clr)
{
	gfxMaterial mat;
	mat.Diffuse = clr;

	SceneGeometry g={(hIEFFECT)s_txtEffect, 0, 0, 0, 0, vtxBuff, 0, type, 0, 0, numVtx, numFaces, &mat, 
		(Matrix*)worldMtx, Txt3DFXCallback, (u32)this, 0, 0, 0, SCENE_SINGLEMTX};

	GFXSceneAdd(g);
}

//get the default texture display FX
hEFFECT IgfxTexture::sGetDfltFX()
{
	return s_txtEffect;
}

/////////////////////////////////////
// Purpose:	render a flat texture on
//			scene. Texture will be
//			displayed on GFXSceneRender
// Output:	scene added
// Return:	TRUE if success
/////////////////////////////////////
u32 IgfxTexture::Blt3D(const Matrix *wrldMtx, const Vec4D *pClr)
{
	if(pClr)
		Blt3DEX(s_pTxtVtxBuff, GFXPT_TRIANGLEFAN, 4, 2, wrldMtx, *pClr);
	else
	{
		Vec4D clr(1,1,1,1);
		Blt3DEX(s_pTxtVtxBuff, GFXPT_TRIANGLEFAN, 4, 2, wrldMtx, clr);
	}
	
	return TRUE;
}

/////////////////////////////////////
// Purpose:	render texture as
//			billboard. Texture will be
//			displayed on GFXSceneRender
// Output:	scene added
// Return:	TRUE if success
/////////////////////////////////////
u32 IgfxTexture::BltBillboard(const Vec3D & center, f32 size, f32 rot, const Vec4D *pClr)
{
	//check for frustrum
	if(!FrustrumCheckCube(center.x, center.y, center.z, size))
		return TRUE;

	Matrix trans; MtxIdentity(&trans);

	if(rot != 0)
	{
		Matrix axisM; MtxAxisRotation(&g_txtDir, rot, &axisM);

		trans._11 = size*axisM._11;
		trans._21 = size*axisM._21;
		trans._31 = size*axisM._31;

		trans._12 = size*axisM._12;
		trans._22 = size*axisM._22;
		trans._32 = size*axisM._32;

		trans._13 = size*axisM._13;
		trans._23 = size*axisM._23;
		trans._33 = size*axisM._33;

		trans._14 = size*axisM._14;
		trans._24 = size*axisM._24;
		trans._34 = size*axisM._34;

		trans._41 = center.x; trans._42 = center.y; trans._43 = center.z;
	}
	else
	{
		trans._11 = size; trans._22 = size; trans._33 = size;
		trans._41 = center.x; trans._42 = center.y; trans._43 = center.z;
	}

	if(pClr)
		Blt3DEX(s_pTxtBBVtxBuff, GFXPT_TRIANGLEFAN, 4, 2, &trans, *pClr);
	else
	{
		Vec4D clr(1,1,1,1);
		Blt3DEX(s_pTxtBBVtxBuff, GFXPT_TRIANGLEFAN, 4, 2, &trans, clr);
	}

	return TRUE;
}