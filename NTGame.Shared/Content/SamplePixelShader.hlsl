// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 color : COLOR0;
	float2 uv: TEXCOORD0;
};

Texture2D diffuseTexture : register(t0);
SamplerState linearSampler : register(s0);

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 color = diffuseTexture.Sample(linearSampler, input.uv);
	//color.rgb += 1.0f;
	return float4(color.rgb, 1.0f);
}
