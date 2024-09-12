#include "BuildIn\SharedLib.fx"

float4x4		WorldViewMatrix;

float4		GlobalColor=1.0f;


struct VS_INPUT
{
    float4  Pos             : POSITION;    
};

struct VS_OUTPUT
{
    float4  Pos			: POSITION;    
};



float4 PShader() : COLOR0
{		
    return FogColor*GlobalColor;
}


VS_OUTPUT VShader(VS_INPUT Input)
{
    VS_OUTPUT  Output;
    
    Output.Pos = mul(float4(Input.Pos.xyz, 1.0f), WorldViewMatrix);
    Output.Pos = mul(Output.Pos, PrjMatrix);  
    
    return Output;
}

technique Tec0
{
    pass p0
    { 	   
		zenable = True;
		zwriteenable = True;
		fogenable = False;
		CullMode = ccw;
				
		AlphaBlendEnable = False;		
		AlphaTestEnable = False;
        
			
        
        VertexShader = compile vs_3_0 VShader();
        PixelShader  = compile ps_3_0 PShader();

    }
}
