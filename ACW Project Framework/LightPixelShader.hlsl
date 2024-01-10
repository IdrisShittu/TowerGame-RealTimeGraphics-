
#define MAX_LIGHTS 16

//Globals
Texture2D shaderTexture;
SamplerState sampleType;

struct Lights
{
	float4 ambientColour;
	float4 diffuseColour;
	float4 lightPositions;
};

cbuffer LightBuffer : register(b0)
{
	Lights lights[MAX_LIGHTS];
	int lightCount;
	float3 padding;
};

//Type definitions
struct PixelInput
{
	float4 positionH : SV_POSITION;
	float3 positionW : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

float4 LightPixelShader(PixelInput input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	float4 baseColour = shaderTexture.Sample(sampleType, input.tex);

	float4 totalAmbient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 totalDiffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 totalSpecular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	for (int i = 0; i < lightCount; i++)
	{
		totalAmbient += saturate(lights[i].ambientColour * baseColour);

		float3 lightDirection = normalize(lights[i].lightPositions.xyz - input.positionW);

		float nDotL = dot(input.normal, lightDirection);

		totalDiffuse += saturate(lights[i].diffuseColour * baseColour);
	}

	return totalAmbient + totalDiffuse;
}