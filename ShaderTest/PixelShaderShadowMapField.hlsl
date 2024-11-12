
// �s�N�Z���V�F�[�_�[�̓���
struct PS_INPUT
{
    float4 Diffuse : COLOR0; // �f�B�t���[�Y�J���[
    float4 Specular : COLOR1; // �X�y�L�����J���[
    float4 TexCoords0_1 : TEXCOORD0; // xy:�e�N�X�`�����W zw:�T�u�e�N�X�`�����W
    float3 VPosition : TEXCOORD1; // ���W( �r���[��� )
    float3 VNormal : TEXCOORD2; // �@��( �r���[��� )
#ifdef BUMPMAP
	float3 VTan            : TEXCOORD3 ;    // �ڐ�( �r���[��� )
	float3 VBin            : TEXCOORD4 ;    // �]�@��( �r���[��� )
#endif // BUMPMAP
    float1 Fog : TEXCOORD5; // �t�H�O�p�����[�^( x )

#if SHADOWMAP
	float3 ShadowMap0Pos   : TEXCOORD6 ;	// �V���h�E�}�b�v�O�̃��C�g���W( x, y, z )
	float3 ShadowMap1Pos   : TEXCOORD7 ;	// �V���h�E�}�b�v�P�̃��C�g���W( x, y, z )
	float3 ShadowMap2Pos   : TEXCOORD8 ;	// �V���h�E�}�b�v�Q�̃��C�g���W( x, y, z )
#endif // SHADOWMAP

    float4 Position : SV_POSITION; // ���W( �v���W�F�N�V������� )
    //���C�g�r���[�X�N���[����Ԃł̍��W��ǉ�
    float4 posInLVP : TEXCOORD3; // ���C�g�r���[�X�N���[����Ԃł̃s�N�Z���̍��W
};

SamplerState smp : register(s0); // �f�B�t���[�Y�}�b�v�e�N�X�`��
Texture2D tex : register(t0); // �f�B�t���[�Y�}�b�v�e�N�X�`��

Texture2D shadowMapSmp : register(t1);

struct PS_OUTPUT
{
    float4 col : SV_TARGET0;
    float4 depth : SV_TARGET1;
    float4 norm : SV_TARGET2;
};

PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT ret;
    ret.depth = input.Position.zwyx;
    ret.col = input.Diffuse * tex.Sample(smp, input.TexCoords0_1.xy);
    ret.norm = float4((input.VNormal + 1) / 2, 1);
    
    float2 uv = input.posInLVP.xy / input.posInLVP.w;
    uv = uv * float2(0.5f, 0.5f);
    uv += 0.5f;
    
    //�e�����]��h������
    uv.y *= -1;
    uv.y += 1.0f;
    
    float3 shadowMap = 1.0f;
    
    if (uv.x > 0.0f && uv.x < 1.0f &&
        uv.y > 0.0f && uv.y < 1.0f)
    {
        shadowMap = shadowMapSmp.Sample(smp, uv);
    }
    
    //float2 uv = input.TexCoords0_1.xy;
    //uv = uv * float2(0.5f, 0.5f);
    //uv += 0.5f;
    
    ret.col.rgb = ret.col.rgb * shadowMap;
    
    //ret.col = input.posInLVP;
    return ret;
}