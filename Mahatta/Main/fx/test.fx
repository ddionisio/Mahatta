float lod=500;
string lod_lvl="Normal,Low";

texture Tex0 : TEX0;

float4x4 Projection : PROJECTION;

// transforms
float4x3 WorldView  : WORLDVIEW;

float4 k_a : MATERIALAMBIENT = { 1.0f, 1.0f, 1.0f, 1.0f };
float4 k_d : MATERIALDIFFUSE = { 1.0f, 1.0f, 1.0f, 1.0f };

// glow parameters
float4 GlowColor     = float4(0.5f, 0.2f, 0.2f, 1.0f);
float4 GlowAmbient   = float4(0.2f, 0.2f, 0.0f, 0.0f);
float  GlowThickness = 1.015f;

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
    float3 Normal   : NORMAL,
    float2 TexCoord : TEXCOORD0
    )
{
    VSTEXTURE_OUTPUT Out = (VSTEXTURE_OUTPUT)0;
  
    float3 P = mul(Position, WorldView);                    // position (view space)
    float3 N = normalize(mul(Normal, (float3x3)WorldView)); // normal (view space)
   
    Out.Position = mul(float4(P, 1), Projection);   // projected position
    Out.Diffuse  = k_d + k_a;               // diffuse 
    Out.TexCoord = TexCoord;                        // texture coordinates
    
    return Out;    
}


struct VSGLOW_OUTPUT
{
    float4 Position : POSITION;
    float4 Diffuse  : COLOR;
};

// draws a transparent hull of the unskinned object.
VSGLOW_OUTPUT VSGlow
    (
    float4 Position : POSITION, 
    float3 Normal   : NORMAL
    )
{
    VSGLOW_OUTPUT Out = (VSGLOW_OUTPUT)0;

    float3 N = normalize(mul(Normal, (float3x3)WorldView));     // normal (view space)
    float3 P = mul(Position, WorldView) + GlowThickness * N;    // displaced position (view space)
    float3 A = float3(0, 0, 1);                                 // glow axis

    float Power;

    Power  = dot(N, A);
    Power *= Power;
    Power -= 1;
    Power *= Power;     // Power = (1 - (N.A)^2)^2 [ = ((N.A)^2 - 1)^2 ]
    
    Out.Position = mul(float4(P, 1), Projection);   // projected position
    Out.Diffuse  = GlowColor * Power + GlowAmbient; // modulated glow color + glow ambient
    
    return Out;    
}

technique Normal {
	pass P0 {
		// single texture/one directional light shader
        VertexShader = compile vs_1_1 VSTexture();
        PixelShader  = NULL;

	cullmode = cw;
        
        // texture
        Texture[0] = (Tex0);

        // sampler states
        MinFilter[0] = LINEAR;
        MagFilter[0] = LINEAR;
        MipFilter[0] = POINT;

        // set up texture stage states for single texture modulated by diffuse 
        ColorOp[0]   = MODULATE;
        ColorArg1[0] = TEXTURE;
        ColorArg2[0] = CURRENT;

        ColorOp[1]   = DISABLE;

	AlphaBlendEnable = FALSE;

	}

	pass p1
    {   
        // glow shader
        VertexShader = compile vs_1_1 VSGlow();
        PixelShader  = NULL;
        
        // no texture
        Texture[0] = NULL;

        // enable alpha blending
        AlphaBlendEnable = TRUE;
        SrcBlend         = ONE;
        DestBlend        = ONE;

        // set up texture stage states to use the diffuse color
        ColorOp[0]   = SELECTARG2;
        ColorArg2[0] = DIFFUSE;
        AlphaOp[0]   = SELECTARG2;
        AlphaArg2[0] = DIFFUSE;

        ColorOp[1]   = DISABLE;
        AlphaOp[1]   = DISABLE;
    }

}

technique Low {
	pass P0 {
		// single texture/one directional light shader
        VertexShader = compile vs_1_1 VSTexture();
        PixelShader  = NULL;

	fvf = XYZ | Normal | Tex1;
	cullmode = cw;
        
        // texture
        Texture[0] = (Tex0);

        // sampler states
        MinFilter[0] = LINEAR;
        MagFilter[0] = LINEAR;
        MipFilter[0] = POINT;

        // set up texture stage states for single texture modulated by diffuse 
        ColorOp[0]   = MODULATE;
        ColorArg1[0] = TEXTURE;
        ColorArg2[0] = CURRENT;

        ColorOp[1]   = DISABLE;

	AlphaBlendEnable = FALSE;

	}
}