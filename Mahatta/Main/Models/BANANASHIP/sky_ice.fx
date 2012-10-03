/*********************************************************************NVMH3****
File:  $Id: sky_ice.fx,v 1.1 2004/06/08 04:23:07 david Exp $

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

******************************************************************************/

float4x4 World : World < string UIWidget="None"; >;	// World or Model matrix
float4x4 WorldViewProj : WorldViewProjection < string UIWidget="None"; >;	// Model*View*Projection

texture cubeMap : ENVIRONMENT
<
	string ResourceName = "sky_ice_cube.dds";
	string TextureType = "Cube";
>;

/////////////////////

technique SkyBox
{
	pass p0
	{
		VertexShaderConstant[0] = <World>; 		// c0-c3 is ModelView matrix
        VertexShaderConstant[4] = <WorldViewProj>;			// c4-c7 is ModelViewProjection matrix
		VertexShader = 
		asm
        {
            vs.1.1					// version number
            dcl_position v0
            //dcl_normal v3
            //dcl_texcoord0 v7
            //dcl_tangent0 v8
            //dcl_binormal0 v9
            //dcl_color0 v5

			
			m4x4 oPos, v0, c4		// pos in screen space.
			mov  oT0, v0
			/*
			mov r3, v3

			dp3 r3.w, r3, r3		// r3 = normal.
			rsq r3.w, r3.w
			mul r3, v3, r3.w

			m3x3 oT0, r3, c0
			*/
			//m3x3 oD0, -r3, c0
			//mov oD0, v0
        };
		
		
//		normalizenormals = true;
		
		Texture[0] = <cubeMap>;
		MinFilter[0] = Linear;
		MagFilter[0] = Linear;
		MipFilter[0] = Linear;

		ColorOp[0] = SelectArg1;		
		ColorArg1[0] = Texture;
		ColorArg2[0] = Current;
		AlphaOp[0] = SelectArg1;
		AlphaArg1[0] = Texture;
		AlphaArg2[0] = Current;

		ColorOp[1] = Disable;
		AlphaOp[1] = Disable;	
	}
}
