
Texture2D<float4> texture0 : register(t0);
SamplerState      sampler0 : register(s0);

struct PointLightInfo
{
	float4 Position;
	float4 Color;
};

cbuffer ConstantBuffer : register(b1)
{
	float4x4         World[8]     : packoffset(c0.x);
	float4x4         View         : packoffset(c32.x);
	float4x4         ViewProj     : packoffset(c36.x);
	float2           UvScale      : packoffset(c40.x);
	PointLightInfo   LightInfo[4] : packoffset(c41.x);
};

struct VSInput
{
	float3 position   : POSITION;
	float3 normal     : NORMAL;
	float2 uv         : TEXCOORD0;
};

struct PSInput
{
	float4 position  : SV_POSITION;
	float4 posInView : TEXCOORD0;
	float3 normal    : TEXCOORD1;
	float2 uv        : TEXCOORD2;
};

struct PsOutput
{
	float4 color    : SV_TARGET;
};

PSInput VSMain(
	VSInput input, 
	uint    insId : SV_InstanceID)
{
	PSInput result;

	float4 worldPos    = mul(World[insId], float4(input.position.xyz, 1));
	float3 worldNormal = mul((float3x3)World[insId], input.normal);
	
	result.position  = mul(ViewProj, worldPos);
	result.posInView = mul(View, worldPos);
	result.normal    = mul((float3x3)View, worldNormal);
	result.uv        = UvScale * input.uv;

	return result;
}

PsOutput PSMain(PSInput input)
{
	PsOutput output;

	float3 pos = input.posInView.xyz/input.posInView.w;
	float3 N = normalize(input.normal);
	float3 albedo = texture0.Sample(sampler0, input.uv).xyz;

	float3 color = 0.0.xxx;
	for(int i=0; i<4; ++i)
	{
		float3 lightPos   = LightInfo[i].Position.xyz;
		float3 lightColor = LightInfo[i].Color.xyz;
		float4 pointLightPos = mul(View, float4(lightPos,1));
		pointLightPos.xyz /= pointLightPos.w;
		float3 diff = (pos - pointLightPos.xyz);
		float lengthStr = dot(diff,diff);
		float3 light = lightColor / (1 + lengthStr);
		
		float3 L = normalize(diff);
		float NdotL = saturate(dot(N,L));

		float3 diffuse = light * albedo * NdotL;

		color += diffuse;
	}
	
	
	float3 skyColor = float3(0.2, 0.3, 0.4);
	float3 groundColor = float3(0.2, 0.2, 0.2);
	float3 skyDir = float3(0,1,0);
	float3 ambient = lerp(groundColor, skyColor, saturate(dot(skyDir, N)*0.5+0.5));
	color += ambient;

	output.color = float4(pow(color, 2.2), 1.0f);

	return output;
}
