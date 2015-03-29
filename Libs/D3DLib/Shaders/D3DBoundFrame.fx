technique tec0						
{									
    pass p0							
    {								
		MultiSampleAntialias = false;
		Lighting = false;			
		zenable = true;				
		zwriteenable = true;		
		CullMode = none;			
		fogenable = false;			
		AlphaTestEnable = false;	
		AlphaBlendEnable = false;	
		ColorOp[0] = SelectArg1;	
		ColorArg1[0] = Diffuse;		
		AlphaOp[0] = disable;		
		ColorOp[1] = disable;		
		AlphaOp[1] = disable;		
		VertexShader = NULL;		
		PixelShader  = NULL;		
    }								
}														