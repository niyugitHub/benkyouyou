#include "VertexShader.h"

// ���_�V�F�[�_�[�̓���
struct VS_INPUT
{
    float3 Position : POSITION; // ���W( ���[�J����� )
    float3 Normal : NORMAL0; // �@��( ���[�J����� )
    float4 Diffuse : COLOR0; // �f�B�t���[�Y�J���[
    float4 Specular : COLOR1; // �X�y�L�����J���[
    float4 TexCoords0 : TEXCOORD0; // �e�N�X�`�����W
    float4 TexCoords1 : TEXCOORD1; // �T�u�e�N�X�`�����W

#ifdef BUMPMAP
	// �o���v�}�b�v
	float3 Tan             : TANGENT0 ;			// �ڐ�( ���[�J����� )
	float3 Bin             : BINORMAL0 ;		// �]�@��( ���[�J����� )
#endif // BUMPMAP

#ifdef SKINMESH
	// �X�L�j���O���b�V��
	int4   BlendIndices0   : BLENDINDICES0 ;	// �{�[�������p Float�^�萔�z��C���f�b�N�X�O
	float4 BlendWeight0    : BLENDWEIGHT0 ;		// �{�[�������p�E�G�C�g�l�O

#ifdef BONE8

		int4   BlendIndices1   : BLENDINDICES1 ;	// �{�[�������p Float�^�萔�z��C���f�b�N�X�P
		float4 BlendWeight1    : BLENDWEIGHT1 ;		// �{�[�������p�E�G�C�g�l�P

#endif // BONE8

#endif // SKINMESH
};

// ���_�V�F�[�_�[�̏o��
struct VS_OUTPUT
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

cbuffer ShadowCb : register(b4)
{
    float4x4 mLVP;
};

#ifdef SKINMESH
#define LOCAL_WORLD_MAT         lLocalWorldMatrix
#else
#define LOCAL_WORLD_MAT         g_Base.LocalWorldMatrix
#endif






