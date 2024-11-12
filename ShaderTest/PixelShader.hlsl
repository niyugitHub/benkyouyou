

cbuffer ConstatBuffer : register(b8){
	float time;
}

Texture2D tex:register(t0);
Texture2D dist:register(t1);
Texture2D depth:register(t2);
Texture2D boke0 : register(t3);
Texture2D boke1 : register(t4);
Texture2D norm : register(t5);

SamplerState smp:register(s0);

struct PSInput{
	float4 pos:SV_POSITION;
	float3 color:COLOR;
	float2 uv : TEXCOORD;
};

float4 main(PSInput input) : SV_TARGET
{
	float4 nm = dist.Sample(smp,input.uv);
	//0Å`1Å®-1Å`1
	nm.xy*=2;
	nm.xy-=1;
	float4 col = tex.Sample(smp,input.uv);//+nm.xy*0.25*sin(time));
    float4 bk0 = boke0.Sample(smp, input.uv);
    float4 bk1 = boke1.Sample(smp, input.uv);
	float3 N = norm.Sample(smp,input.uv).rgb;

	uint w,h,mip;
	norm.GetDimensions(0,w,h,mip);
	float2 dxdy = float2(1.0/w,1.0/h);
	float3 N1 = norm.Sample(smp,input.uv+dxdy*float2(1,0)).rgb;
	float3 N2 = norm.Sample(smp,input.uv+dxdy*float2(-1,0)).rgb;
	float3 N3 = norm.Sample(smp,input.uv+dxdy*float2(0,1)).rgb;
	float3 N4 = norm.Sample(smp,input.uv+dxdy*float2(0,-1)).rgb;

	float b=step(0.95,(1-(4*N-N1-N2-N3-N4)).r);

	//return float4( b,b,b, 1);

	N=N*2-1;

	float3 light=float3(1,-1,1);
	float diffuse=dot(N,-light);
	
	//return float4(col.rgb*diffuse,col.a);

	float dep = depth.Sample(smp,input.uv).r;

    dep = pow(dep, 2000);

	float dis=distance(dep,0);
	float t = smoothstep(0.75,0.85,dis);
	//return lerp(col, bk1, t)+ (sin(time+input.uv.x*10)+1)*input.uv.y* float4(1,1,1,1)*dep*0.4;

    //float t = smoothstep(0.125, 0.5, distance(0.25, dep));
    //return lerp(col, bk1, t);

	return col;
}