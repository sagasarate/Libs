technique tec0									
{												
    pass p0										
    {											
		zenable = true;							
		zwriteenable = false;					
		lighting = false;						
		CullMode = none;						
		fogenable = false;				
		AlphaTestEnable = false;				
		AlphaBlendEnable = true;				
		SrcBlend = SrcAlpha;					
		DestBlend = InvSrcAlpha;				
		ColorOp[0] = SelectArg1;				
		ColorArg1[0] = Diffuse;					
		AlphaOp[0] = SelectArg1;					
		AlphaArg1[0] = diffuse;					
		ColorOp[1] = disable;					
		AlphaOp[1] = disable;					
		VertexShader = NULL;					
		PixelShader  = NULL;					
    }											
}												