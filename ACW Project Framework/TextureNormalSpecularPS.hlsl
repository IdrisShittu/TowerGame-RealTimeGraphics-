
#define MAX_LIGHTS 16

//Globals
Texture2D textures[3] : register(t0);
Texture2D depthMapTexture[MAX_LIGHTS] : register(t3);
SamplerState sampleTypeWrap : register(s0);
SamplerState sampleTypeClamp : register(s1);

struct Lights
{
	float4 ambientColour;
	float4 diffuseColour;
	float4 specularColour;
	float3 lightPositions;
	float specularPower;
	int isDirectionalLight;
	float3 padding;
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
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float3 viewDirection : TEXCOORD1;
	float4 lightViewPosition[MAX_LIGHTS] : TEXCOORD2;
};

float CalculateShadow(Texture2D depthMap, float4 position, float nDotL)
{
	float2 projectedTextureCoords;

	projectedTextureCoords.x = position.x / position.w / 2.0f + 0.5f;
	projectedTextureCoords.y = -position.y / position.w / 2.0f + 0.5f;

	float lightDepthValue = position.z / position.w;

	float bias = max(0.01f * (1.0f - nDotL), 0.005f);
	//bias = 0.005f;

	float shadow = 0.0f;

	// Determine if the projected coordinates are in the 0 to 1 range.  If so then this pixel is in the view of the light.
	if ((saturate(projectedTextureCoords.x) == projectedTextureCoords.x) && (saturate(projectedTextureCoords.y) == projectedTextureCoords.y))
	{
		float2 textureSize;

		depthMap.GetDimensions(textureSize.x, textureSize.y);

		textureSize = 1.0f / textureSize;

		float depthValue = 0.0f;

		for (int x = -2; x <= 2; x++)
		{
			for (int y = -2; y <= 2; y++)
			{
				float depthValue = depthMap.Sample(sampleTypeClamp, projectedTextureCoords.xy + float2(x, y) * textureSize).r;
				shadow += lightDepthValue - bias > depthValue ? 1.0f : 0.0f;
			}
		}
	}

	return shadow / 25.0f;
}

float4 TextureNormalSpecularPS(PixelInput input) : SV_TARGET
{
	//float3 lightPosWorld;
	//float4 textureColour;
	//float4 bumpMap;
	//float3 bumpNormal;
	//float lightIntensity;
	//float4 colour = float4(0.0f, 0.0f, 0.0f, 0.0f);
	//float4 specularIntensity;
	//float3 reflection;
	//float4 specular;

	//input.normal = normalize(input.normal);

	////Receive pixel sample from normal texture using the samplerstate
	//bumpMap = textures[1].Sample(sampleType, input.tex);

	////Range is held in [0,1], need to expand that to [-1.1]
	//bumpMap = (bumpMap * 2.0f) - 1.0f;

	////Calculate our normal
	//bumpNormal = normalize((bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal));

	//for (int i = 0; i < lightCount; i++)
	//{
	//	lightPosWorld = normalize(lights[i].lightPositions - input.positionW);

	//	lightIntensity = saturate(dot(bumpNormal, lightPosWorld));

	//	colour += lights[i].diffuseColour * lightIntensity;

	//	//If there is a light intensity we need to do a specular calculation on our colour
	//	if (lightIntensity > 0.0f)
	//	{
	//		//Receive pixel sample from specular texture using the samplerstate
	//		specularIntensity = textures[2].Sample(sampleType, input.tex);

	//		//Calculate the reflection with the bumpmap normal
	//		//reflection = normalize(2 * lightIntensity * bumpNormal - (-lightDirection));
	//		reflection = normalize(((2.0f * lightIntensity) * dot(bumpNormal, lightPosWorld)) - lightPosWorld);

	//		//Calculate the specular colour
	//		//specular = pow(saturate(max(0.0f, dot(reflection, input.viewDirection)), lights[i].specularPower);

	//		specular = lights[i].specularColour * (pow(max(0.0f, dot(reflection, input.viewDirection)), lights[i].specularPower));

	//		//Multiply specular colour by intensity
	//		specular = specular * specularIntensity;

	//		//Add to our colour
	//		colour += specular;
	//	}
	//}

	////Receive pixel sample from colour texture using the samplerstate
	//textureColour = textures[0].Sample(sampleType, input.tex);

	//colour = saturate(colour) * textureColour;

	////return float4(lightCount, 0.0f, 0.0f, 1.0f);

	//return colour;

	float3 viewDirection = normalize(input.viewDirection);

	float4 baseColour = textures[0].Sample(sampleTypeWrap, input.tex);

	float4 totalAmbient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 totalDiffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 totalSpecular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	//Receive pixel sample from normal texture using the samplerstate
	float4 bumpMap = textures[1].Sample(sampleTypeWrap, input.tex);

	//Range is held in [0,1], need to expand that to [-1.1]
	bumpMap = (bumpMap * 2.0f) - 1.0f;

	//Calculate our normal
	float3 bumpNormal = normalize((bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal));

	float shadow = 0.0f;

	for (int i = 0; i < lightCount; i++)
	{
		totalAmbient += saturate(lights[i].ambientColour * baseColour);

		float3 lightDirection = normalize(lights[i].lightPositions - input.positionW);

		float nDotL = dot(bumpNormal, lightDirection);

		float3 reflection = normalize(((2.0f * bumpNormal) * nDotL) - lightDirection);

		float rDotV = max(0.0f, dot(reflection, viewDirection));

		if (lights[i].isDirectionalLight)
		{
			shadow += CalculateShadow(depthMapTexture[i], input.lightViewPosition[i], nDotL);
		}

		totalDiffuse += saturate(lights[i].diffuseColour * nDotL * baseColour);

		if (saturate(nDotL) > 0.0f)
		{
			float4 specularIntensity = textures[2].Sample(sampleTypeWrap, input.tex);
			totalSpecular += saturate(lights[i].specularColour * (pow(rDotV, lights[i].specularPower)) * specularIntensity);
		}
	}

	return ((totalAmbient + (1.0f - shadow)) * (totalDiffuse + totalSpecular));
}