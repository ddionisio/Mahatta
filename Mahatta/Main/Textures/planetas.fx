texture BaseTex <
	string ResourceName = "planetas.jpg"; >;

texture GlowTxt <
	string ResourceName = "planetas_glow.tga"; >;

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
Texture = (GlowTxt);
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
};


// draws unskinned object with one texture and one directional light.
VSTEXTURE_OUTPUT VSTexture
    (
    float4 Position : POSITION, 
    float3 Normal   : NORMAL,
    float2 TexCoord : TEXCOORD0
    )
{
    VSTEXTURE_OUTPUT Out = (VSTEXTURE_OUTPUT)0;
  
    float3 P = mul(Position, WorldView);                    // position (view space)
    float3 N = normalize(mul(Normal, (float3x3)WorldView)); // normal (view space)
   
    Out.Position = mul(float4(P, 1), Projection);   // projected position
    Out.Diffuse  = k_d;               // diffuse 
    Out.TexCoord = TexCoord;                        // texture coordinates

    return Out;    
}

// draws unskinned object with one texture and one directional light.
VSTEXTURE_OUTPUT VSTexture2
    (
    float4 Position : POSITION, 
    float3 Normal   : NORMAL,
    float2 TexCoord : TEXCOORD0
    )
{
    VSTEXTURE_OUTPUT Out = (VSTEXTURE_OUTPUT)0;
  
    float3 P = mul(Position, WorldView);                    // position (view space)
    float3 N = normalize(mul(Normal, (float3x3)WorldView)); // normal (view space)
   
    Out.Position = mul(float4(P, 1), Projection);   // projected position
    Out.Diffuse  = k_d;               // diffuse 
    Out.TexCoord = TexCoord;                        // texture coordinates

    Out.Diffuse.w = sin(t*3);
    
    return Out;    
}

technique Level_Cool {
pass P0 {
cullmode = ccw;
Lighting = FALSE;
Sampler[0] = (Sampler);
ColorOp[0] = SELECTARG1;
ColorArg1[0] = TEXTURE;
AlphaOp[0] = DISABLE;

VertexShader = compile vs_1_1 VSTexture();
PixelShader = NULL; } 

pass P1 {
Sampler[0] = (Sampler2);
ColorOp[0] = MODULATE;
ColorArg1[0] = CURRENT;
ColorArg2[0] = TEXTURE;
AlphaOp[0] = MODULATE;
AlphaArg1[0] = CURRENT;
AlphaArg2[0] = TEXTURE;

VertexShader = compile vs_1_1 VSTexture2();
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
ColorArg2[0] = DIFFUSE;
AlphaOp[0] = DISABLE;
VertexShader = NULL;
PixelShader = NULL; } }