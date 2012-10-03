texture BaseTex <
	string ResourceName = "crazy_FRGRT.ini"; >;

texture StaticTex <
	string ResourceName = "static.ini"; >;

texture ScanLineTex <
	string ResourceName = "scanline.jpg"; >;

texture ShadowTex : TEX1;

float4x4 World : WORLD;
float4x4 View : VIEW;
float4x4 Projection : PROJECTION;

// transforms
float4x3 WorldView  : WORLDVIEW;

sampler Sampler = sampler_state {
Texture = (BaseTex);
MipFilter = LINEAR;
MinFilter = LINEAR;
MagFilter = LINEAR; };

sampler Sampler2 = sampler_state {
Texture = (StaticTex);
MipFilter = LINEAR;
MinFilter = LINEAR;
MagFilter = LINEAR; };

sampler Sampler3 = sampler_state {
Texture = (ScanLineTex);
MipFilter = LINEAR;
MinFilter = LINEAR;
MagFilter = LINEAR; 
ADDRESSU = MIRROR;
ADDRESSV = MIRROR; };

sampler Sampler4 = sampler_state {
Texture = (ShadowTex);
MipFilter = LINEAR;
MinFilter = LINEAR;
MagFilter = LINEAR; };

float t : TIMEELAPSE;

float4 k_d = { 1.0f, 1.0f, 1.0f, 1.0f };

struct VSTEXTURE_OUTPUT
{
    float4 Position : POSITION;
    float4 Diffuse  : COLOR;
    float2 TexCoord : TEXCOORD0;
    float2 TexCoord2 : TEXCOORD1;
    float2 TexCoord3 : TEXCOORD2;
    float2 TexCoord4 : TEXCOORD3;
};

// draws unskinned object with one texture and one directional light.
VSTEXTURE_OUTPUT VSTexture
    (
    float4 Position : POSITION, 
    float3 Normal   : NORMAL,
    float2 TexCoord : TEXCOORD0,
    float2 TexCoord2 : TEXCOORD1
    )
{
    VSTEXTURE_OUTPUT Out = (VSTEXTURE_OUTPUT)0;
  
    float3 P = mul(Position, WorldView);                    // position (view space)
   
    Out.Position = mul(float4(P, 1), Projection);   // projected position
    Out.Diffuse  = k_d;               // diffuse 
    Out.TexCoord = TexCoord;                        // texture coordinates
    Out.TexCoord2 = TexCoord;                        // texture coordinates

    Out.TexCoord3 = TexCoord;                        // texture coordinates
    Out.TexCoord3.x += t*1.5;
    Out.TexCoord3.y += t;

    Out.TexCoord4 = TexCoord2;                        // texture coordinates
    
    return Out;    
}

technique Level_Cool {
pass P0 {
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
ColorOp[1] = ADD;
ColorArg1[1] = CURRENT;
ColorArg2[1] = TEXTURE;
AlphaOp[1] = ADD;
AlphaArg1[1] = CURRENT;
AlphaArg2[1] = TEXTURE;

Sampler[2] = (Sampler3);
ColorOp[2] = ADD;
ColorArg1[2] = CURRENT;
ColorArg2[2] = TEXTURE;
AlphaOp[2] = ADD;
AlphaArg1[2] = CURRENT;
AlphaArg2[2] = TEXTURE;

Sampler[3] = (Sampler4);
ColorOp[3] = MODULATE;
ColorArg1[3] = CURRENT;
ColorArg2[3] = TEXTURE;
AlphaOp[3] = MODULATE;
AlphaArg1[3] = CURRENT;
AlphaArg2[3] = TEXTURE;

VertexShader = compile vs_1_1 VSTexture();
PixelShader = NULL; } }

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
VertexShader = NULL;
PixelShader = NULL; } }