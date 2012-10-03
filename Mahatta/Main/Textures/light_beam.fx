bool transparent=true;

texture BaseTex <
	string ResourceName = "light_beam.jpg"; >;

// transforms
float4x4 Projection : PROJECTION;
float4x3 WorldView  : WORLDVIEW;

sampler Sampler = sampler_state {
Texture = (BaseTex);
ADDRESSU=CLAMP;
ADDRESSV=CLAMP;
MipFilter = LINEAR;
MinFilter = LINEAR;
MagFilter = LINEAR; };

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
    float2 TexCoord : TEXCOORD0
    )
{
    VSTEXTURE_OUTPUT Out = (VSTEXTURE_OUTPUT)0;
  
    float3 P = mul(Position, WorldView);                    // position (view space)
   
    Out.Position = mul(float4(P, 1), Projection);   // projected position
    Out.Diffuse  = k_d;               // diffuse 
    Out.TexCoord = TexCoord;                        // texture coordinates

    return Out;    
}

technique normal {
pass P0 {
fogenable=false;
cullmode = ccw;
ZWRITEENABLE = false;
srcblend=one;
destblend=one;
Sampler[0] = (Sampler);
ColorOp[0] = MODULATE;
ColorArg1[0] = TEXTURE;
ColorArg2[0] = CURRENT;
AlphaOp[0] = MODULATE;
AlphaArg1[0] = TEXTURE;
AlphaArg2[0] = CURRENT;
VertexShader = compile vs_1_1 VSTexture();
PixelShader = NULL; } }
