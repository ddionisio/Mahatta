float4x4 World : WORLD;
float4x4 View : VIEW;
float4x4 Projection : PROJECTION;

float4 k_a = { 0.35f, 0.35f, 0.35f, 1.0f };
float4 k_d = { 0.0f, 0.0f, 0.0f, 1.0f };
float4 k_s = { 0.75f, 0.75f, 0.75f, 1.0f };
int    n   = 32;

texture ShinyTex <
	string ResourceName = "sky_ice_cube.dds";
	string TextureType = "Cube"; >;

texture ShinyTex2D <
	string ResourceName = "stars.jpg"; >;

sampler SamplerShiny = sampler_state {
Texture = (ShinyTex);
MipFilter = LINEAR;
MinFilter = LINEAR;
MagFilter = LINEAR; };

sampler SamplerShiny2D = sampler_state {
Texture = (ShinyTex2D);
MipFilter = LINEAR;
MinFilter = LINEAR;
MagFilter = LINEAR; };

technique crap
{
	pass p0
	{
		//alphablendenable = true;

		WorldTransform[0] = (World);
		ViewTransform = (View);
		ProjectionTransform = (Projection);

		SPECULARENABLE=TRUE;
		NORMALIZENORMALS=TRUE;
		//lighting = true;

		MaterialAmbient  = (k_a);
		MaterialDiffuse  = (k_d);
		MaterialSpecular = (k_s);
		MaterialPower = (n);

		cullmode = cw;

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

technique crap2D
{
	pass p0
	{
		//alphablendenable = true;

		WorldTransform[0] = (World);
		ViewTransform = (View);
		ProjectionTransform = (Projection);

		SPECULARENABLE=TRUE;
		NORMALIZENORMALS=TRUE;
		//lighting = true;

		MaterialAmbient  = (k_a);
		MaterialDiffuse  = (k_d);
		MaterialSpecular = (k_s);
		MaterialPower = (n);

		cullmode = cw;

		Sampler[0] = (SamplerShiny2D);
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