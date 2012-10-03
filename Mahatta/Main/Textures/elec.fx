bool transparent=true;

texture BaseTex <
	string ResourceName = "elec.jpg"; >;

texture BaseTex2 <
	string ResourceName = "elec_2.jpg"; >;

float t : TIMEELAPSE;

// transforms
float4x4 Projection : PROJECTION;
float4x3 WorldView  : WORLDVIEW;

sampler Sampler = sampler_state {
Texture = (BaseTex);
MipFilter = LINEAR;
MinFilter = LINEAR;
MagFilter = LINEAR; };

sampler Sampler2 = sampler_state {
Texture = (BaseTex2);
MipFilter = LINEAR;
MinFilter = LINEAR;
MagFilter = LINEAR; };

float4 k_d = { 0.3f, 0.3f, 0.75f, 1.0f };

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
    float2 TexCoord : TEXCOORD0
    )
{
    VSTEXTURE_OUTPUT Out = (VSTEXTURE_OUTPUT)0;
  
    float3 P = mul(Position, WorldView);                    // position (view space)
   
    Out.Position = mul(float4(P, 1), Projection);   // projected position
    Out.Diffuse  = k_d;               // diffuse 
    Out.TexCoord = TexCoord;                        // texture coordinates
    Out.TexCoord.x += t*0.75;
    Out.TexCoord2 = TexCoord;                        // texture coordinates
    Out.TexCoord2.x += t;

    return Out;    
}

technique normal {
pass P0 {
srcblend=one;
destblend=one;
cullmode = none;
ZWRITEENABLE = false;
Sampler[0] = (Sampler);
ColorOp[0] = MODULATE;
ColorArg1[0] = TEXTURE;
ColorArg2[0] = CURRENT;
AlphaOp[0] = MODULATE;
AlphaArg1[0] = TEXTURE;
AlphaArg2[0] = CURRENT;
Sampler[1] = (Sampler2);
ColorOp[1] = ADD;
ColorArg1[1] = CURRENT;
ColorArg2[1] = TEXTURE;
AlphaOp[1] = ADD;
AlphaArg1[1] = CURRENT;
AlphaArg2[1] = TEXTURE;
VertexShader = compile vs_1_1 VSTexture();
PixelShader = NULL; } }
