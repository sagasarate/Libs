#include "BuildIn\SharedLib.fx"

float4x4		WorldMatrix;
float4x4		WorldViewMatrix;

texture TexLay0 ;
texture TexLay1 ;

float4			GlobalColor=1.0f;

float4 WaterColorLight={0.4f,0.8f,0.4f,1.0f};
float4 WaterColorDark={0.1f,0.2f,0.1f,1.0f};


sampler Sampler0 =
sampler_state
{
    Texture = <TexLay0>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;

};

sampler Sampler1 =
sampler_state
{
    Texture = <TexLay1>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Clamp;
    AddressV = Clamp;

};



struct VS_INPUT
{
    float4  Pos             : POSITION;
    float4	Diffuse			: COLOR0;
    float2  Tex0			: TEXCOORD0;    
};

struct VS_OUTPUT
{
    float4  Pos			: POSITION;
    float4  Diffuse		: COLOR0;  
    float4  Specular	: COLOR1;
    float2  Tex0		: TEXCOORD0;
    float3  Tex1		: TEXCOORD1;
};
struct PS_INPUT
{
	float4	Diffuse		: COLOR0;
	float4  Specular	: COLOR1;
	float2	Tex0		: TEXCOORD0;
	float3	Tex1		: TEXCOORD1;
};


float4 PShader( PS_INPUT Input) : COLOR0
{
	float FogFactor=saturate(((Input.Tex1.z-FogNear)/(FogFar-FogNear)));
	float4 DepthTex = mul(float4(Input.Tex1.xyz, 1.0f),PrjMatrix);
	DepthTex.x = (DepthTex.x/DepthTex.w+1.0f)/2;
    DepthTex.y = (1.0f-DepthTex.y/DepthTex.w)/2;
    DepthTex.z = saturate((Input.Tex1.z-CameraNear)/(CameraFar-CameraNear));
    
	float Depth=tex2D( Sampler1, DepthTex.xy ).r;
	Depth=saturate((Depth-DepthTex.z)*(CameraFar-CameraNear)/5+0.6)*0.95;
	
	float4 TexColor=tex2D( Sampler0, Input.Tex0*8.0f );
	
	//Input.Diffuse.a=Input.Diffuse.a*1.5f+0.3f;
	
	//float DepthAlpha=TexColor.a*Depth;
	
	float4 Specular=Input.Specular*TexColor.a;
	
	float4 OutColor=lerp(WaterColorDark,WaterColorDark+WaterColorLight,TexColor.a);
	
	//OutColor=OutColor+TexColor;
	
	OutColor=OutColor*(Input.Diffuse+Specular);
	
	OutColor.a=Depth;
	
	OutColor.xyz=OutColor.xyz*(1.0f-FogFactor)+FogColor*FogFactor;
	
	OutColor=OutColor*GlobalColor;
	
    return OutColor;
}



VS_OUTPUT VShaderNormal(VS_INPUT Input)
{
    VS_OUTPUT   Output;
    float3      Pos = 0.0f;
        
    Pos = mul(float4(Input.Pos.xyz, 1.0f), WorldMatrix);   
    float3 Normal = normalize(mul(float3(0.0f, 1.0f, 0.0f), WorldMatrix)); 
   
    
    Output.Pos = mul(float4(Input.Pos.xyz, 1.0f), WorldViewMatrix);
    Output.Tex1 = Output.Pos;
    Output.Pos = mul(Output.Pos, PrjMatrix);

	Output.Diffuse = CaculateDiffuse(Pos,Normal);
	Output.Diffuse.a = Input.Diffuse.a;
	Output.Specular = CaculateSpecular(Pos,Normal);
    Output.Tex0 = Input.Tex0; 	
    
    

    return Output;
}

technique Tec0
{
    pass p0
    { 	   
		zenable = True;
		zwriteenable = True;
		fogenable = False;
		CullMode = none;
		
		AlphaBlendEnable = True;		
		AlphaTestEnable = False;
		
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
        
			
        
        VertexShader = compile vs_2_0 VShaderNormal();
        PixelShader  = compile ps_2_0 PShader();

    }
}

