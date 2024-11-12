// -------------------------------------------------------------------------------
// 
// 		�c�w���C�u����		�s�N�Z���V�F�[�_�[�֘A��`
// 
// 				Ver 3.24b
// 
// -------------------------------------------------------------------------------

#ifndef DX_SHADER_PS_D3D11_H
#define DX_SHADER_PS_D3D11_H

// �C���N���[�h ---------------------------------

#include "DxShader_Common_D3D11.h"

// �}�N����` -----------------------------------

#define DX_VERTEXLIGHTING_LIGHT_NUM			(3)		// ���_�P�ʃ��C�e�B���O�œ����Ɏg�p�ł��郉�C�g�̍ő吔
#define DX_PIXELLIGHTING_LIGHT_NUM			(6)		// �s�N�Z���P�ʃ��C�e�B���O�œ����Ɏg�p�ł��郉�C�g�̍ő吔

#define DX_D3D11_PS_CONST_FILTER_SIZE		(1280)	// �t�B���^�[�p�萔�o�b�t�@�̃T�C�Y

// �f�[�^�^��` ---------------------------------

// �V���h�E�}�b�v�p�����[�^
struct DX_D3D11_PS_CONST_SHADOWMAP
{
	DX_D3D11_SHADER_FLOAT		AdjustDepth ;			// 臒l�[�x�␳�l
	DX_D3D11_SHADER_FLOAT		GradationParam ;		// �O���f�[�V�����͈�
	DX_D3D11_SHADER_FLOAT		Enable_Light0 ;			// ���C�g�O�ւ̓K�p���
	DX_D3D11_SHADER_FLOAT		Enable_Light1 ;			// ���C�g�P�ւ̓K�p���

	DX_D3D11_SHADER_FLOAT		Enable_Light2 ;			// ���C�g�Q�ւ̓K�p���
	DX_D3D11_SHADER_FLOAT3		Padding ;				// �p�f�B���O
} ;

// �萔�o�b�t�@�s�N�Z���V�F�[�_�[��{�p�����[�^
struct DX_D3D11_PS_CONST_BUFFER_BASE
{
	DX_D3D11_SHADER_FLOAT4		FactorColor ;			// �A���t�@�l��

	DX_D3D11_SHADER_FLOAT		MulAlphaColor ;			// �J���[�ɃA���t�@�l����Z���邩�ǂ���( 0.0f:��Z���Ȃ�  1.0f:��Z���� )
	DX_D3D11_SHADER_FLOAT		AlphaTestRef ;			// �A���t�@�e�X�g�Ŏg�p�����r�l
	DX_D3D11_SHADER_FLOAT2		Padding1 ;

	DX_D3D11_SHADER_INT			AlphaTestCmpMode ;		// �A���t�@�e�X�g��r���[�h( DX_CMP_NEVER �Ȃ� )
	DX_D3D11_SHADER_INT			NoLightAngleAttenuation ;	// ���C�g�̊p�x�������s��Ȃ���( 0:�������s��   1:�������s��Ȃ� )
	DX_D3D11_SHADER_INT2		Padding2 ;

	DX_D3D11_SHADER_FLOAT4		IgnoreTextureColor ;	// �e�N�X�`���J���[���������p�J���[

	DX_D3D11_SHADER_FLOAT4		DrawAddColor ;			// ���Z����F
} ;


// �萔�o�b�t�@�V���h�E�}�b�v�p�����[�^
struct DX_D3D11_PS_CONST_BUFFER_SHADOWMAP
{
	DX_D3D11_PS_CONST_SHADOWMAP	Data[ 3 ] ;
} ;


#endif // DX_SHADER_PS_D3D11_H