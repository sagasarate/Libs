#include "BuildIn\SharedLib.fx"


float4		GlobalColor=1.0f;
float4			ShadowColor=0.5f;

float4x4	WorldMatrix;
float4x4	WorldViewMatrix;

static const float TexRatio=8.0f;
static const float AlphaTexRatio=1.0f;
static const float ShadowRatio=0.5f;


texture TexLay0 ;
texture TexLay1 ;
texture TexLay2 ;
texture TexLay3 ;
texture TexLay4 ;
texture TexLay5 ;
texture TexLay6 ;
texture TexShadow ;


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
    AddressU = Wrap;
    AddressV = Wrap;

};

sampler Sampler2 =
sampler_state
{
    Texture = <TexLay2>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Clamp;
    AddressV = Clamp;

};

sampler Sampler3 =
sampler_state
{
    Texture = <TexLay3>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
};

sampler Sampler4 =
sampler_state
{
    Texture = <TexLay4>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Clamp;
    AddressV = Clamp;
};

sampler Sampler5 =
sampler_state
{
    Texture = <TexLay5>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
};

sampler Sampler6 =
sampler_state
{
    Texture = <TexLay6>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Clamp;
    AddressV = Clamp;
};

sampler SamplerShadow =
sampler_state
{
    Texture = <TexShadow>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Clamp;
    AddressV = Clamp;
};

struct VS_INPUT
{
    float4  Pos             : POSITION;
    float4  Normal          : NORMAL;
    float4	Diffuse			: COLOR0;
    float2  Tex0			: TEXCOORD0;    
};

struct VS_OUTPUT
{
    float4  Pos			: POSITION;
    float4  Diffuse		: COLOR0; 
    float4  Specular	: COLOR1; 
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
	float4	Specular	: COLOR1;
    float4	Tex0		: TEXCOORD0;
    
};

struct PS_OUTPUT
{
	float4	Color				: COLOR0;
	float4	ColorForDepthMap	: COLOR1;
};


PS_OUTPUT PShaderOneLayer(PS_INPUT Input)
{	
	PS_OUTPUT   Output;
	 
	Output.Color=tex2D( Sampler0, Input.Tex0.xy*TexRatio );
    Output.Color=Output.Color*Input.Diffuse+Input.Specular*Output.Color.a;
	Output.Color.xyz=Output.Color.xyz*(1.0f-Input.Tex0.z)+FogColor*Input.Tex0.z;
	
	Output.Color=Output.Color*GlobalColor;
	
    Output.ColorForDepthMap=Input.Tex0.w;
    return Output;
}

PS_OUTPUT PShaderOneLayerWithShadow(PS_INPUT Input)
{
	PS_OUTPUT   Output;
	
	Output.Color=tex2D( Sampler0, Input.Tex0.xy*TexRatio );
	
	float4 SHAlpha=tex2D( SamplerShadow, Input.Tex0.xy );	
	Output.Color=Output.Color*saturate(ShadowColor*ShadowRatio+(1-SHAlpha.a));
	
	
    Output.Color=Output.Color*Input.Diffuse+Input.Specular*Output.Color.a*(1-SHAlpha.a);
	Output.Color.xyz=Output.Color.xyz*(1.0f-Input.Tex0.z)+FogColor*Input.Tex0.z;
	
	Output.Color=Output.Color*GlobalColor;
	
    Output.ColorForDepthMap=Input.Tex0.w;
    return Output;
}

PS_OUTPUT PShaderTwoLayer(PS_INPUT Input)
{
	PS_OUTPUT   Output;
	
	Output.Color=tex2D( Sampler0, Input.Tex0.xy*TexRatio );
	
	float4 Color1=tex2D( Sampler1, Input.Tex0.xy*TexRatio );
	float4 Alpha1=tex2D( Sampler2, Input.Tex0.xy*AlphaTexRatio );	
	Output.Color=Output.Color*(1.0f-Alpha1.a)+Color1*Alpha1.a;
	
	Output.Color=Output.Color*Input.Diffuse+Input.Specular*Output.Color.a;
	Output.Color.xyz=Output.Color.xyz*(1.0f-Input.Tex0.z)+FogColor*Input.Tex0.z;
	
	Output.Color=Output.Color*GlobalColor;
	
    Output.ColorForDepthMap=Input.Tex0.w;
    return Output;
}

