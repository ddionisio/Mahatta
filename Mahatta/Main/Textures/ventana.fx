texture BaseTex <
	string ResourceName = "window.jpg"; >;

bool transparent = true;

texture Window <
	string ResourceName = "ventana.tga"; >;

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
Texture = (Window);
MipFilter = LINEAR;
MinFilter = LINEAR;
MagFilter = LINEAR; };

sampler Sampler3 = sampler_state {
Texture = (ShadowTex);
MipFilter = LINEAR;
MinFilter = LINEAR;
MagFilter = LINEAR; };

float t : TIMEELAPSE;

float4 k_d = { 1.0f, 1.0f, 1.0f, 1.0f };


struct VSTEXTURE_OUTPUT2
{
    float4 Position : POSITION;
    float4 Diffuse  : COLOR;
    float2 TexCoord : TEXCOORD0;
    float2 TexCoord2 : TEXCOORD1;
};

// draws unskinned object with one texture and one directional light.
VSTEXTURE_OUTPUT2 VSTexture2
    (
    float4 Position : POSITION, 
    float3 Normal   : NORMAL,
    float2 TexCoord : TEXCOORD0,
    float2 TexCoord2 : TEXCOORD1
    )
{
    VSTEXTURE_OUTPUT2 Out = (VSTEXTURE_OUTPUT2)0;
  
    float3 P = mul(Position, WorldView);                    // position (view space)
    float3 N = normalize(mul(Normal, (float3x3)WorldView)); // normal (view space)
   
    Out.Position = mul(float4(P, 1), Projection);   // projected position
    Out.Diffuse  = k_d;               // diffuse 
    Out.TexCoord = TexCoord;                        // texture coordinates
    Out.TexCoord2 = TexCoord2;                        // texture coordinates

    return Out;    
}

technique Level_Cool {
pass p0
	{
		alphablendenable = true;
		ZWRITEENABLE = false;

		srcblend=one;
		destblend=one;

		WorldTransform[0] = (World);
		ViewTransform = (View);
		ProjectionTransform = (Projection);

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

		TEXCOORDINDEX[0] = CAMERASPACEREFLECTIONVECTOR;

VertexShader = NULL;
	}

pass P1 {
		srcblend=SRCALPHA;
		destblend=INVSRCALPHA;

Sampler[0] = (Sampler2);
ColorOp[0] = MODULATE;
ColorArg1[0] = CURRENT;
ColorArg2[0] = TEXTURE;
AlphaOp[0] = MODULATE;
AlphaArg1[0] = CURRENT;
AlphaArg2[0] = TEXTURE;

Sampler[1] = (Sampler3);
ColorOp[1] = MODULATE;
ColorArg1[1] = CURRENT;
ColorArg2[1] = TEXTURE;
AlphaOp[1] = MODULATE;
AlphaArg1[1] = CURRENT;
AlphaArg2[1] = TEXTURE;

TEXCOORDINDEX[0] = PASSTHRU;

VertexShader = compile vs_1_1 VSTexture2();
PixelShader = NULL; } }

technique normal {
pass P0 {
WorldTransform[0] = (World);
ViewTransform = (View);
ProjectionTransform = (Projection);
DIFFUSEMATERIALSOURCE=COLOR1;
cullmode = ccw;
Lighting = FALSE;
Sampler[0] = (Sampler2);
ColorOp[0] = MODULATE;
ColorArg1[0] = TEXTURE;
ColorArg2[0] = DIFFUSE;
AlphaOp[0] = MODULATE;
AlphaArg1[0] = TEXTURE;
AlphaArg2[0] = DIFFUSE;
VertexShader = NULL;
PixelShader = NULL; } }