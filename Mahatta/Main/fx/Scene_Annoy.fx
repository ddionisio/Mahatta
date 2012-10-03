/*********************************************************************NVMH3****
File:  $Id: Scene_Annoy.fx,v 1.1 2004/06/04 00:58:38 david Exp $

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
<setdepthstenciltarget texture="SceneDepth"/>
<clear color="0,0,0" depth="1.0"/>
<draw type="objects"/>
<setcolortarget/>
<setdepthstenciltarget/>
<settechnique name="Desat"/>
<draw type="buffer"/>
</scene_commands>
******************************************************************************/

#include "Quad.fxh"

DECLARE_QUAD_TEX(SceneTexture,SceneSampler,"X8R8G8B8")
DECLARE_QUAD_DEPTH_BUFFER(SceneDepth,"D24S8")

QUAD_REAL4 MouseL : LEFTMOUSEDOWN < string UIWidget="None"; >;
// QUAD_REAL4 MouseR : RIGHTMOUSEDOWN < string UIWidget="None"; >;
QUAD_REAL3 MousePos : MOUSEPOSITION < string UIWidget="None"; >;
QUAD_REAL Time : TIMEELAPSE < string UIWidget = "None"; >;

///////////////////////////////////////////////////////////
/////////////////////////////////////// Tweakables ////////
///////////////////////////////////////////////////////////

QUAD_REAL Speed <
    string UIWidget = "slider";
    QUAD_REAL UIMin = -1.0f;
    QUAD_REAL UIMax = 1.0f;
    QUAD_REAL UIStep = 0.01f;
> = 0.5f;

QUAD_REAL Speed2 <
    string UIWidget = "slider";
    QUAD_REAL UIMin = 0.0f;
    QUAD_REAL UIMax = 10.0f;
    QUAD_REAL UIStep = 0.01f;
> = 0.2f;

QUAD_REAL Pulse <
    string UIWidget = "slider";
    QUAD_REAL UIMin = 0.05f;
    QUAD_REAL UIMax = 0.95f;
    QUAD_REAL UIStep = 0.01f;
> = 0.15f;

QUAD_REAL PulseE <
    string UIWidget = "slider";
    QUAD_REAL UIMin = 0.1f;
    QUAD_REAL UIMax = 5.0f;
    QUAD_REAL UIStep = 0.01f;
> = 0.5f;

QUAD_REAL CenterX <
    string UIWidget = "slider";
    QUAD_REAL UIMin = 0.0f;
    QUAD_REAL UIMax = 1.0f;
    QUAD_REAL UIStep = 0.01f;
> = 0.5f;

QUAD_REAL CenterY <
    string UIWidget = "slider";
    QUAD_REAL UIMin = 0.0f;
    QUAD_REAL UIMax = 1.0f;
    QUAD_REAL UIStep = 0.01f;
> = 0.5f;


//////////////////////////////////////////////////////
////////////////////////////////// pixel shaders /////
//////////////////////////////////////////////////////

QuadVertexOutput AnnoyVS(
		QUAD_REAL3 Position : POSITION, 
		QUAD_REAL3 TexCoord : TEXCOORD0
) {
    QuadVertexOutput OUT;
	QUAD_REAL r = Time*Speed;	// radians
	r *= (2.0 * (MouseL.z-0.5));
	float2 cs = float2(sin(r),cos(r));
	r = 2.0*(pow(0.5*(sin(Speed2*Time)+1.0),PulseE)-0.5);
	r = 1 + Pulse*r;
    OUT.Position = QUAD_REAL4(Position, 1);
	QUAD_REAL2 off = QUAD_REAL2(QuadTexOffset/(QuadScreenSize.x),QuadTexOffset/(QuadScreenSize.y));
    QUAD_REAL2 ctr = QUAD_REAL2(CenterX,CenterY);
    QUAD_REAL2 t = r*(QUAD_REAL2(TexCoord.xy+off) - ctr);
    OUT.UV = (((QUAD_REAL2((t.x*cs.x - t.y*cs.y), 
						(t.x*cs.y + t.y*cs.x)))) + ctr);
    return OUT;
}

////////////////////////////////////////////////////////////
/////////////////////////////////////// techniques /////////
////////////////////////////////////////////////////////////

technique Annoy {
    pass p0 {
	VertexShader = compile vs_2_0 AnnoyVS();
	cullmode = none;
	ZEnable = false;
	PixelShader  = compile ps_2_0 TexQuadPS(SceneSampler);
    }
}
