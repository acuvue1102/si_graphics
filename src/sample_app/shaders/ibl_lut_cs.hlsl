
#define THREAD_X 8
#define THREAD_Y 8
#define PI 3.14159265359

cbuffer RootConstants : register(b0)
{
	uint width;
	uint height;
};

Texture2D<float4>   inputTexture  : register(t0);
RWTexture2D<float4> outputTexture : register(u0);

float G_Smith(float roughness, float NdotV, float NdotL)
{
	float alpha = roughness * roughness;
	float k = alpha/2;

	float g1 = NdotV/(NdotV*(1-k) + k);
	float g2 = NdotL/(NdotL*(1-k) + k);

	return g1*g2;
}

float3 TangentToWorld(float3 V, float3 N)
{
	float tmp = (abs(N.y) < 0.999)? float3(0,1,0) : float3(1,0,0);
	float3 tangentZ = normalize( cross( tmp, N ) );
	float3 tangentX = cross( N, tangentZ );
	return tangentX * V.x + N * V.y + tangentZ * V.z;
}

float3 ImportanceSampleGGX( float2 random2, float roughness, float3 N )
{
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;

	float phi = 2 * PI * random2.x;
	float cosTheta = sqrt( (1 - random2.y) / ( 1 + (alpha2 - 1) * random2.y ) );
	float sinTheta = sqrt( 1 - cosTheta * cosTheta );

	float3 H;
	H.x = sinTheta * cos( phi );
	H.y = cosTheta;
	H.z = sinTheta * sin( phi );

	return normalize(TangentToWorld(H,N));
}

float2 Hammersley2d(uint i, uint maxSample)
{
	float x = float(i)/float(maxSample);
	
	uint bits = i;
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	float y = float(bits) * 2.3283064365386963e-10;

	return float2(x, y); // value range is 0 - 1.
}

float2 IntegrateBRDF(float roughness, float NdotV)
{
	roughness = max(roughness, 0.04);

	float3 V = float3( sqrt( 1.0f - NdotV * NdotV ), NdotV, 0);

	float2 lut = float2(0.0, 0.0);
	const int sampleCount = 1024;
	for(int i=0; i<sampleCount; ++i)
	{
		float2 random2 = Hammersley2d( i, sampleCount );
		float3 N = float3(0,1,0);
        float3 H  = ImportanceSampleGGX(random2, roughness, N);

		float3 L = reflect(-V, H);
		float NdotL = max(0, L.y);
		float NdotH = max(0, H.y);
		float VdotH = max(0, dot(V,H));

		if(0.0 < NdotL)
		{		
			float FBias  = (pow(1-VdotH, 5));
			float FScale = (1 - FBias);
			float G = G_Smith(roughness, NdotV, NdotL);
			float GVis = G * VdotH / (NdotH * NdotV);
		
			lut.x += GVis * FScale;
			lut.y += GVis * FBias;
		}
	}

	lut.xy /= (float2)sampleCount.xx;

	return lut;
}

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

	float2 uv = ((float2)index.xy + float2(0.5,0.5)) / float2(width, height);
	float NdotV     = uv.x;
	float roughness = uv.y;
	
	float2 lut = IntegrateBRDF(uv.y, uv.x);

	outputTexture[index] = float4(lut,0,1);
}
