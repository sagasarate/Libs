#include "BuildIn\SharedLib.fx"



float4		GlobalColor=1.0f;
float4x4    UVMatrix0;

float4x4			WorldMatrix;
float4x4			WorldViewMatrix;

texture TexLay0 ;
texture TexLay1 ;
texture TexLay2 ;
texture TexLay3 ;


sampler Sampler0 =
sampler_state
{
    Texture = <TexLay0>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = <TextureAddrU0>;
    AddressV = <TextureAddrV0>;

};

sampler Sampler1 =
sampler_state
{
    Texture = <TexLay1>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = <TextureAddrU1>;
    AddressV = <TextureAddrV1>;

};

sampler Sampler2 =
sampler_state
{
    Texture = <TexLay2>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = <TextureAddrU2>;
    AddressV = <TextureAddrV2>;

};


sampler Sampler3 =
sampler_state
{
    Texture = <TexLay3>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = <TextureAddrU3>;
    AddressV = <TextureAddrV3>;

};



///////////////////////////////////////////////////////
struct VS_INPUT
{
    float4  Pos             : POSITION;
    float4  Normal          : NORMAL;
    float2  Tex0            : TEXCOORD0;
    float4	Diffuse			: COLOR0;
    float4  BlendWeights    : BLENDWEIGHT;
    float4	BlendIndices    : BLENDINDICES;
    
};

struct VS_OUTPUT
{
    float4  Pos			: POSITION;
    float4  Diffuse		: COLOR0;  
    float4  Tex0		: TEXCOORD0;
    float3  EnvTex		: TEXCOORD1;
};

struct PS_INPUT
{
	float4	Diffuse		: COLOR0;	
    float4	Tex0		: TEXCOORD0;
    float3  EnvTex		: TEXCOORD1;
    
};

struct PS_OUTPUT
{
	float4	Color				: COLOR0;
	float4	ColorForDepthMap	: COLOR1;
};


PS_OUTPUT PShaderWithNormal(PS_INPUT Input)
{
	PS_OUTPUT Output;
		
	Output.Color=tex2D( Sampler0, Input.Tex0.xy );
<OtherPSOperation>
	Output.Color=Output.Color<PSDiffuseFunction>;
	Output.Color=Output.Color*GlobalColor;
	if(FogEnable)
	{
		Output.Color.xyz=Output.Color.xyz*(1.0f-Input.Tex0.z)+FogColor*Input.Tex0.z;
	}
	
	Output.ColorForDepthMap=Input.Tex0.w;
    return Output;
}


VS_OUTPUT VShaderNormal(VS_INPUT Input)
{
    VS_OUTPUT   Output;
    float3      Pos = Input.Pos.xyz;
    float3      Normal = 0.0f;
        
   
    Normal = normalize(mul(Input.Normal.xyz, WorldMatrix));

	Output.Pos = mul(float4(Pos.xyz, 1.0f), WorldViewMatrix);
	Output.Tex0.z = saturate(((Output.Pos.z-FogNear)/(FogFar-FogNear)));
	Output.Tex0.w = saturate((Output.Pos.z-CameraNear)/(CameraFar-CameraNear));
	Output.Pos = mul(Output.Pos, PrjMatrix);
	Output.Diffuse = <VSDiffuseFunction>;
	<VertexAlphaOperation>;
    <TextureUVAni>; 
	Output.EnvTex = reflect(CameraPos,Normal);

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
        AlphaRef = 0xE0;
        AlphaFunc = GreaterEqual;
			
        
        VertexShader = compile vs_2_0 VShaderNormal();
        PixelShader  = compile ps_2_0 PShaderWithNormal();

    }
}

