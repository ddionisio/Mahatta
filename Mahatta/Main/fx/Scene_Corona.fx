/*********************************************************************NVMH3****
File:  $Id: Scene_Corona.fx,v 1.1 2004/06/04 00:58:38 david Exp $

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
    Render-to-Texture (RTT) animated glow example.
    Blurs is done in two separable passes.

    Not made for EffectEdit.

<scene_commands>
<setcolortarget texture="ObjectsMap"/>
	<clear color="0.1,0.1,0.1,0" depth="1.0"/>
	<draw type="objects"/>
<setcolortarget texture="HBlurredMap"/>
	<settechnique name="HorizBlur"/>
	<draw type="buffer"/>
<setcolortarget texture="GlowMap"/>
	<clear color="0.1,0.1,0.1,0" depth="1.0"/>
	<settechnique name="VertBlur"/>
	<draw type="buffer"/>
<setcolortarget/>
	<clear color="0.1,0.1,0.1,0" depth="1.0"/>
	<settechnique name="RedrawGlow"/>
	<draw type="buffer"/>
	<settechnique name="RedrawObj"/>
	<draw type="buffer"/>
</scene_commands>
******************************************************************************/

#define NOISY_HALO

///////////////////////////////////////////////////////////
/////////////////////////////////////// Tweakables ////////
///////////////////////////////////////////////////////////

float4 GlowCol <
	string UIName = "Glow Color";
	string UIWidget = "Color";
> = {1.0f, 0.6f, 0.0f, 1.0f};

float Glowness <
    string UIName = "Glow Strength";
    string UIWidget = "slider";
    float UIMin = 0.0f;
    float UIMax = 3.0f;
    float UIStep = 0.02f;
> = 2.2f;

float Bias <
	string UIWidget = "slider";
	float UIMin = 0.0;
	float UIMax = 8.0;
	float UIStep = 0.1;
> = 3.0f;

float Trailfade <
	string UIName = "Trail Fade";
	string UIWidget = "slider";
	float UIMin = 0.0;
	float UIMax = 1.0;
	float UIStep = 0.01;
> = 0.6f;

///////////////////////////////////////////////////////////
/////////////////////////////////////// Un-Tweakables /////
///////////////////////////////////////////////////////////

float2 ScreenSize : VIEWPORTPIXELSIZE < string UIWidget="None"; >;

///////////////////////////////////////////////////////////
///////////////////////////// Render-to-Texture Data //////
///////////////////////////////////////////////////////////

texture ObjectsMap : RENDERCOLORTARGET
< 
    float2 ViewPortDimensions = {1.0,1.0};
    int MIPLEVELS = 1;
    string format = "A8R8G8B8";
    string UIWidget = "None";
>;

sampler ObjectsSamp = sampler_state 
{
    texture = <ObjectsMap>;
    AddressU  = CLAMP;        
    AddressV  = CLAMP;
    AddressW  = CLAMP;
    MIPFILTER = NONE;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};

texture HBlurredMap : RENDERCOLORTARGET
< 
    float2 ViewPortDimensions = {1.0,1.0};
    int MIPLEVELS = 1;
    string format = "A8R8G8B8";
    string UIWidget = "None";
>;

sampler HBlurredSamp = sampler_state 
{
    texture = <HBlurredMap>;
    AddressU  = CLAMP;        
    AddressV  = CLAMP;
    AddressW  = CLAMP;
    MIPFILTER = NONE;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};

texture GlowMap : RENDERCOLORTARGET
< 
    float2 ViewPortDimensions = {1.0,1.0};
    int MIPLEVELS = 1;
    string format = "A8R8G8B8";
    string UIWidget = "None";
>;

sampler GlowSampler = sampler_state 
{
    texture = <GlowMap>;
    AddressU  = CLAMP;        
    AddressV  = CLAMP;
    AddressW  = CLAMP;
    MIPFILTER = NONE;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};

