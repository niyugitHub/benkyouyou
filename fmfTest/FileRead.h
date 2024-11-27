#pragma once

#include <windows.h>
#include <vector>

// FMF�t�@�C���w�b�_ (20 bytes)
typedef struct tag_FMFHeader
{
	DWORD	dwIdentifier;	// �t�@�C�����ʎq 'FMF_'
	DWORD	dwSize;			// �w�b�_���������f�[�^�T�C�Y
	DWORD	dwWidth;		// �}�b�v�̉���
	DWORD	dwHeight;		// �}�b�v�̍���
	BYTE	byChipWidth;	// �}�b�v�`�b�v1�̕�(pixel)
	BYTE	byChipHeight;	// �}�b�v�`�b�v�P�̍���(pixel)
	BYTE	byLayerCount;	// ���C���[�̐�
	BYTE	byBitCount;		// ���C���f�[�^�̃r�b�g�J�E���g
}FMFHEADER;

class FileRead
{
public:
	// �R���X�g���N�^
	FileRead();
	// �f�X�g���N�^
	~FileRead();

	/// <summary>
	/// �t�@�C�����J��
	/// </summary>
	/// <param name="szFilePath">�t�@�C���p�X</param>
	bool Open(const wchar_t *szFilePath);

	// �}�b�v���J����Ă��邩
	bool IsOpen() const;

	// �}�b�v�������J��
	void Close();

	/// <summary>
	/// �w�背�C���[�̐퓬�A�h���X�𓾂�
	/// <summary>
	/// <param name="byLayerIndex">���C���[�C���f�b�N�X</param>
	void* GetLayerAddr(byte byLayerIndex) const;

	/// <summary>
	///���C���ԍ��ƍ��W���w�肵�Ē��ڃf�[�^�����炤
	/// </summary>
	/// <param name="byLayerIndex">���C���[�C���f�b�N�X</param>
	/// <param name="dwX">X���W</param>
	/// <param name="dwY">Y���W</param>
	int GetValue(byte byLayerIndex, DWORD dwX, DWORD dwY) const;

	/// <summary>
	/// ���C���ԍ��ƍ��W���w�肵�ăf�[�^���Z�b�g
	/// </summary>
	/// <param name="byLayerIndex">���C���[�C���f�b�N�X</param>
	/// <param name="dwX">X���W</param>
	/// <param name="dwY">Y���W</param>
	/// <param name="nValue">�Z�b�g����l</param>
	void SetValue(BYTE byLayerIndex, DWORD dwX, DWORD dwY, int nValue);

	// �w�b�_�̏��𓾂�
	DWORD GetMapWidth(void) const;
	DWORD GetMapHeight(void) const;
	BYTE GetChipWidth(void) const;
	BYTE GetChipHeight(void) const;
	BYTE GetLayerCount(void) const;
	BYTE GetLayerBitCount(void) const;

private:
	// FMF�t�@�C���w�b�_�\����
	FMFHEADER	m_fmfHeader;
	// ���C���[�f�[�^�ւ̃|�C���^
	BYTE* m_pLayerAddr;

	// �t�@�C����ǂݍ���
	std::vector<BYTE> m_data1;
	std::vector<WORD> m_data2;
};

