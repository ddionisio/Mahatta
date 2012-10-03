/*
<scene_commands>

<draw type="objects"/>

<settechnique name="WireFrame"/>
<draw type="objects_override"/>

</scene_commands>
*/


float4x4 World : WORLD;
float4x4 View : VIEW;
float4x4 Projection : PROJECTION;

technique WireFrame {
	pass P0 {

        VertexShader = NULL;
        PixelShader  = NULL;

	cullmode = none;
	Lighting = False;
	zenable = false;
	fillmode = wireframe;

	WorldTransform[0] = (World);
	ViewTransform = (View);
	ProjectionTransform = (Projection);

	MaterialAmbient = {1.0, 1.0, 1.0, 1.0}; 
        MaterialDiffuse = {1.0, 1.0, 1.0, 1.0}; 
        MaterialSpecular = {1.0, 1.0, 1.0, 1.0}; 
        MaterialPower = 40.0f;
        
        // texture
        Texture[0] = 0;

	AlphaBlendEnable = FALSE;

	}
}