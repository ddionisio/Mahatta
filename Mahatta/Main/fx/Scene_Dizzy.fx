/*********************************************************************NVMH3****
File:  $Id: Scene_Dizzy.fx,v 1.1 2004/06/04 00:58:38 david Exp $

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
<clear color="0,0,0" depth="1.0"/>
<draw type="objects"/>
<setcolortarget texture="DTex0"/>
<settechnique name="Shake0"/>
<draw type="buffer"/>
<setcolortarget texture="DTex1"/>
<settechnique name="Shake1"/>
<draw type="buffer"/>
<setcolortarget texture="DTex2"/>
<settechnique name="Shake2"/>
<draw type="buffer"/>
<setcolortarget texture="DTex3"/>
<settechnique name="Shake3"/>
<draw type="buffer"/>
<setcolortarget texture="DTex4"/>
<settechnique name="Shake4"/>
<draw type="buffer"/>
<setcolortarget/>
<settechnique name="Combine"/>
<draw type="buffer"/>
</scene_commands>
******************************************************************************/

#include "Quad.fxh"
#include "vertex_noise.fxh"

DECLARE_QUAD_TEX(SceneTexture,SceneSampler,"X8R8G8B8")
DECLARE_QUAD_TEX(DTex0,DSamp0,"X8R8G8B8")
DECLARE_QUAD_TEX(DTex1,DSamp1,"X8R8G8B8")
DECLARE_QUAD_TEX(DTex2,DSamp2,"X8R8G8B8")
DECLARE_QUAD_TEX(DTex3,DSamp3,"X8R8G8B8")
DECLARE_QUAD_TEX(DTex4,DSamp4,"X8R8G8B8")

QUAD_REAL Time : TIMEELAPSE <string UIWidget="None";>;

///////////////////////////////////////////////////////////
/////////////////////////////////////// Tweakables ////////
///////////////////////////////////////////////////////////

float Speed <
    string UIWidget = "slider";
    float UIMin = -1.0f;
    float UIMax = 100.0f;
    float UIStep = 0.01f;
> = 21.f;

float TimeSpacing <
    string UIWidget = "slider";
    float UIMin = 0.0f;
    float UIMax = 0.2f;
    float UIStep = 0.01f;
> = 0.1f;

float Shake <
    string UIWidget = "slider";
    float UIMin = 0.0f;
    float UIMax = 1.0f;
    float UIStep = 0.01f;
> = 0.25f;

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
		float2 TexCoord : TEXCOORD0,
		uniform float TimeOff
) {
    QuadVertexOutput OUT;
    OUT.Position = float4(Position, 1);
    float2 dn = Speed*(Time+TimeOff*TimeSpacing)*TimeDelta;
	float2 off = float2(QuadTexOffset/(QuadScreenSize.x),QuadTexOffset/(QuadScreenSize.y));
    //float2 noisePos = TexCoord+off+dn;
    float2 noisePos = (float2)(0.5)+off+dn;
    float2 i = Shake*float2(vertex_noise(noisePos, NTab),
							vertex_noise(noisePos.yx, NTab));
    i = sign(i) * pow(i,Sharpness);
    OUT.UV = TexCoord.xy+i;
    return OUT;
}

#define C0 1.0
#define C1 2.0
#define C2 4.0
#define C3 8.0
#define C4 16.0

#define CS (C0+C1+C2+C3+C4)

QUAD_REAL4 CombinePS(QuadVertexOutput IN) : COLOR
{   
	QUAD_REAL4 texCol0 = tex2D(DSamp0, IN.UV)*(C0/CS);
	QUAD_REAL4 texCol1 = tex2D(DSamp1, IN.UV)*(C1/CS);
	QUAD_REAL4 texCol2 = tex2D(DSamp2, IN.UV)*(C2/CS);
	QUAD_REAL4 texCol3 = tex2D(DSamp3, IN.UV)*(C3/CS);
	QUAD_REAL4 texCol4 = tex2D(DSamp4, IN.UV)*(C4/CS);
	return (texCol0+texCol1+texCol2+texCol3+texCol4);
}  

////////////////////////////////////////////////////////////
/////////////////////////////////////// techniques /////////
////////////////////////////////////////////////////////////

technique Shake0 {
    pass p0 {
		VertexShader = compile vs_2_0 ShakerVS(0);
		cullmode = none;
		ZEnable = false;
		PixelShader  = compile ps_2_0 TexQuadPS(SceneSampler);
    }
}

technique Shake1 {
    pass p0 {
		VertexShader = compile vs_2_0 ShakerVS(1);
		cullmode = none;
		ZEnable = false;
		PixelShader  = compile ps_2_0 TexQuadPS(SceneSampler);
    }
}

technique Shake2 {
    pass p0 {
		VertexShader = compile vs_2_0 ShakerVS(2);
		cullmode = none;
		ZEnable = false;
		PixelShader  = compile ps_2_0 TexQuadPS(SceneSampler);
    }
}

technique Shake3 {
    pass p0 {
		VertexShader = compile vs_2_0 ShakerVS(3);
		cullmode = none;
		ZEnable = false;
		PixelShader  = compile ps_2_0 TexQuadPS(SceneSampler);
    }
}

technique Shake4 {
    pass p0 {
		VertexShader = compile vs_2_0 ShakerVS(4);
		cullmode = none;
		ZEnable = false;
		PixelShader  = compile ps_2_0 TexQuadPS(SceneSampler);
    }
}

technique Combine {
    pass p0 {
		VertexShader = compile vs_2_0 ScreenQuadVS();
		cullmode = none;
		ZEnable = false;
		PixelShader  = compile ps_2_0 CombinePS();
    }
}
