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
		AlphaTestEnable = false;				
		AlphaBlendEnable = true;				
		SrcBlend = InvDestColor;				
		DestBlend = InvDestColor;				
		ColorOp[0] = SelectArg1;				
		ColorArg1[0] = Diffuse;					
		AlphaOp[0] = SelectArg1;					
		AlphaArg1[0] = diffuse;					
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