//////////////////////////////////////
/// Noise ////////////////////////////
//////////////////////////////////////

#ifdef NOISY_HALO

float Timer : TIME < string UIWidget = "None"; >;

float Speed <
	string UIName = "Noise Speed";
	string UIWidget = "slider";
	float UIMin = 0.0;
	float UIMax = 0.3;
	float UIStep = 0.01;
> = 0.04f;

float2 Crawl <
	string UIName = "Noise Crawl Speed";
> = {0.04f, -0.02f};

float NoiseBright <
	string UIName = "Anim Variance";
	string UIWidget = "slider";
	float UIMin = 0.0;
	float UIMax = 4.0;
	float UIStep = 0.01;
> = 1.5f;

float NoiseScale <
	string UIName = "Anim Size";
	string UIWidget = "slider";
	float UIMin = 0.0;
	float UIMax = 3.0;
	float UIStep = 0.01;
> = 0.9f;

#include "noise_3d.fxh"

#endif /* NOISY_HALO */

///////////////////////////////////////////////////////////
/////////////////////////////////// data structures ///////
///////////////////////////////////////////////////////////

struct VS_OUTPUT_BLUR
{
    float4 Position   : POSITION;
    float4 Diffuse    : COLOR0;
    float4 TexCoord0   : TEXCOORD0;
    float4 TexCoord1   : TEXCOORD1;
    float4 TexCoord2   : TEXCOORD2;
    float4 TexCoord3   : TEXCOORD3;
    float4 TexCoord4   : TEXCOORD4;
    float4 TexCoord5   : TEXCOORD5;
    float4 TexCoord6   : TEXCOORD6;
    float4 TexCoord7   : TEXCOORD7;
    float4 TexCoord8   : COLOR1;   
};

struct VS_OUTPUT
{
   	float4 Position   : POSITION;
    float4 Diffuse    : COLOR0;
    float4 TexCoord0   : TEXCOORD0;
};

////////////////////////////////////////////////////////////
////////////////////////////////// vertex shaders //////////
////////////////////////////////////////////////////////////

VS_OUTPUT VS_Quad(float3 Position : POSITION, 
			float3 TexCoord : TEXCOORD0)
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;
    OUT.Position = float4(Position, 1);
    OUT.TexCoord0 = float4(TexCoord, Bias); 
    return OUT;
}

VS_OUTPUT_BLUR VS_Quad_Vertical_9tap(float3 Position : POSITION, 
			float3 TexCoord : TEXCOORD0)
{
    VS_OUTPUT_BLUR OUT = (VS_OUTPUT_BLUR)0;
    OUT.Position = float4(Position, 1);
	float TexelIncrement = (1.0+Bias)/ScreenSize.y;
    //float3 Coord = float3(TexCoord.x + TexelIncrement, TexCoord.y + TexelIncrement, Bias);
    float3 Coord = float3(TexCoord.x, TexCoord.y, Bias);
    OUT.TexCoord0 = float4(Coord.x, Coord.y + TexelIncrement, TexCoord.z, Bias);
    OUT.TexCoord1 = float4(Coord.x, Coord.y + TexelIncrement * 2, TexCoord.z, Bias);
    OUT.TexCoord2 = float4(Coord.x, Coord.y + TexelIncrement * 3, TexCoord.z, Bias);
    OUT.TexCoord3 = float4(Coord.x, Coord.y + TexelIncrement * 4, TexCoord.z, Bias);
    OUT.TexCoord4 = float4(Coord.x, Coord.y, TexCoord.z, Bias);
    OUT.TexCoord5 = float4(Coord.x, Coord.y - TexelIncrement, TexCoord.z, Bias);
    OUT.TexCoord6 = float4(Coord.x, Coord.y - TexelIncrement * 2, TexCoord.z, Bias);
    OUT.TexCoord7 = float4(Coord.x, Coord.y - TexelIncrement * 3, TexCoord.z, Bias);
    OUT.TexCoord8 = float4(Coord.x, Coord.y - TexelIncrement * 4, TexCoord.z, Bias);
    return OUT;
}

