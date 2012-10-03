#include "..//gfx_i.h"

#include "gfx_qbsp.h"
#include "gfx_qbsp_i.h"

//EffectSetBaseTexture(hEFFECT fx, u32 ind, hTXT txt);
//"cullmode = ccw;\n"
s8 g_qbspDfltFXBuff[] = 
"texture BaseTex : TEX0;"\
"texture ShadowTex : TEX1;"\
"float4x4 World : WORLD;"\
"float4x4 View : VIEW;"\
"float4x4 Projection : PROJECTION;"\

"sampler Sampler = sampler_state {\n"\
"Texture = (BaseTex);\n"\
"MipFilter = LINEAR;\n"\
"MinFilter = LINEAR;\n"\
"MagFilter = LINEAR; };\n"\
"sampler Sampler2 = sampler_state {\n"\
"Texture = (ShadowTex);\n"\
"MipFilter = LINEAR;\n"\
"MinFilter = LINEAR;\n"\
"MagFilter = LINEAR; };\n"\

"technique Level_MultiTex < string compat = \"d3dcapsTexCoordCount=2\"; > {\n"\
"pass P0 {\n"\
"WorldTransform[0] = (World);"\
"ViewTransform = (View);"\
"ProjectionTransform = (Projection);"\
"cullmode = ccw;\n"\
"Lighting = FALSE;\n"\
"Sampler[0] = (Sampler);\n"\
"ColorOp[0] = SELECTARG1;\n"\
"ColorArg1[0] = TEXTURE;\n"\
"AlphaOp[0] = DISABLE;\n"\
"Sampler[1] = (Sampler2);\n"\
"ColorOp[1] = MODULATE;\n"\
"ColorArg1[1] = TEXTURE;\n"\
"ColorArg2[1] = CURRENT;\n"\
"AlphaOp[1] = DISABLE;\n"\
"VertexShader = NULL;\n"\
"PixelShader = NULL; } }\n"\

"technique Level_NoLightMap {\n"\
"pass P0 {\n"\
"WorldTransform[0] = (World);"\
"ViewTransform = (View);"\
"ProjectionTransform = (Projection);"\
"cullmode = ccw;\n"\
"Lighting = FALSE;\n"\
"Sampler[0] = (Sampler);\n"\
"ColorOp[0] = SELECTARG1;\n"\
"ColorArg1[0] = TEXTURE;\n"\
"AlphaOp[0] = DISABLE;\n"\
"ColorOp[1] = DISABLE;\n"\
"AlphaOp[1] = DISABLE;\n"\
"VertexShader = NULL;\n"\
"PixelShader = NULL; } }\n"\

"technique "\
"Model"\
" {\n"\
"pass P0 {\n"\
"WorldTransform[0] = (World);"\
"ViewTransform = (View);"\
"ProjectionTransform = (Projection);"\
"cullmode = ccw;\n"\
"Sampler[0] = (Sampler);\n"\
"ColorOp[0] = SELECTARG1;\n"\
"ColorArg1[0] = TEXTURE;\n"\
"AlphaOp[0] = SELECTARG1;\n"\
"AlphaArg1[0] = TEXTURE;\n"\
"ColorOp[1] = DISABLE;\n"\
"AlphaOp[1] = DISABLE;\n"\
"VertexShader = NULL;\n"\
"PixelShader = NULL; } }\n";

void QBSPFXCB(hIEFFECT effect, u32 wParam, s32 lParam)
{
	static hIQBSP qbsp; qbsp = (hIQBSP)wParam;

	static hTXT txt; txt = qbsp->FaceGetTexture(lParam);
	static hTXT lmTxt; lmTxt = qbsp->FaceGetLMTexture(lParam);

	if(txt)
		effect->SetBaseTexture(0, txt);
	
	if(lmTxt)
		effect->SetBaseTexture(1, lmTxt);
}

u32 IgfxQBSP::CreateDfltFX()
{
	//just in case...
	SAFE_RELEASE(m_dfltFX);

	m_dfltFX = EffectCreate(L"QBSPDefaultFX", g_qbspDfltFXBuff);

	//get the technique for the model
	m_mdlFXTech = m_dfltFX->GetTechnique(QBSP_MDL_TECH);

	if(!m_dfltFX->ValidateTechnique(m_mdlFXTech))
		m_dfltFX->GetNextValidTechnique(m_mdlFXTech, &m_mdlFXTech);

	return TRUE;
}