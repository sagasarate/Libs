texture TexLay0;
texture TexLay1;
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
		BlendOp = Add;						
		SrcBlend = SrcAlpha;				
		DestBlend = InvSrcAlpha;			
		Texture[0] = <TexLay0>;				
		ColorOp[0] = Modulate;				
		ColorArg1[0] = Texture;				
		ColorArg2[0] = Diffuse;				
		AlphaOp[0] = Modulate;				
		AlphaArg1[0] = Texture;				
		AlphaArg2[0] = diffuse;				
		ColorOp[1] = disable;				
		AlphaOp[1] = disable;				
		AddressU[0] = clamp;				
		AddressV[0] = clamp;				
		MinFilter[0] = linear;				
		MagFilter[0] = linear;				
		MipFilter[0] = linear;				
		VertexShader = NULL;				
		PixelShader  = NULL;				
    }										
}											
