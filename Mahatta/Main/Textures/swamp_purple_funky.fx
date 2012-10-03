texture BaseTex <
	string ResourceName = "swamp_purple_crystal.jpg"; >;
texture ShinyTex <
	string ResourceName = "swamp_purple_funky.jpg"; >;
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
ColorOp[1] = MODULATE;
ColorArg1[1] = CURRENT;
ColorArg2[1] = TEXTURE;
AlphaOp[1] = MODULATE;
AlphaArg1[1] = CURRENT;
AlphaArg2[1] = TEXTURE;

TEXCOORDINDEX[1] = SPHEREMAP;//CAMERASPACEREFLECTIONVECTOR; //

VertexShader = NULL;
PixelShader = NULL; } }

technique normal {
pass P0 {
WorldTransform[0] = (World);
ViewTransform = (View);
ProjectionTransform = (Projection);
cullmode = ccw;
Lighting = FALSE;
Sampler[0] = (Sampler);
ColorOp[0] = SELECTARG1;
ColorArg1[0] = TEXTURE;
AlphaOp[0] = SELECTARG1;
AlphaArg1[0] = TEXTURE;
Sampler[1] = (Sampler2);
ColorOp[1] = MODULATE;
ColorArg1[1] = TEXTURE;
ColorArg2[1] = CURRENT;
AlphaOp[1] = MODULATE;
AlphaArg1[1] = CURRENT;
AlphaArg2[1] = TEXTURE;
VertexShader = NULL;
PixelShader = NULL; } }