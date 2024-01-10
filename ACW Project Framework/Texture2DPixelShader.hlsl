
//Globals
Texture2D shaderTexture;
SamplerState sampleType;

//Type definitions
struct PixelInput
{
	float4 positionH : SV_POSITION;
	float3 positionW : POSITION;
	float2 tex : TEXCOORD0;
};

float4 Texture2DPixelShader(PixelInput input) : SV_TARGET
{
	float4 textureColour;

    //Receive pixel sample from the texture2D using the samplerstate and receive the colours needed
	textureColour = shaderTexture.Sample(sampleType, input.tex);

	return textureColour;
}