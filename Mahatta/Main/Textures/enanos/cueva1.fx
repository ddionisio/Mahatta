bool transparent=true;

texture BaseTex <
	string ResourceName = "cueva1.tga"; >;

texture ShadowTex : TEX1;

// transforms
float4x4 Projection : PROJECTION;
float4x3 WorldView  : WORLDVIEW;

sampler Sampler = sampler_state {
Texture = (BaseTex);
MipFilter = POINT;
MinFilter = POINT;
MagFilter = POINT; };

sampler Sampler2 = sampler_state {
Texture = (ShadowTex);
MipFilter = POINT;
MinFilter = POINT;
MagFilter = POINT; };

float4 k_d = { 1.0f, 1.0f, 1.0f, 1.0f };

struct VSTEXTURE_OUTPUT
{
    float4 Position : POSITION;
    float4 Diffuse  : COLOR;
    float2 TexCoord : TEXCOORD0;
    float2 TexCoord2 : TEXCOORD1;
};

// draws unskinned object with one texture and one directional light.
VSTEXTURE_OUTPUT VSTexture
    (
    float4 Position : POSITION, 
    float2 TexCoord : TEXCOORD0,
    float2 TexCoord2 : TEXCOORD1
    )
{
    VSTEXTURE_OUTPUT Out = (VSTEXTURE_OUTPUT)0;
  
    float3 P = mul(Position, WorldView);                    // position (view space)
   
    Out.Position = mul(float4(P, 1), Projection);   // projected position
    Out.Diffuse  = k_d;               // diffuse 
    Out.TexCoord = TexCoord;                        // texture coordinates
    Out.TexCoord2 = TexCoord2;                        // texture coordinates

    return Out;    
}

technique normal {
pass P0 {
cullmode = none;
fogenable=false;
ZWRITEENABLE = false;
Sampler[0] = (Sampler);
ColorOp[0] = MODULATE;
ColorArg1[0] = TEXTURE;
ColorArg2[0] = DIFFUSE;
AlphaOp[0] = MODULATE;
AlphaArg1[0] = TEXTURE;
AlphaArg2[0] = DIFFUSE;
Sampler[1] = (Sampler2);
ColorOp[1] = MODULATE;
ColorArg1[1] = CURRENT;
ColorArg2[1] = TEXTURE;
AlphaOp[1] = MODULATE;
AlphaArg1[1] = CURRENT;
AlphaArg2[1] = TEXTURE;
VertexShader = compile vs_1_1 VSTexture();
PixelShader = NULL; } }
