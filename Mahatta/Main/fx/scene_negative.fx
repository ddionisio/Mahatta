/*********************************************************************NVMH3****

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
This scene clears the background to a color
The preview technique before is for the media panel, and simply draws the texture decal



<scene_commands>
<setcolortarget texture="SceneTarget"/>

<clear color="1.0,.23,.23" depth="1.0"/>
<draw type ="objects"/>
<setcolortarget/>
<setdepthstenciltarget/>
<settechnique name="Negative"/>
<draw type="buffer"/>
</scene_commands>
******************************************************************************/

texture SceneTarget : RENDERCOLORTARGET < 
    float2 ViewPortDimensions = {1.0,1.0};
    int MIPLEVELS = 1;
    string format = "X8R8G8B8";
	string UIWidget = "None";
>;
texture DepthTarget : RENDERDEPTHSTENCILTARGET < 
    float2 ViewPortDimensions = {1.0,1.0};
    string format = "D24S8";
    string UIWidget = "None";
>;

sampler SceneTarget_Sampler = sampler_state {
    texture = <SceneTarget>;
    AddressU  = CLAMP;        
    AddressV  = CLAMP;
    AddressW  = CLAMP;
    MIPFILTER = NONE;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};

struct VS_OUTPUT
{
   	float4 Position   : POSITION;
    float4 Diffuse    : COLOR0;
    float4 TexCoord0   : TEXCOORD0;
};

// Accepts positions in screen space and outputs them
VS_OUTPUT VS_Quad(float3 Position : POSITION, 
			float3 TexCoord : TEXCOORD0)
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;
    OUT.Position = float4(Position, 1);
    OUT.TexCoord0 = float4(TexCoord, 1); 
    return OUT;
}

float4 PS_Negative(VS_OUTPUT IN) : COLOR
{   
	float4 tex = tex2D(SceneTarget_Sampler, float2(IN.TexCoord0.x, IN.TexCoord0.y));
	tex = 1-tex;
	return tex;
}  

float4 PS_Copy(VS_OUTPUT IN) : COLOR
{   
	float4 tex = tex2D(SceneTarget_Sampler, float2(IN.TexCoord0.x, IN.TexCoord0.y));
	return tex;
}  

////////////////////////////////////////////////

technique Negative {
	pass p0 {
ColorOp[0]   = MODULATE;
        ColorArg1[0] = TEXTURE;
        ColorArg2[0] = CURRENT;

        ColorOp[1]   = DISABLE;

	AlphaBlendEnable = FALSE;

		VertexShader = compile vs_1_1 VS_Quad();
		PixelShader = compile ps_1_1 PS_Negative();
		ZEnable = false;
		ZWriteEnable = false;
		CullMode = None;
	}
}

technique SceneCopy {
	pass p0
	{
		VertexShader = compile vs_1_1 VS_Quad();
		PixelShader = compile ps_1_1 PS_Copy();
		ZEnable = false;
		ZWriteEnable = false;
		CullMode = None;
	}
}
