//Global
cbuffer MatrixBuffer : register(b0)
{
	matrix worldInverseMatrix;
	matrix texTransform;
};

cbuffer TessellationBuffer : register(b1)
{
	float maxTessellationDistance;
	float minTessellationDistance;
	float maxTessellationFactor;
	float minTessellationFactor;
};

cbuffer CameraBuffer : register(b2)
{
	float3 cameraPosition;
	float padding;
};

//Type definitions
struct VertexInput
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	matrix instanceWorldMatrix : INSTANCEMATRIX;
};

struct HullInput
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float tessellationFactor : TESS;
};

HullInput LightVertexShader(VertexInput input)
{
	HullInput output;

	output.position = mul(float4(input.position, 1.0f), input.instanceWorldMatrix).xyz;

	//Pass texture coords to pixel shader
	output.tex = input.tex;
	//output.tex = mul(float4(input.tex, 0.0f, 1.0f), texTransform).xy;

	//Calculate normal vector against world matrix
	output.normal = normalize(mul(input.normal, (float3x3)input.instanceWorldMatrix));

	float distanceToCamera = distance(output.position, cameraPosition);

	float tess = saturate((minTessellationDistance - distanceToCamera) / (minTessellationDistance - maxTessellationDistance));

	output.tessellationFactor = minTessellationFactor + tess * (maxTessellationFactor - minTessellationFactor);

	return output;
}