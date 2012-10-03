bool transparent = true;
texture BaseTex <string ResourceName = "slime.tga"; >;

float4x4 TransMtx : PROJECTION;
struct VS_OUTPUT
{
float4 Pos : POSITION;
float4 Diff : COLOR0;
float2 Tex : TEXCOORD0;
};
VS_OUTPUT VS(float3 Pos : POSITION, float4 clr : COLOR0, float2 Tex : TEXCOORD0) {
VS_OUTPUT Out = (VS_OUTPUT)0;
Out.Pos  = mul(float4(Pos, 1), TransMtx);
Out.Diff = clr;
Out.Tex  = Tex; return Out; }
sampler Sampler = sampler_state {
Texture = (BaseTex);
MipFilter = LINEAR;
MinFilter = LINEAR;
MagFilter = LINEAR; };
technique TVertexShaderOnly {
pass P0 {
fogenable=false;
alphablendenable = true;
cullmode = none;
ZWRITEENABLE = false;
srcblend=srcalpha;
destblend=one;
Sampler[0] = (Sampler);
ColorOp[0] = ADD;
ColorArg1[0] = TEXTURE;
ColorArg2[0] = DIFFUSE;
AlphaOp[0] = MODULATE;
AlphaArg1[0] = TEXTURE;
AlphaArg2[0] = DIFFUSE;
ColorOp[1] = DISABLE;
AlphaOp[1] = DISABLE;
VertexShader = compile vs_1_1 VS();
PixelShader = NULL; } }