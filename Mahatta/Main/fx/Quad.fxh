/*********************************************************************NVMH3****
File:  $Id: Quad.fxh,v 1.1 2004/06/04 00:58:38 david Exp $

Copyright NVIDIA Corporation 2002
TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
*AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.


Comments:
    Header file for use with FX Composer <drawquad> scene commands
    Not made for EffectEdit.

Use DECLARE_QUAD_TEX(t,s,f) for most RTT rendertarget declarations
Use ScreenQuadVS() as a standard vertex shader for screen-aligned quads
Use TexQuadPS(s) as a standard pixel shader for copying texture to a Screen-aligned Quad
Use TexQuadBiasPS(s,b) as a biased version of TexQuadPS()

QuadScreenSize is VIEWPORTPIXELSIZE
QuadTexOffset reconciles pixel and texel addressing

******************************************************************************/

#ifndef _QUAD_FXH
#define _QUAD_FXH

// Types we are likely to encounter....
//	Redefine these before including "Quad.fxh" if you want
//		to use a type other than "half" for these actions
#ifndef QUAD_REAL
#define QUAD_REAL half
#define QUAD_REAL2 half2
#define QUAD_REAL3 half3
#define QUAD_REAL4 half4
#define QUAD_REAL3x3 half3x3
#define QUAD_REAL4x3 half4x3
#define QUAD_REAL3x4 half3x4
#define QUAD_REAL4x4 half4x4
#endif /* QUAD_REAL */

// example usage: DECLARE_QUAD_TEX(ObjMap,ObjSampler,"A8R8G8B8")
#define DECLARE_QUAD_TEX(TextureName,SamplerName,PixelFormat) texture TextureName : RENDERCOLORTARGET < \
    float2 ViewPortDimensions = {1.0,1.0}; \
    int MIPLEVELS = 1; \
    string format = PixelFormat ; \
	string UIWidget = "None"; \
>; \
sampler SamplerName = sampler_state { \
    texture = <TextureName>; \
    AddressU  = CLAMP; \
    AddressV = CLAMP; \
    MIPFILTER = NONE; \
    MINFILTER = LINEAR; \
    MAGFILTER = LINEAR; \
};

// example usage: DECLARE_QUAD_DEPTH_BUFFER(ObjMap,"D24S8")
#define DECLARE_QUAD_DEPTH_BUFFER(TextureName,PixelFormat) texture TextureName : RENDERDEPTHSTENCILTARGET < \
    float2 ViewPortDimensions = {1.0,1.0}; \
    string format = PixelFormat ; \
	string UIWidget = "None"; \
>; 

QUAD_REAL QuadTexOffset = 0.5;
QUAD_REAL2 QuadScreenSize : VIEWPORTPIXELSIZE < string UIWidget="None"; >;

////////////////////////////////////////////////////////////
////////////////////////////////// vertex shaders //////////
////////////////////////////////////////////////////////////

struct QuadVertexOutput
{
   	QUAD_REAL4 Position	: POSITION;
    QUAD_REAL2 UV		: TEXCOORD0;
};

QuadVertexOutput ScreenQuadVS(
		QUAD_REAL3 Position : POSITION, 
		QUAD_REAL3 TexCoord : TEXCOORD0
) {
    QuadVertexOutput OUT;
    OUT.Position = QUAD_REAL4(Position, 1);
	QUAD_REAL2 off = QUAD_REAL2(QuadTexOffset/(QuadScreenSize.x),QuadTexOffset/(QuadScreenSize.y));
    OUT.UV = QUAD_REAL2(TexCoord.xy+off); 
    return OUT;
}

//////////////////////////////////////////////////////
////////////////////////////////// pixel shaders /////
//////////////////////////////////////////////////////

// add glow on top of model

QUAD_REAL4 TexQuadPS(QuadVertexOutput IN,uniform sampler2D InputSampler) : COLOR
{   
	QUAD_REAL4 texCol = tex2D(InputSampler, IN.UV);
	return texCol;
}  

QUAD_REAL4 TexQuadBiasPS(QuadVertexOutput IN,uniform sampler2D InputSampler,QUAD_REAL TBias) : COLOR
{   
	QUAD_REAL4 texCol = tex2Dbias(InputSampler, QUAD_REAL4(IN.UV,0,TBias));
	return texCol;
}  

#define TEX_TECH(TechName,SamplerName) technique TechName { \
    pass TexturePass  { \
		VertexShader = compile vs_2_0 ScreenQuadVS(); \
		AlphaBlendEnable = false; ZEnable = false; \
		PixelShader  = compile ps_2_a TexQuadPS(SamplerName); } }

#define TEX_BLEND_TECH(TechName,SamplerName) technique TechName { \
    pass TexturePass { \
		VertexShader = compile vs_2_0 ScreenQuadVS(); \
		ZEnable = false; AlphaBlendEnable = true; \
		SrcBlend = SrcAlpha; DestBlend = InvSrcAlpha; \
		PixelShader  = compile ps_2_a TexQuadPS(SamplerName); } }

#endif /* _QUAD_FXH */

////////////// eof ///