// main�֐�
VS_OUTPUT main(VS_INPUT VSInput)
{
    VS_OUTPUT VSOutput;
    int4 lBoneFloatIndex;
    float4 lLocalWorldMatrix[3];
    float4 lLocalPosition;
    float4 lWorldPosition;
    float4 lViewPosition;
    float3 lWorldNrm;
    float3 lWorldTan;
    float3 lWorldBin;
    float3 lViewNrm;
    float3 lViewTan;
    float3 lViewBin;
#if SHADOWMAP
	float4		lLViewPosition ;
#endif


	// ���_���W�ϊ� ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++( �J�n )

	// ���[�J�����W�̃Z�b�g
    lLocalPosition.xyz = VSInput.Position;
    lLocalPosition.w = 1.0f;

	// ���W�v�Z( ���[�J�����r���[���v���W�F�N�V���� )
    lWorldPosition.x = dot(lLocalPosition, LOCAL_WORLD_MAT[0]);
    lWorldPosition.y = dot(lLocalPosition, LOCAL_WORLD_MAT[1]);
    lWorldPosition.z = dot(lLocalPosition, LOCAL_WORLD_MAT[2]);
    lWorldPosition.w = 1.0f;

    lViewPosition.x = dot(lWorldPosition, g_Base.ViewMatrix[0]);
    lViewPosition.y = dot(lWorldPosition, g_Base.ViewMatrix[1]);
    lViewPosition.z = dot(lWorldPosition, g_Base.ViewMatrix[2]);
    lViewPosition.w = 1.0f;

    VSOutput.Position.x = dot(lViewPosition, g_Base.ProjectionMatrix[0]);
    VSOutput.Position.y = dot(lViewPosition, g_Base.ProjectionMatrix[1]);
    VSOutput.Position.z = dot(lViewPosition, g_Base.ProjectionMatrix[2]);
    VSOutput.Position.w = dot(lViewPosition, g_Base.ProjectionMatrix[3]);
	
	// ���W( �r���[��� )��ۑ�
    VSOutput.VPosition = lViewPosition.xyz;
    
    //VSOutput.Position = mul(float3(100.0f, 300.0f, -200.0f), VSOutput.VPosition);
    //VSOutput.Position = mul(float3(0.0f, 0.0f, 0.0f), VSOutput.VPosition);
    //VSOutput.VPosition = float3(0, 0, 0);
    //VSOutput.Position.xyz = float3(0, 0, 0);
	
	// �@�����v�Z
    lWorldNrm.x = dot(VSInput.Normal, LOCAL_WORLD_MAT[0].xyz);
    lWorldNrm.y = dot(VSInput.Normal, LOCAL_WORLD_MAT[1].xyz);
    lWorldNrm.z = dot(VSInput.Normal, LOCAL_WORLD_MAT[2].xyz);

    lViewNrm.x = dot(lWorldNrm, g_Base.ViewMatrix[0].xyz);
    lViewNrm.y = dot(lWorldNrm, g_Base.ViewMatrix[1].xyz);
    lViewNrm.z = dot(lWorldNrm, g_Base.ViewMatrix[2].xyz);

	// �@��( �r���[��� )��ۑ�
    VSOutput.VNormal = lViewNrm;
    
	// �f�B�t���[�Y�J���[���Z�b�g
    VSOutput.Diffuse = g_Base.DiffuseSource > 0.5f ? VSInput.Diffuse : g_Common.Material.Diffuse;
	
	// �X�y�L�����J���[���Z�b�g
    VSOutput.Specular = (g_Base.SpecularSource > 0.5f ? VSInput.Specular : g_Common.Material.Specular) * g_Base.MulSpecularColor;

    //���C�g�r���[�X�N���[����Ԃ̍��W���v�Z����
    VSOutput.posInLVP = mul(mLVP, lWorldPosition);
    
	// ���_���W�ϊ� ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++( �I�� )





	// �t�H�O�v�Z =============================================( �J�n )

#if FOG_LINEAR || FOG_EXP || FOG_EXP2

#ifdef FOG_LINEAR

			// ���`�t�H�O�v�Z
			VSOutput.Fog.x = lViewPosition.z * g_Common.Fog.LinearDiv + g_Common.Fog.LinearAdd ;

#endif

#ifdef FOG_EXP

			// �w���t�H�O�v�Z 1.0f / pow( e, ���� * density )
			VSOutput.Fog.x = 1.0f / pow( abs( g_Common.Fog.E ), lViewPosition.z * g_Common.Fog.Density ) ;

#endif

#ifdef FOG_EXP2

			// �w���t�H�O�Q�v�Z 1.0f / pow( e, ( ���� * density ) * ( ���� * density ) )
			VSOutput.Fog.x = 1.0f / pow( abs( g_Common.Fog.E ), ( lViewPosition.z * g_Common.Fog.Density ) * ( lViewPosition.z * g_Common.Fog.Density ) ) ;

#endif

#else // FOG_LINEAR || FOG_EXP || FOG_EXP2
	
    VSOutput.Fog.x = 1.0f;

#endif // FOG_LINEAR || FOG_EXP || FOG_EXP2

	// �t�H�O�v�Z =============================================( �I�� )

#if SHADOWMAP
	// �[�x�e�p�̃��C�g���猩���ˉe���W���Z�o =================( �J�n )

	// ���[���h���W���V���h�E�}�b�v�O�̃��C�g�ݒ�̎ˉe���W�ɕϊ�
	VSOutput.ShadowMap0Pos.x = dot( lWorldPosition, g_OtherMatrix.ShadowMapLightViewProjectionMatrix[ 0 ][ 0 ] ) ;
	VSOutput.ShadowMap0Pos.y = dot( lWorldPosition, g_OtherMatrix.ShadowMapLightViewProjectionMatrix[ 0 ][ 1 ] ) ;
	VSOutput.ShadowMap0Pos.z = dot( lWorldPosition, g_OtherMatrix.ShadowMapLightViewProjectionMatrix[ 0 ][ 2 ] ) ;

	// ���[���h���W���V���h�E�}�b�v�P�̃��C�g�ݒ�̎ˉe���W�ɕϊ�
	VSOutput.ShadowMap1Pos.x = dot( lWorldPosition, g_OtherMatrix.ShadowMapLightViewProjectionMatrix[ 1 ][ 0 ] ) ;
	VSOutput.ShadowMap1Pos.y = dot( lWorldPosition, g_OtherMatrix.ShadowMapLightViewProjectionMatrix[ 1 ][ 1 ] ) ;
	VSOutput.ShadowMap1Pos.z = dot( lWorldPosition, g_OtherMatrix.ShadowMapLightViewProjectionMatrix[ 1 ][ 2 ] ) ;

	// ���[���h���W���V���h�E�}�b�v�Q�̃��C�g�ݒ�̎ˉe���W�ɕϊ�
	VSOutput.ShadowMap2Pos.x = dot( lWorldPosition, g_OtherMatrix.ShadowMapLightViewProjectionMatrix[ 2 ][ 0 ] ) ;
	VSOutput.ShadowMap2Pos.y = dot( lWorldPosition, g_OtherMatrix.ShadowMapLightViewProjectionMatrix[ 2 ][ 1 ] ) ;
	VSOutput.ShadowMap2Pos.z = dot( lWorldPosition, g_OtherMatrix.ShadowMapLightViewProjectionMatrix[ 2 ][ 2 ] ) ;

	// �[�x�e�p�̃��C�g���猩���ˉe���W���Z�o =================( �I�� )
#endif

	// �e�N�X�`�����W�̃Z�b�g
    VSOutput.TexCoords0_1.x = dot(VSInput.TexCoords0, g_OtherMatrix.TextureMatrix[0][0]);
    VSOutput.TexCoords0_1.y = dot(VSInput.TexCoords0, g_OtherMatrix.TextureMatrix[0][1]);

    VSOutput.TexCoords0_1.z = dot(VSInput.TexCoords1, g_OtherMatrix.TextureMatrix[0][0]);
    VSOutput.TexCoords0_1.w = dot(VSInput.TexCoords1, g_OtherMatrix.TextureMatrix[0][1]);

    return VSOutput;
}

