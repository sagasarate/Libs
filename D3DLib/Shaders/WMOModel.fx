#include "BuildIn\SharedLib.fx"

float4		GlobalColor=1.0f;

float4x4	WorldMatrix;
float4x4	WorldViewMatrix;


texture TexLay0 ;
texture TexLay1 ;


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

struct VS_INPUT
{
    float4  Pos             : POSITION;
    float4  Normal          : NORMAL;
    float4	Diffuse			: COLOR0;
    float2  Tex0            : TEXCOORD0;    
};

struct VS_OUTPUT
{
    float4  Pos			: POSITION;
    float4  Diffuse		: COLOR0;  
    float4  Tex0		: TEXCOORD0;
};

struct VS_OUTPUT_FOR_DEPTH
{
    float4  Pos			: POSITION;
    float4  Diffuse		: COLOR0; 
};

struct PS_INPUT
{
	float4	Diffuse		: COLOR0;	
    float4	Tex0		: TEXCOORD0;
    
};

struct PS_OUTPUT
{
	float4	Color				: COLOR0;
	float4	ColorForDepthMap	: COLOR1;
};


PS_OUTPUT PShaderWithNormal(PS_INPUT Input)
{	
	PS_OUTPUT Output;
	
    Output.Color=tex2D( Sampler0, Input.Tex0.xy )*Input.Diffuse;
    Output.Color.xyz=Output.Color.xyz*(1.0f-Input.Tex0.z)+FogColor*Input.Tex0.z;
    
    Output.Color=Output.Color*GlobalColor;
    
    Output.ColorForDepthMap=Input.Tex0.w;
    return Output;
}

VS_OUTPUT VShaderNormal(VS_INPUT Input)
{
    VS_OUTPUT   Output;
    
    float3 Pos = mul(float4(Input.Pos.xyz, 1.0f), WorldMatrix);
    float3 Normal = normalize(mul(Input.Normal.xyz, WorldMatrix));
    
    Output.Pos = mul(float4(Input.Pos.xyz, 1.0f), WorldViewMatrix);
    Output.Tex0.z = saturate(((Output.Pos.z-FogNear)/(FogFar-FogNear)));
    Output.Tex0.w = saturate((Output.Pos.z-CameraNear)/(CameraFar-CameraNear));
    Output.Pos = mul(Output.Pos, PrjMatrix);

	Output.Diffuse = <DiffuseOperation>;
    Output.Tex0.xy = Input.Tex0;
    
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
        AlphaRef = 0x80;
        AlphaFunc = GreaterEqual;
			
        
        VertexShader = compile vs_2_0 VShaderNormal();
        PixelShader  = compile ps_2_0 PShaderWithNormal();

    }
}

