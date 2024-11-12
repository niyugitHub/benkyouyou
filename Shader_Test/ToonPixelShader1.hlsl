cbuffer ConstantBuffer : register(b8)
{
    float time;
}

Texture2D tex : register(t0);
Texture2D depth : register(t1);
Texture2D boke0 : register(t2);
Texture2D boke1 : register(t3);
Texture2D norm : register(t4);

sampler smp : register(s0);

struct PSInput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

float4 main(PSInput input) : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}