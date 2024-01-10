cbuffer MatrixBuffer : register(b0)
{
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer DisplacementBuffer : register(b1)
{
	float mipInterval;
	float mipMinimum;
	float mipMaximum;
	float displacementPower;
};

//Type definitions
struct PatchConstantOutput
{
	float edges[3] : SV_TessFactor;
	float inside : SV_InsideTessFactor;
};

struct DomainInput
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

struct PixelInput
{
	float4 positionH : SV_POSITION;
	float3 positionW : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

[domain("tri")]
PixelInput TextureNormalDomainShader(in PatchConstantOutput input, in const float3 uvwCoord : SV_DomainLocation, const OutputPatch<DomainInput, 3> patch)
{
	PixelInput output;

	//Calculate new vertex position
	output.positionW = uvwCoord.x * patch[0].position + uvwCoord.y * patch[1].position + uvwCoord.z * patch[2].position;
	output.tex = uvwCoord.x * patch[0].tex + uvwCoord.y * patch[1].tex + uvwCoord.z * patch[2].tex;
	output.normal = uvwCoord.x * patch[0].normal + uvwCoord.y * patch[1].normal + uvwCoord.z * patch[2].normal;
	output.tangent = uvwCoord.x * patch[0].tangent + uvwCoord.y * patch[1].tangent + uvwCoord.z * patch[2].tangent;
	output.binormal = uvwCoord.x * patch[0].binormal + uvwCoord.y * patch[1].binormal + uvwCoord.z * patch[2].binormal;

	output.normal = normalize(output.normal);


	//Offset the vertex along the normal
	const float scaleOffset = 0.15f;

	matrix viewProjection = mul(viewMatrix, projectionMatrix);

	output.positionH = mul(float4(output.positionW, 1.0f), viewProjection);

	return output;
}