texture BaseTex <
	string ResourceName = "crazy_F1.jpg"; >;

// transforms
float4x4 Projection : PROJECTION;
float4x3 WorldView  : WORLDVIEW;

sampler Sampler = sampler_state {
Texture = (BaseTex);
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

// draws unskinned object with one texture
VSTEXTURE_OUTPUT VSTexture
    (
    float3 Position : POSITION, 
    float2 TexCoord : TEXCOORD0
    )
{
    VSTEXTURE_OUTPUT Out = (VSTEXTURE_OUTPUT)0;
  
    float3 P = mul(float4(Position, 1), WorldView);                    // position (view space)
   
    Out.Position = mul(float4(P, 1), Projection);   // projected position
   Out.Diffuse  = k_d;               		    // diffuse 
    Out.TexCoord = TexCoord;                        // texture coordinates


    return Out;    
}

technique normal {
pass P0 {
fogenable=false;
//lighting=false;
cullmode = cw;
//ZWRITEENABLE = false;
Sampler[0] = (Sampler);
ColorOp[0] = MODULATE;
ColorArg1[0] = DIFFUSE;
ColorArg2[0] = TEXTURE;
AlphaOp[0] = MODULATE;
AlphaArg1[0] = DIFFUSE;
AlphaArg2[0] = TEXTURE;

ColorOp[1] = DISABLE;
AlphaOp[1] = DISABLE;

VertexShader = compile vs_1_1 VSTexture();
PixelShader = NULL; } }
