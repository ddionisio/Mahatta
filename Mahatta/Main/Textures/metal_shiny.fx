texture BaseTex <
	string ResourceName = "metal.jpg"; >;
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
cullmode = ccw;
Lighting = TRUE;
Sampler[0] = (Sampler);
ColorOp[0] = SELECTARG1;
ColorArg1[0] = TEXTURE;
AlphaOp[0] = SELECTARG1;
AlphaArg1[0] = TEXTURE;

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