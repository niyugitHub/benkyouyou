// ピクセルシェーダーの入力
struct PS_INPUT
{
    float4 Diffuse : COLOR0; // ディフューズカラー
    float4 Specular : COLOR1; // スペキュラカラー
    float4 TexCoords0_1 : TEXCOORD0; // xy:テクスチャ座標 zw:サブテクスチャ座標
    float3 VPosition : TEXCOORD1; // 座標( ビュー空間 )
    float3 VNormal : TEXCOORD2; // 法線( ビュー空間 )
#ifdef BUMPMAP
    float3 VTan            : TEXCOORD3;    // 接線( ビュー空間 )
    float3 VBin            : TEXCOORD4;    // 従法線( ビュー空間 )
#endif // BUMPMAP
    float1 Fog : TEXCOORD5; // フォグパラメータ( x )

#if SHADOWMAP
    float3 ShadowMap0Pos   : TEXCOORD6;	// シャドウマップ０のライト座標( x, y, z )
    float3 ShadowMap1Pos   : TEXCOORD7;	// シャドウマップ１のライト座標( x, y, z )
    float3 ShadowMap2Pos   : TEXCOORD8;	// シャドウマップ２のライト座標( x, y, z )
#endif // SHADOWMAP

    float4 Position : SV_POSITION; // 座標( プロジェクション空間 )
    float4 Pos : POSITION;
};

SamplerState    smp        : register(s0); //ディフューズマップテクスチャ
Texture2D       tex        : register(t0); //ディフューズマップテクスチャ

struct PS_OUTPUT
{
    float4 col : SV_TARGET0;
    float4 depth : SV_TARGET1;
    float4 norm : SV_TARGET2;
};


PS_OUTPUT main(PS_INPUT input) : SV_TARGET
{
    PS_OUTPUT ret;
    ret.depth = input.Position.zwyx;
   // ret.col = float4(0.0, 1.0, 0.0, 1.0);
    //ret.col = input.Diffuse;
    //ret.col = float4(input.VNormal, 1);

    ret.col = input.Diffuse * tex.Sample(smp, input.TexCoords0_1.xy);
    ret.norm = float4(((input.VNormal  + 1) / 2), 1);
    //ret.col = float4((input.VNormal + 1) / 2, 1)* input.Diffuse;
    return ret;
}