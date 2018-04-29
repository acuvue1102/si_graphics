
struct VSInput
{
	float4 position : POSITION;
	float4 color    : COLOR;
};

struct PSInput
{
	float4 position : SV_POSITION;
	float4 color    : COLOR;
};

struct PsOutput
{
	float4 color    : SV_TARGET
};

PSInput VSMain(VSInput input)
{
	PSInput result;

	result.position = input.position;
	result.color    = input.color;

	return result;
}

PsOutput PSMain(PSInput input)
{
	PsOutput output;

	output.color = input.color;

	return output;
}
