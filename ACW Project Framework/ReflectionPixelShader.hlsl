//Globals
TextureCube textureCube : register(t0);

SamplerState sampleTypeWrap : register(s0);


//Type definitions
struct PixelInput
{
	float4 positionH : SV_POSITION;
	float3 positionW : POSITION;
	float3 posWorld : TEXCOORD1;
	float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD0;
};

float4 ReflectionPixelShader(PixelInput input) : SV_TARGET
{
	return textureCube.Sample(sampleTypeWrap, reflect((input.positionW - input.viewDirection), -input.posWorld));
}