VS_OUTPUT_BLUR VS_Quad_Horizontal_9tap(float3 Position : POSITION, 
			float3 TexCoord : TEXCOORD0)
{
    VS_OUTPUT_BLUR OUT = (VS_OUTPUT_BLUR)0;
    OUT.Position = float4(Position, 1);
	float TexelIncrement = (1.0+Bias)/ScreenSize.x;
    float3 Coord = float3(TexCoord.x, TexCoord.y, Bias);
    OUT.TexCoord0 = float4(Coord.x + TexelIncrement, Coord.y, TexCoord.z, Bias);
    OUT.TexCoord1 = float4(Coord.x + TexelIncrement * 2, Coord.y, TexCoord.z, Bias);
    OUT.TexCoord2 = float4(Coord.x + TexelIncrement * 3, Coord.y, TexCoord.z, Bias);
    OUT.TexCoord3 = float4(Coord.x + TexelIncrement * 4, Coord.y, TexCoord.z, Bias);
    OUT.TexCoord4 = float4(Coord.x, Coord.y, TexCoord.z, Bias);
    OUT.TexCoord5 = float4(Coord.x - TexelIncrement, Coord.y, TexCoord.z, Bias);
    OUT.TexCoord6 = float4(Coord.x - TexelIncrement * 2, Coord.y, TexCoord.z, Bias);
    OUT.TexCoord7 = float4(Coord.x - TexelIncrement * 3, Coord.y, TexCoord.z, Bias);
    OUT.TexCoord8 = float4(Coord.x - TexelIncrement * 4, Coord.y, TexCoord.z, Bias);
    return OUT;
}

//////////////////////////////////////////////////////
////////////////////////////////// pixel shaders /////
//////////////////////////////////////////////////////

// For two-pass blur, we have chosen to do  the horizontal blur FIRST. The
//	vertical pass includes a post-blur scale factor.

// Relative filter weights indexed by distance from "home" texel
//    This set for 9-texel sampling
#define WT9_0 1.0
#define WT9_1 0.8
#define WT9_2 0.6
#define WT9_3 0.4
#define WT9_4 0.2

// Alt pattern -- try your own!
// #define WT9_0 0.1
// #define WT9_1 0.2
// #define WT9_2 3.0
// #define WT9_3 1.0
// #define WT9_4 0.4

#define WT9_NORMALIZE (WT9_0+2.0*(WT9_1+WT9_2+WT9_3+WT9_4))

float4 PS_Blur_Horizontal_9tap(VS_OUTPUT_BLUR IN) : COLOR
{   
    float OutCol = tex2Dbias(ObjectsSamp, IN.TexCoord0).w * (WT9_1/WT9_NORMALIZE);
    OutCol += tex2Dbias(ObjectsSamp, IN.TexCoord1).w * (WT9_2/WT9_NORMALIZE);
    OutCol += tex2Dbias(ObjectsSamp, IN.TexCoord2).w * (WT9_3/WT9_NORMALIZE);
    OutCol += tex2Dbias(ObjectsSamp, IN.TexCoord3).w * (WT9_4/WT9_NORMALIZE);
    OutCol += tex2Dbias(ObjectsSamp, IN.TexCoord4).w * (WT9_0/WT9_NORMALIZE);
    OutCol += tex2Dbias(ObjectsSamp, IN.TexCoord5).w * (WT9_1/WT9_NORMALIZE);
    OutCol += tex2Dbias(ObjectsSamp, IN.TexCoord6).w * (WT9_2/WT9_NORMALIZE);
    OutCol += tex2Dbias(ObjectsSamp, IN.TexCoord7).w * (WT9_3/WT9_NORMALIZE);
    OutCol += tex2Dbias(ObjectsSamp, IN.TexCoord8).w * (WT9_4/WT9_NORMALIZE);
    OutCol += Trailfade * tex2D(GlowSampler, IN.TexCoord4.xy).x;
#ifdef NOISY_HALO
	float3 nuv = float3((NoiseScale*IN.TexCoord4.xy+(Timer*Crawl)),(Speed*Timer));
    OutCol *= NoiseBright*(NOISE3D(nuv)).x;
#endif /* NOISY_HALO */
    return OutCol.xxxx;
} 

