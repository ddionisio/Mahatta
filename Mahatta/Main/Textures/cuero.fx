texture BaseTex <
	string ResourceName = "cuero.jpg"; >;
texture ShinyTex <
	string ResourceName = "chrome_env2.tga"; >;
float4x4 World : WORLD;
float4x4 View : VIEW;
float4x4 Projection : PROJECTION;

sampler Sampler = sampler_state {
Texture = (BaseTex);
MipFilter = LINEAR;
MinFilter = LINEAR;
MagFilter = LINEAR; };

sampler Sampler2 = sampler_state {
Texture = (ShinyTex);
MipFilter = LINEAR;
MinFilter = LINEAR;
MagFilter = LINEAR; };

technique Level_MultiTex {
pass P0 {
WorldTransform[0] = (World);
ViewTransform = (View);
ProjectionTransform = (Projection);
DIFFUSEMATERIALSOURCE=COLOR1;
cullmode = ccw;
Lighting = FALSE;
Sampler[0] = (Sampler);
ColorOp[0] = MODULATE;
ColorArg1[0] = TEXTURE;
ColorArg2[0] = DIFFUSE;
AlphaOp[0] = MODULATE;
AlphaArg1[0] = TEXTURE;
AlphaArg2[0] = DIFFUSE;

ColorOp[1] = DISABLE;
AlphaOp[1] = DISABLE;

TEXCOORDINDEX[0] = PASSTHRU;

VertexShader = NULL;
PixelShader = NULL; } 

pass P1 {
WorldTransform[0] = (World);
ViewTransform = (View);
ProjectionTransform = (Projection);

cullmode = ccw;
Lighting = FALSE;
Sampler[0] = (Sampler2);
ColorOp[0] = MODULATE2X;
ColorArg1[0] = CURRENT;
ColorArg2[0] = TEXTURE;
AlphaOp[0] = MODULATE;
AlphaArg1[0] = CURRENT;
AlphaArg2[0] = TEXTURE;

TEXCOORDINDEX[0] = SPHEREMAP;
 } }

technique normal {
pass P0 {
WorldTransform[0] = (World);
ViewTransform = (View);
ProjectionTransform = (Projection);
DIFFUSEMATERIALSOURCE=COLOR1;
cullmode = ccw;
Lighting = FALSE;
Sampler[0] = (Sampler);
ColorOp[0] = MODULATE;
ColorArg1[0] = TEXTURE;
ColorArg2[0] = DIFFUSE;
AlphaOp[0] = MODULATE;
AlphaArg1[0] = TEXTURE;
AlphaArg2[0] = DIFFUSE;
Sampler[1] = (Sampler2);
ColorOp[1] = MODULATE;
ColorArg1[1] = TEXTURE;
ColorArg2[1] = CURRENT;
AlphaOp[1] = MODULATE;
AlphaArg1[1] = CURRENT;
AlphaArg2[1] = TEXTURE;
VertexShader = NULL;
PixelShader = NULL; } }