PS_OUTPUT PShaderTwoLayerWithShadow(PS_INPUT Input)
{
	PS_OUTPUT   Output;
	 	
	Output.Color=tex2D( Sampler0, Input.Tex0.xy*TexRatio );
	
	float4 Color1=tex2D( Sampler1, Input.Tex0.xy*TexRatio );
	float4 Alpha1=tex2D( Sampler2, Input.Tex0.xy*AlphaTexRatio );		
	Output.Color=Output.Color*(1.0f-Alpha1.a)+Color1*Alpha1.a;
	
	float4 SHAlpha=tex2D( SamplerShadow, Input.Tex0.xy );	
	Output.Color=Output.Color*saturate(ShadowColor*ShadowRatio+(1-SHAlpha.a));
	
	
    Output.Color=Output.Color*Input.Diffuse+Input.Specular*Output.Color.a*(1-SHAlpha.a);
	Output.Color.xyz=Output.Color.xyz*(1.0f-Input.Tex0.z)+FogColor*Input.Tex0.z;
	
	Output.Color=Output.Color*GlobalColor;
	
    Output.ColorForDepthMap=Input.Tex0.w;
    return Output;
}

PS_OUTPUT PShaderThreeLayer(PS_INPUT Input)
{	
	PS_OUTPUT   Output;
	
	Output.Color=tex2D( Sampler0, Input.Tex0.xy*TexRatio );
	
	float4 Color1=tex2D( Sampler1, Input.Tex0.xy*TexRatio );
	float4 Alpha1=tex2D( Sampler2, Input.Tex0.xy*AlphaTexRatio );	
	
	float4 Color2=tex2D( Sampler3, Input.Tex0.xy*TexRatio );
	float4 Alpha2=tex2D( Sampler4, Input.Tex0.xy*AlphaTexRatio );	
	
	Output.Color=Output.Color*(1.0f-Alpha1.a-Alpha2.a)+Color1*Alpha1.a+Color2*Alpha2.a;	
	
	Output.Color=Output.Color*Input.Diffuse+Input.Specular*Output.Color.a;
	Output.Color.xyz=Output.Color.xyz*(1.0f-Input.Tex0.z)+FogColor*Input.Tex0.z;
	
	Output.Color=Output.Color*GlobalColor;
	
    Output.ColorForDepthMap=Input.Tex0.w;
    return Output;
}

PS_OUTPUT PShaderThreeLayerWithShadow(PS_INPUT Input)
{	
	PS_OUTPUT   Output;
	
	Output.Color=tex2D( Sampler0, Input.Tex0.xy*TexRatio );
	
	float4 Color1=tex2D( Sampler1, Input.Tex0.xy*TexRatio );
	float4 Alpha1=tex2D( Sampler2, Input.Tex0.xy*AlphaTexRatio );	
	
	float4 Color2=tex2D( Sampler3, Input.Tex0.xy*TexRatio );
	float4 Alpha2=tex2D( Sampler4, Input.Tex0.xy*AlphaTexRatio );	
	
	Output.Color=Output.Color*(1.0f-Alpha1.a-Alpha2.a)+Color1*Alpha1.a+Color2*Alpha2.a;	
	
	float4 SHAlpha=tex2D( SamplerShadow, Input.Tex0.xy );	
	Output.Color=Output.Color*saturate(ShadowColor*ShadowRatio+(1-SHAlpha.a));
	
	
    Output.Color=Output.Color*Input.Diffuse+Input.Specular*Output.Color.a*(1-SHAlpha.a);
	Output.Color.xyz=Output.Color.xyz*(1.0f-Input.Tex0.z)+FogColor*Input.Tex0.z;
	
	Output.Color=Output.Color*GlobalColor;
	
    Output.ColorForDepthMap=Input.Tex0.w;
    return Output;
}

PS_OUTPUT PShaderFourLayer(PS_INPUT Input)
{	
	PS_OUTPUT   Output;
	
	Output.Color=tex2D( Sampler0, Input.Tex0.xy*TexRatio );
	
	float4 Color1=tex2D( Sampler1, Input.Tex0.xy*TexRatio );
	float4 Alpha1=tex2D( Sampler2, Input.Tex0.xy*AlphaTexRatio );	
	
	float4 Color2=tex2D( Sampler3, Input.Tex0.xy*TexRatio );
	float4 Alpha2=tex2D( Sampler4, Input.Tex0.xy*AlphaTexRatio );	
	
	float4 Color3=tex2D( Sampler5, Input.Tex0.xy*TexRatio );
	float4 Alpha3=tex2D( Sampler6, Input.Tex0.xy*AlphaTexRatio );	
	
	Output.Color=Output.Color*(1.0f-Alpha1.a-Alpha2.a-Alpha3.a)+Color1*Alpha1.a+Color2*Alpha2.a+Color3*Alpha3.a;
	
	Output.Color=Output.Color*Input.Diffuse+Input.Specular*Output.Color.a;
	Output.Color.xyz=Output.Color.xyz*(1.0f-Input.Tex0.z)+FogColor*Input.Tex0.z;
	
	Output.Color=Output.Color*GlobalColor;
	
    Output.ColorForDepthMap=Input.Tex0.w;
    return Output;
}

