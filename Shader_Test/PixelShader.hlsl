
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
    float4 col = tex.Sample(smp, input.uv);
    float bk0 = boke0.Sample(smp, input.uv);
    float bk1 = boke1.Sample(smp, input.uv);
    float3 N = norm.Sample(smp, input.uv).rgb;

    return col;
    
    uint w, h, mip;
    norm.GetDimensions(0, w, h, mip);
    float2 dxdy = float2(1.0 / w, 1.0 / h);
    float3 N1 = norm.Sample(smp, input.uv + dxdy * float2(1, 0)).rgb;
    float3 N2 = norm.Sample(smp, input.uv + dxdy * float2(-1, 0)).rgb;
    float3 N3 = norm.Sample(smp, input.uv + dxdy * float2(0, 1)).rgb;
    float3 N4 = norm.Sample(smp, input.uv + dxdy * float2(0, -1)).rgb;

    float b = step(0.9,(1 - (4 * N - N1 - N2 - N3 - N4)).r);

    return float4(b, b, b, 1);

    N = (N * 2) - 1;

    float3 light = float3(1, -1, 1);
    float diffuse = dot(N, -light);

    return float4(col.rgb * diffuse, col.a);

    float4 pos = depth.Sample(smp, input.uv);
    //float dep = pow(pos.r, 500);

    float dep = depth.Sample(smp, input.uv).r;

    dep = pow(dep, 200);


    //‹——£
    float dis = distance(dep, 1);
    //smoothstep‚Æ‚Í
    //‚ ‚éˆê’è‚Ì’l‚ğ’´‚¦‚½‚ç‹}Œƒ‚É”’l‚ª‘‚¦‚Ä‚¢‚­
    float t = smoothstep(0.25, 0.55, dis);

    //‹——£‚É‰‚¶‚Ä”’‚­‚È‚é
    //return lerp(col, bk1, t) + float4(1,1,1,1) * dep * 0.4;

    return lerp(col, bk1, t) + (sin(time + input.uv.x * 10) + 1) * input.uv.y * float4(1, 1, 1, 1) * dep * 0.4;

    return col;
    return lerp(col, bk1, distance(dep, 0.5));


    //return bk0;
    //return float4(dep, dep, dep, 1);




    if (col.r == 0.0f &&
        col.g == 0.0f &&
        col.b == 0.0f)
    {
        return float4(0.0f, 0.0f, 0.0f, 0.0f);
    }
    col.r = col.r + time;
    //col *= depth.Sample(smp, input.uv).r;

    //return float4(dep)

    return float4(col.r, col.g, col.b, col.a);
    return col.brga;
}