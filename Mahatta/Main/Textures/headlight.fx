texture BaseTex <
	string ResourceName = "headlight.jpg"; >;
texture ShinyTex <
	string ResourceName = "metal_shiny.jpg"; >;
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

TEXCOORDINDEX[0] = PASSTHRU;

Sampler[1] = (Sampler2);
ColorOp[1] = ADD;
ColorArg1[1] = CURRENT;
ColorArg2[1] = TEXTURE;
AlphaOp[1] = ADD;
AlphaArg1[1] = CURRENT;
AlphaArg2[1] = TEXTURE;

TEXCOORDINDEX[1] = SPHEREMAP;

VertexShader = NULL;
PixelShader = NULL; } }