PS_OUTPUT PShaderFourLayerWithShadow(PS_INPUT Input)
{	
	PS_OUTPUT   Output;
	
	Output.Color=tex2D( Sampler0, Input.Tex0.xy*TexRatio );
	
	float4 Color1=tex2D( Sampler1, Input.Tex0.xy*TexRatio );
	float4 Alpha1=tex2D( Sampler2, Input.Tex0.xy*AlphaTexRatio );	
	
	float4 Color2=tex2D( Sampler3, Input.Tex0.xy*TexRatio );
	float4 Alpha2=tex2D( Sampler4, Input.Tex0.xy*AlphaTexRatio );	
	
	float4 Color3=tex2D( Sampler5, Input.Tex0.xy*TexRatio );
	float4 Alpha3=tex2D( Sampler6, Input.Tex0.xy*AlphaTexRatio );	
	
	Output.Color=Output.Color*(1.0f-Alpha1.a-Alpha2.a-Alpha3.a)+Color1*Alpha1.a+Color2*Alpha2.a+Color3*Alpha3.a;
	
	float4 SHAlpha=tex2D( SamplerShadow, Input.Tex0.xy );	
	Output.Color=Output.Color*saturate(ShadowColor*ShadowRatio+(1-SHAlpha.a));
	
	
    Output.Color=Output.Color*Input.Diffuse+Input.Specular*Output.Color.a*(1-SHAlpha.a);
	Output.Color.xyz=Output.Color.xyz*(1.0f-Input.Tex0.z)+FogColor*Input.Tex0.z;
	
	Output.Color=Output.Color*GlobalColor;
	
	Output.ColorForDepthMap=Input.Tex0.w;
    return Output;
}

VS_OUTPUT VShaderNormal(VS_INPUT Input)
{
    VS_OUTPUT   Output;
    
    
    float3 Pos =mul(float4(Input.Pos.xyz, 1.0f), WorldMatrix);
    float3 Normal = normalize(mul(Input.Normal.xyz, WorldMatrix));
    
    
    
    Output.Pos = mul(float4(Input.Pos.xyz, 1.0f), WorldViewMatrix);
    Output.Tex0.z = saturate(((Output.Pos.z-FogNear)/(FogFar-FogNear)));
    Output.Tex0.w = saturate((Output.Pos.z-CameraNear)/(CameraFar-CameraNear));
    Output.Pos = mul(Output.Pos, PrjMatrix);
    
    Output.Diffuse = CaculateDiffuse(Pos,Normal);
    Output.Specular = CaculateSpecular(Pos,Normal);
    Output.Tex0.xy = Input.Tex0; 	

    return Output;
}

float4 PShaderForDepth( float4 Diffuse : COLOR0) : COLOR0
{		
	return Diffuse;
}

VS_OUTPUT_FOR_DEPTH VShaderForDepth(VS_INPUT Input)
{
    VS_OUTPUT_FOR_DEPTH   Output;
    
    Output.Pos = mul(float4(Input.Pos.xyz, 1.0f), WorldViewMatrix);
    Output.Pos = mul(Output.Pos, PrjMatrix);
    Output.Diffuse = saturate((Output.Pos.z-CameraNear)/(CameraFar-CameraNear));
    
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
        
			
        
        VertexShader = compile vs_3_0 VShaderNormal();
        PixelShader  = compile ps_3_0 <PSFunction>();

    }
}

technique TechForDepthTexture
{
    pass p0
    { 	   
		zenable = True;
		zwriteenable = True;
		fogenable = False;
		CullMode = ccw;
		
		AlphaBlendEnable = False;		
		AlphaTestEnable = False;
        
			
        
        VertexShader = compile vs_2_0 VShaderForDepth();
        PixelShader  = compile ps_2_0 PShaderForDepth();

    }
}