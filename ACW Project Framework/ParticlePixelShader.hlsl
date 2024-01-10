
//Globals
Texture2D shaderTexture;
SamplerState sampleType;

cbuffer PixelParameters : register(b1)
{
	float3 colourTint;
	float transparency;
};

//Type definitions
struct PixelInput
{
	float4 positionH : SV_POSITION;
	float3 positionW : POSITION;
	float2 tex : TEXCOORD0;
};

float4 ParticlePixelShader(PixelInput input) : SV_TARGET
{
	float3 textureColour;

	float range = 0.5f * 0.5f - dot(input.tex - 0.5f, input.tex - 0.5f);

	float shade = 2.0f / (1.0f + exp(12 * range));

	//Receive pixel sample from the texture2D using the samplerstate and receive the colours needed
	textureColour = shaderTexture.Sample(sampleType, input.tex).rgb;

	float greyScale = (textureColour.r + textureColour.g + textureColour.b) / 3.0f;

	return float4((1 - shade) * saturate(colourTint * greyScale), transparency);
}