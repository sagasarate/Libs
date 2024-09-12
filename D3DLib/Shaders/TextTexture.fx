texture TexLay0 < string name = "test.jpg"; >;
texture TexLay1 < string name = "test1.jpg"; >;
technique tec0
{
    pass p0
    {
		MultiSampleAntialias = FALSE;
		Lighting=false;
		zenable = false;
		zwriteenable = false;
		CullMode = ccw;
		fogenable = false;
		Texture[0] = <TexLay0>;
		AlphaTestEnable = false;
		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		Texture[0] = <TexLay0>;
     	ColorOp[0] = SelectArg2;
       ColorArg1[0] = Texture;
       ColorArg2[0] = Diffuse;      	
       AlphaOp[0] = SelectArg2;
       AlphaArg1[0] = Texture;
       AlphaArg2[0] = diffuse;
		ColorOp[1] = disable;
		AlphaOp[1] = disable;
		AddressU[0] = clamp;
		AddressV[0] = clamp;      
		MinFilter[0] = none;
       MagFilter[0] = none;
       MipFilter[0] = none;
		VertexShader = NULL;
		PixelShader  = NULL;
    }
}