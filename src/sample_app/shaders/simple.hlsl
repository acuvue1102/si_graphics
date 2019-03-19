
cbuffer SceneCB : register(b0)
{
	float4x4 cbView : packoffset(c0.x);
	float4x4 cbProj : packoffset(c4.x);
};

cbuffer InstanceCB : register(b1)
{
	float4x4 cbWorlds[64] : packoffset(c0.x);
};

cbuffer MaterialCB : register(b2)
{
	float2 cbUvScale     : packoffset(c0.x);
};

Texture2D<float4> texture0 : register(t0);
SamplerState      sampler0 : register(s0);

struct VSInput
{
	float3 position : POSITION;
	float3 normal   : NORMAL;
	float2 uv       : TEXCOORD0;
};

struct PSInput
{
	float4 position : SV_POSITION;
	float3 normal   : TEXCOORD0;
	float2 uv       : TEXCOORD1;
};

struct PsOutput
{
	float4 color    : SV_TARGET;
};

PSInput VSMain(VSInput input,
	uint    insId : SV_InstanceID)
{
	PSInput result;

	float4 worldPos    = mul(cbWorlds[insId], float4(input.position.xyz, 1));
	float3 worldNormal = mul((float3x3)cbWorlds[insId], input.normal);
	
	result.position  = mul(ViewProj, worldPos);
	result.uv       = cbUvScale * input.uv;

	return result;
}

PsOutput PSMain(PSInput input)
{
	PsOutput output;
	
	output.color = texture0.Sample(sampler0, input.uv);

	return output;
}
