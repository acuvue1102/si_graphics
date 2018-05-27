
Texture2D<float4> texture0 : register(t0);
SamplerState      sampler0 : register(s0);

cbuffer ConstantBuffer : register(b1)
{
	float m_vertexScale : packoffset(c0.x);
	float m_uvScale     : packoffset(c0.y);
};

struct VSInput
{
	float3 position : POSITION;
	float2 uv       : TEXCOORD0;
};

struct PSInput
{
	float4 position : SV_POSITION;
	float2 uv       : TEXCOORD0;
};

struct PsOutput
{
	float4 color    : SV_TARGET;
};

PSInput VSMain(VSInput input)
{
	PSInput result;

	result.position = float4(m_vertexScale * input.position.xyz, 1);
	result.uv       = m_uvScale * input.uv;

	return result;
}

PsOutput PSMain(PSInput input)
{
	PsOutput output;
	
	output.color = texture0.Sample(sampler0, input.uv);

	return output;
}
