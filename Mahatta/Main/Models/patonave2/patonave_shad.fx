bool transparent = true;

texture BaseTex <
	string ResourceName = "patonave_shad.png"; >;

float4x4 World : WORLD;
float4x4 View : VIEW;
float4x4 Projection : PROJECTION;
float4 k_a : MATERIALAMBIENT = { 1.0f, 1.0f, 1.0f, 1.0f };
float4 k_d : MATERIALDIFFUSE = { 1.0f, 1.0f, 1.0f, 1.0f };
float4 k_s : MATERIALSPECULAR= { 1.0f, 1.0f, 1.0f, 1.0f };
int    n   : MATERIALPOWER = 32;

sampler Sampler = sampler_state {
Texture = (BaseTex);
MipFilter = LINEAR;
MinFilter = LINEAR;
MagFilter = LINEAR; };

technique Normal {
	pass P0 
	{
		WorldTransform[0] = (World);
		ViewTransform = (View);
		ProjectionTransform = (Projection);
		MaterialAmbient  = (k_a);
		MaterialDiffuse  = (k_d);
		MaterialSpecular = (k_s);
		MaterialPower = (n);
		cullmode = cw;
		ZWRITEENABLE = false;

		Sampler[0] = (Sampler);
		ColorOp[0] = MODULATE;
		ColorArg1[0] = TEXTURE;
		ColorArg2[0] = DIFFUSE;
		AlphaOp[0] = MODULATE;
		AlphaArg1[0] = TEXTURE;
		AlphaArg2[0] = DIFFUSE;
		ColorOp[1] = DISABLE;
		AlphaOp[1] = DISABLE;
		VertexShader = NULL;
		PixelShader = NULL; 
	} 
}