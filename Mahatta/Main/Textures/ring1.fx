bool transparent=true;

texture BaseTex < string ResourceName = "ring1.tga"; >;

// transforms
float4x4 World : WORLD;
float4x4 View : VIEW;
float4x4 Projection : PROJECTION;

sampler Sampler = sampler_state {
Texture = (BaseTex);
ADDRESSU=CLAMP;
ADDRESSV=CLAMP; };

float4 k_d = { 1.0f, 1.0f, 1.0f, 1.0f };


struct VSTEXTURE_OUTPUT
{
    float4 Position : POSITION;
    float4 Diffuse  : COLOR;
    float2 TexCoord : TEXCOORD0;
};

technique normal {
pass P0 {
srcblend=one;
destblend=one;
cullmode = none;
WorldTransform[0] = (World);
ViewTransform = (View);
ProjectionTransform = (Projection);
//fogenable=false;
Lighting = FALSE;
ZWRITEENABLE = false;
Sampler[0] = (Sampler);
ColorOp[0] = MODULATE;
ColorArg1[0] = TEXTURE;
ColorArg2[0] = DIFFUSE;
AlphaOp[0] = MODULATE;
AlphaArg1[0] = TEXTURE;
AlphaArg2[0] = DIFFUSE;
VertexShader = null;
PixelShader = NULL; } }
