/*********************************************************************NVMH3****
File:  $Id: scene_TV.fx,v 1.1 2004/06/04 00:58:40 david Exp $

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
    Flickering scanlines, as from an old TV.
	Loosely based on the version seen at NVISION Moscow.
	Accelerated a fair bit.

    Not made for EffectEdit.

<scene_commands>
<setcolortarget texture="BaseMap1"/>
<setdepthstenciltarget texture="DepthMap"/>
<clear color="0,0,0" depth="1.0"/>
<draw type="objects"/>
<setcolortarget/>
<settechnique name="TV"/>
<draw type="buffer"/>
</scene_commands>
******************************************************************************/

///////////////////////////////////////////////////////////
/////////////////////////////////////// Tweakables ////////
///////////////////////////////////////////////////////////

half Timer : TIMEELAPSE < string UIWidget = "None"; >;

half ScanLines = 486;

half Speed <
	string UIName = "V.Hold";
	string UIWidget = "slider";
	half UIMin = 0.0;
	half UIMax = 1.0;
	half UIStep = 0.01;
> = 0.05f;

///////////////////////////////////////////////////////////
/////////////////////////////////////// Un-Tweakables /////
///////////////////////////////////////////////////////////

half2 ScreenSize : VIEWPORTPIXELSIZE < string UIWidget="None"; >;

///////////////////////////////////////////////////////////
///////////////////////////// Render-to-Texture Data //////
///////////////////////////////////////////////////////////

texture BaseMap1 : RENDERCOLORTARGET < 
    float2 ViewPortDimensions = {1.0,1.0};
    int MIPLEVELS = 1;
    string format = "X8R8G8B8";
    string UIWidget = "None";
>;
texture DepthMap : RENDERDEPTHSTENCILTARGET
<
    float2 ViewPortDimensions = {1.0,1.0};
    string format = "D24S8";
    string UIWidget = "None";
>;

sampler BaseSamp1 = sampler_state {
    texture = <BaseMap1>;
    AddressU  = CLAMP;        
    AddressV  = CLAMP;
    AddressW  = CLAMP;
    MIPFILTER = NONE;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};

///////////////////////////////////////////
// Sine Func //////////////////////////////
///////////////////////////////////////////

texture SineTex < 
    string TextureType = "VOLUME"; 
    string function = "sine_function"; 
    string UIWidget = "None";
    int width  = 32;
    int height = 1;
>;

sampler1D SineSampler = sampler_state {
    texture = (SineTex);
    MipFilter = NONE;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
};

float4 sine_function(float3 Pos : POSITION) : COLOR
{
	return 0.5*sin(Pos.x*2*3.141592653589793238) + 0.5f;
}

///////////////////////////////////////////
// 3D Noise ///////////////////////////////
///////////////////////////////////////////

texture NoisyTex < 
    string TextureType = "VOLUME"; 
    string function = "noisy_function"; 
    string UIWidget = "None";
    int width  = 64;
    int height = 64;
    int depth  = 64; 
>;

sampler3D NoisySampler = sampler_state {
    texture = (NoisyTex);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
    AddressW = WRAP;
};

// Noise function

float4 noisy_function(float3 Pos : POSITION) : COLOR
{
	return (noise(Pos * 50.5) * .5) + .5f;
}

///////////////////////////////////////////////////////////
/////////////////////////////////// data structures ///////
///////////////////////////////////////////////////////////

struct VS_OUTPUT
{
   	half4 Position   : POSITION;
    half4 TexCoordA   : TEXCOORD0;
    half4 ScanFlash   : TEXCOORD1;
    half4 TexCoordB   : TEXCOORD2;
    half4 TexCoordC   : TEXCOORD3;
    half4 TexCoordD   : TEXCOORD4;
    half4 TexCoordE   : TEXCOORD5;
    half4 TexCoordF   : TEXCOORD6;
};

////////////////////////////////////////////////////////////
////////////////////////////////// vertex shaders //////////
////////////////////////////////////////////////////////////

VS_OUTPUT VS_Quad(half3 Position : POSITION, 
			half3 TexCoord : TEXCOORD0)
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;
    OUT.Position = half4(Position, 1);
	half tx = TexCoord.x + (1+sin(Timer/2))*0.002;
	half ty = TexCoord.y + (1+sin(frac(Timer*2)))*0.002;
    half4 baseTC = half4(tx,ty,TexCoord.z, 1); 
    OUT.TexCoordA = baseTC;
    OUT.TexCoordB = (baseTC+Timer) * 11;
    OUT.TexCoordC = (baseTC-Timer) * 11;
    OUT.TexCoordD = (-baseTC+Timer) * 11;
	OUT.TexCoordE = (baseTC+Timer) * 2;
	OUT.TexCoordF = (baseTC+Timer) * 5;
	half scan = ty*ScanLines+Timer*Speed;
	// Flash
	half flash = 1.0;
	if(frac(Timer/10)<0.1) flash = 3.0*(0.5+0.5*sin(Timer*4.0));
    OUT.ScanFlash = half4(scan,flash,0,1); 
    return OUT;
}

//////////////////////////////////////////////////////
////////////////////////////////// pixel shaders /////
//////////////////////////////////////////////////////

half4 PS_TV(VS_OUTPUT IN) : COLOR
{   
	half4 img = tex2D(BaseSamp1, IN.TexCoordA.xy);
	half scanlines = tex1D(SineSampler,IN.ScanFlash.x).xxx;
	img *= scanlines;
	img *= IN.ScanFlash.y;
	half4 noise = half4(tex3D(NoisySampler, IN.TexCoordB).x,
							tex3D(NoisySampler, IN.TexCoordC).x,
							tex3D(NoisySampler, IN.TexCoordD).x,1);
	half4 noise2 = tex3D(NoisySampler, IN.TexCoordE);
	half4 noise3 = tex3D(NoisySampler, IN.TexCoordF);
	img *= 3.0 * noise*noise2*noise3 + 0.8;
	return (img);
}  

////////////////////////////////////////////////////////////
/////////////////////////////////////// techniques /////////
////////////////////////////////////////////////////////////

technique TV {
    pass TV_lines
    {
		cullmode = none;
		ZEnable = false;
		VertexShader = compile vs_2_0 VS_Quad();
		PixelShader  = compile ps_2_0 PS_TV();
    }
}

////////////// eof ///
