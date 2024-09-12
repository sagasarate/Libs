#include "BuildIn\SharedLib.fx"


texture TexLay0 ;
texture TexLay1 ;

float4			GlobalColor=1.0f;

sampler Sampler0 =
sampler_state
{
    Texture = <TexLay0>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = WRAP;
    AddressV = WRAP;

};

sampler Sampler1 =
sampler_state
{
    Texture = <TexLay1>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = WRAP;
    AddressV = WRAP;

};



///////////////////////////////////////////////////////
struct VS_INPUT
{
    float4  Pos             : POSITION;
    float4  Diffuse			: COLOR0;
    float2  Tex0            : TEXCOORD0;    
};

struct VS_OUTPUT
{
    float4  Pos			: POSITION;
    float4  Diffuse		: COLOR0;
    float3  Tex0		: TEXCOORD0;   
};


float4 PShaderWithNormal( float4 Diffuse : COLOR0,           
           float3 Tex0 : TEXCOORD0) : COLOR0
{	
	float4 Color=tex2D( Sampler0, Tex0.xy )*Diffuse;
    Color.xyz=Color.xyz*(1.0f-Tex0.z)+FogColor*Tex0.z;
    Color=Color*GlobalColor;
	return Color;
}


VS_OUTPUT VShaderNormal(VS_INPUT Input)
{
    VS_OUTPUT   Output;
    
    float3 Pos = Input.Pos.xyz;
    
    Output.Diffuse = Input.Diffuse * <DiffuseFunction>;
    
    Pos = mul(float4(Pos.xyz, 1.0f), ViewMatrix);
    
    Output.Tex0.z = saturate(((Pos.z-FogNear)/(FogFar-FogNear)));
    
    Output.Pos = mul(float4(Pos.xyz, 1.0f), PrjMatrix);    

	
    Output.Tex0.xy  = Input.Tex0; 

    return Output;
}


technique Tec0
{
    pass p0
    { 	   
		zenable = True;
		zwriteenable = <EnableZWrite>;
		fogenable = False;
		CullMode = <CullMode>;
		
		AlphaBlendEnable = <EnableAlphaBlend>;
		BlendOp = <BlendOp>;
		SrcBlend = <SrcBlend>;
		DestBlend = <DestBlend>;
		
		AlphaTestEnable = <EnableAlphaTest>;
        AlphaRef = 0x10;
        AlphaFunc = GreaterEqual;
			
        
        VertexShader = compile vs_2_0 VShaderNormal();
        PixelShader  = compile ps_2_0 PShaderWithNormal();

    }
}

