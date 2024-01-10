
//Globals
TextureCube shaderTexture;
SamplerState sampleType;

//Type definitions
struct PixelInput
{
	float4 positionH : SV_POSITION;
	float3 positionW : POSITION;
	float3 tex : TEXCOORD0;
};

float4 TextureCubePixelShader(PixelInput input) : SV_TARGET
{
	float4 textureColour;

//Receive pixel sample from the texture2D using the samplerstate and receive the colours needed
textureColour = shaderTexture.Sample(sampleType, input.tex);

return textureColour;
}