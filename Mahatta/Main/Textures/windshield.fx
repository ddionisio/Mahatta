bool transparent = true;

float4x4 World : WORLD;
float4x4 View : VIEW;
float4x4 Projection : PROJECTION;

float4 k_a = { 0.5f, 0.5f, 0.5f, 1.0f };
float4 k_d = { 0.0f, 0.0f, 0.0f, 1.0f };
float4 k_s = { 0.5f, 0.5f, 0.5f, 1.0f };
int    n   = 16;

texture ShinyTex <
	string ResourceName = "windshield2.jpg"; >;

sampler SamplerShiny = sampler_state {
Texture = (ShinyTex);
MipFilter = LINEAR;
MinFilter = LINEAR;
MagFilter = LINEAR; };

technique crap
{
	pass p0
	{
		alphablendenable = true;
		ZWRITEENABLE = false;

		srcblend=one;
		destblend=one;

		WorldTransform[0] = (World);
		ViewTransform = (View);
		ProjectionTransform = (Projection);

		MaterialAmbient  = (k_a);
		MaterialDiffuse  = (k_d);
		MaterialSpecular = (k_s);
		MaterialPower = (n);

		cullmode = cw;
		Lighting = TRUE;

		Sampler[0] = (SamplerShiny);
		ColorOp[0] = MODULATE;
		ColorArg1[0] = TEXTURE;
		ColorArg2[0] = DIFFUSE;
		AlphaOp[0] = MODULATE;
		AlphaArg1[0] = TEXTURE;
		AlphaArg2[0] = DIFFUSE;
		ColorOp[1] = DISABLE;
		AlphaOp[1] = DISABLE;

		TEXCOORDINDEX[0] = CAMERASPACEREFLECTIONVECTOR;
	}

}