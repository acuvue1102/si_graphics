
#define THREAD_X 8
#define THREAD_Y 8

cbuffer RootConstants : register(b0)
{
	uint width;
	uint height;
};

Texture2D<float4>   inputTexture  : register(t0);
RWTexture2D<float4> outputTexture : register(u0);

[numthreads(THREAD_X, THREAD_Y, 1)]
void CSMain(
	uint3 groupId          : SV_GroupID,
	uint3 groupThreadId    : SV_GroupThreadID,
	uint3 dispatchThreadID : SV_DispatchThreadID,
	uint  groupIndex       : SV_GroupIndex)
{
	uint2 index = dispatchThreadID.xy;

	if(any(uint2(width, height) <= index))
	{
		// out of pixels.
		return;
	}

	outputTexture[index] = float4(1.0f, 0.5f, 0.5f, 1.0f) * inputTexture[index];
}
