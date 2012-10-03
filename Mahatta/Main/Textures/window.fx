bool transparent = true;

float4x4 World : WORLD;
float4x4 View : VIEW;
float4x4 Projection : PROJECTION;

float4 k_d : MATERIALDIFFUSE = { 1.0f, 1.0f, 1.0f, 1.0f };

texture ShinyTex <
	string ResourceName = "window.jpg"; >;

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

		cullmode = ccw;
		Lighting = FALSE;

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