float4 PS_Blur_Vertical_9tap(VS_OUTPUT_BLUR IN) : COLOR
{   
    float OutCol = tex2Dbias(HBlurredSamp, IN.TexCoord0).w * (WT9_1/WT9_NORMALIZE);
    OutCol += tex2Dbias(HBlurredSamp, IN.TexCoord1).w * (WT9_2/WT9_NORMALIZE);
    OutCol += tex2Dbias(HBlurredSamp, IN.TexCoord2).w * (WT9_3/WT9_NORMALIZE);
    OutCol += tex2Dbias(HBlurredSamp, IN.TexCoord3).w * (WT9_4/WT9_NORMALIZE);
    OutCol += tex2Dbias(HBlurredSamp, IN.TexCoord4).w * (WT9_0/WT9_NORMALIZE);
    OutCol += tex2Dbias(HBlurredSamp, IN.TexCoord5).w * (WT9_1/WT9_NORMALIZE);
    OutCol += tex2Dbias(HBlurredSamp, IN.TexCoord6).w * (WT9_2/WT9_NORMALIZE);
    OutCol += tex2Dbias(HBlurredSamp, IN.TexCoord7).w * (WT9_3/WT9_NORMALIZE);
    OutCol += tex2Dbias(HBlurredSamp, IN.TexCoord8).w * (WT9_4/WT9_NORMALIZE);
	// OutCol = OutCol.w * GlowCol;	// all alpha
    float4 glo = (Glowness*OutCol)*GlowCol;
    // float4 OldCol = tex2Dbias(ObjectsSamp, IN.TexCoord0);
    // return OldCol + glo;
    return glo;
} 

////////

// just drawn model itself

// add glow on top of model

float4 PS_GlowPass(VS_OUTPUT IN) : COLOR
{   
	float4 tex = tex2D(GlowSampler, float2(IN.TexCoord0.x, IN.TexCoord0.y));
	return tex;
}  

float4 PS_OrigPass(VS_OUTPUT IN) : COLOR
{   
	float4 tex = tex2D(ObjectsSamp, float2(IN.TexCoord0.x, IN.TexCoord0.y));
	return tex;
}  

////////////////////////////////////////////////////////////
/////////////////////////////////////// techniques /////////
////////////////////////////////////////////////////////////

technique HorizBlur
{
    pass BlurGlowBuffer_Horz
    {
	cullmode = none;
	ZEnable = false;
	VertexShader = compile vs_2_0 VS_Quad_Horizontal_9tap();
	PixelShader  = compile ps_2_0 PS_Blur_Horizontal_9tap();
    }
}

technique VertBlur
{
    pass BlurGlowBuffer_Vert
    {
	cullmode = none;
	ZEnable = false;
	VertexShader = compile vs_2_0 VS_Quad_Vertical_9tap();
	PixelShader  = compile ps_2_0 PS_Blur_Vertical_9tap();
    }
}

technique RedrawGlow
{
    pass BlurGlowBuffer_Vert 
    {
	VertexShader = compile vs_2_0 VS_Quad();
	AlphaBlendEnable = true;
	SrcBlend = One;
	DestBlend = Zero;
	PixelShader  = compile ps_2_0 PS_GlowPass();
    }
}

technique RedrawObj
{
    pass BlurGlowBuffer_Vert
    {
	VertexShader = compile vs_2_0 VS_Quad();
	AlphaBlendEnable = true;
	SrcBlend = One;
	DestBlend = InvSrcAlpha;
	PixelShader  = compile ps_2_0 PS_OrigPass();
    }
}