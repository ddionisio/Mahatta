/*********************************************************************NVMH3****
File:  $Id: Scene_CameraShake.fx,v 1.1 2004/06/04 00:58:38 david Exp $

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

<scene_commands>
<setcolortarget texture="SceneTexture"/>
<setdepthstenciltarget texture="DepthTexture"/>
<clear color="0,0,0" depth="1.0"/>
<draw type="objects"/>
<setcolortarget/>
<settechnique name="Desat" />
<draw type="buffer"/>
</scene_commands>
******************************************************************************/

#include "Quad.fxh"
#include "vertex_noise.fxh"

DECLARE_QUAD_TEX(SceneTexture,SceneSampler,"X8R8G8B8")

/*texture SceneTexture : RENDERCOLORTARGET < 
    //float2 ViewPortDimensions = {1.0,1.0}; 
    float width = 512;
    float height = 512;
    int MIPLEVELS = 1; 
    string format = "X8R8G8B8"; 
	string UIWidget = "None"; 
>; 

sampler SceneSampler = sampler_state { 
    texture = <SceneTexture>; 
    AddressU  = CLAMP; 
    AddressV = CLAMP; 
    MIPFILTER = NONE; 
    MINFILTER = LINEAR; 
    MAGFILTER = LINEAR; 
};*/

DECLARE_QUAD_DEPTH_BUFFER(DepthTexture, "D24S8")

QUAD_REAL Time : TIMEELAPSE <string UIWidget="None";>;

///////////////////////////////////////////////////////////
/////////////////////////////////////// Tweakables ////////
///////////////////////////////////////////////////////////

float Speed <
    string UIWidget = "slider";
    float UIMin = -1.0f;
    float UIMax = 100.0f;
    float UIStep = 0.01f;
> = 31.f;

float Shake <
    string UIWidget = "slider";
    float UIMin = 0.0f;
    float UIMax = 1.0f;
    float UIStep = 0.01f;
> = 1.25f;

float Sharpness <
    string UIWidget = "slider";
    float UIMin = 0.1f;
    float UIMax = 3.0f;
    float UIStep = 0.1f;
> = 2.2f;

float2 TimeDelta = {1,.2};

//////////////////////////////////////////////////////
////////////////////////////////// pixel shaders /////
//////////////////////////////////////////////////////

QuadVertexOutput ShakerVS(
		float3 Position : POSITION, 
		float2 TexCoord : TEXCOORD0
) {
    QuadVertexOutput OUT;
    OUT.Position = float4(Position, 1);
    float2 dn = Speed*Time*TimeDelta;
	float2 off = float2(QuadTexOffset/(QuadScreenSize.x),QuadTexOffset/(QuadScreenSize.y));
    //float2 noisePos = TexCoord+off+dn;
    float2 noisePos = (float2)(0.5)+off+dn;
    float2 i = Shake*float2(vertex_noise(noisePos, NTab),
							vertex_noise(noisePos.yx, NTab));
    i = sign(i) * pow(i,Sharpness);
    OUT.UV = TexCoord.xy+i;
    return OUT;
}

////////////////////////////////////////////////////////////
/////////////////////////////////////// techniques /////////
////////////////////////////////////////////////////////////

technique Shake {
    pass p0 {
		VertexShader = compile vs_2_0 ShakerVS();
		cullmode = none;
		ZEnable = false;
		PixelShader  = compile ps_2_0 TexQuadPS(SceneSampler);
    }
}
