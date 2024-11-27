#include "FileRead.h"
#include <fstream>
#include<string>
#include <iostream>

FileRead::FileRead() : 
	m_pLayerAddr(nullptr),
	m_fmfHeader{}
{
}

//-----------------------------------------------------------------------------
//	�f�X�g���N�^
//-----------------------------------------------------------------------------
FileRead::~FileRead()
{
	Close();
}
//-----------------------------------------------------------------------------
//	�}�b�v���J���ăf�[�^��ǂݍ���
// �����F	szFilePath	= �}�b�v�t�@�C���̃p�X
// �߂�l:	����I��	= TRUE
//			�G���[		= FALSE
//-----------------------------------------------------------------------------
bool FileRead::Open(const wchar_t *szFilePath)
{
	// ���ɊJ���Ă���ꍇ�͕���
	Close();

	// �t�@�C�����J��
	std::ifstream ifs(szFilePath, std::ios::binary);

	if (!ifs.is_open()) {
		std::cerr << "Failed to open the file." << std::endl;
	}

	// �w�b�_����ǂ�
	ifs.read((char*)&m_fmfHeader, sizeof(FMFHEADER));

	std::cout << "Read " << ifs.gcount() << " bytes." << std::endl;

	// ���ʎq�̃`�F�b�N
	if (memcmp(&m_fmfHeader.dwIdentifier, "FMF_", 4) != 0)
	{
		//�G���[���b�Z�[�W��\��
		//std::cerr << "Error: file not opened." << std::endl;
		return 1;
	}

	// �������m��
	m_pLayerAddr = new BYTE[m_fmfHeader.dwSize];
	if (m_pLayerAddr == NULL)
	{
		//�G���[���b�Z�[�W��\��
		std::cerr << "Error: memory allocation failed." << std::endl;
		return 1;
	}

	//���C���f�[�^��ǂݍ���
	ifs.read((char*)&m_pLayerAddr, 10);

	std::cout << "Read " << ifs.gcount() << " bytes." << std::endl;

	return true;

//	// �t�@�C�����J��
//	HANDLE hFile = CreateFile(szFilePath, GENERIC_READ, FILE_SHARE_READ, NULL,
//		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//	if (hFile == INVALID_HANDLE_VALUE)
//		return FALSE;
//
//	// �w�b�_����ǂ�
//	DWORD dwReadBytes;
//	if (!ReadFile(hFile, &m_fmfHeader, sizeof(FMFHEADER), &dwReadBytes, NULL) ||
//		dwReadBytes != sizeof(FMFHEADER))
//		goto error_return;
//
//	// ���ʎq�̃`�F�b�N
//	if (memcmp(&m_fmfHeader.dwIdentifier, "FMF_", 4) != 0)
//		goto error_return;
//
//	// �������m��
//	m_pLayerAddr = new BYTE[m_fmfHeader.dwSize];
//	if (m_pLayerAddr == NULL)
//		goto error_return;
//
//	// ���C���[�f�[�^��ǂ�
//	if (!ReadFile(hFile, m_pLayerAddr, m_fmfHeader.dwSize, &dwReadBytes, NULL) ||
//		dwReadBytes != m_fmfHeader.dwSize)
//		goto error_return;
//
//	// ����I��
//	CloseHandle(hFile);
//	return TRUE;
//
//error_return:
//	// �G���[�I��
//	CloseHandle(hFile);
//	Close();
//	return FALSE;
}
//-----------------------------------------------------------------------------
// �}�b�v���J����Ă��邩
//-----------------------------------------------------------------------------
bool FileRead::IsOpen() const
{
	return m_pLayerAddr != nullptr;
}
//-----------------------------------------------------------------------------
//	�}�b�v���������J��
//-----------------------------------------------------------------------------
void FileRead::Close(void)
{
	if (m_pLayerAddr != NULL)
	{
		delete[] m_pLayerAddr;
		m_pLayerAddr = NULL;
	}
}

//-----------------------------------------------------------------------------
//	�w�背�C���̐擪�A�h���X�𓾂�
//	�����F	���C���ԍ�
//	�߂�l:	����I��	= ���C���f�[�^�̃A�h���X
//			�G���[		= NULL
//	�e���C���f�[�^�͘A�������������̈�ɔz�u����Ă�̂�
//	�w�背�C���f�[�^�̃A�h���X���v�Z�ŋ��߂�B
//-----------------------------------------------------------------------------
void* FileRead::GetLayerAddr(byte byLayerIndex) const
{
	// �������`�F�b�N�A�͈̓`�F�b�N
	if ((m_pLayerAddr == NULL) || (byLayerIndex >= m_fmfHeader.byLayerCount))
		return NULL;

	BYTE bySize = m_fmfHeader.byBitCount / 8;
	return m_pLayerAddr + m_fmfHeader.dwWidth * m_fmfHeader.dwHeight * bySize * byLayerIndex;
}

//-----------------------------------------------------------------------------
// ���C���ԍ��ƍ��W���w�肵�Ē��ڃf�[�^��Ⴄ
// �����F
// 	byLayerIndex	= ���C���ԍ�
// 	dwX				= X���W�i0�`m_fmfHeader.dwWidth - 1�j
// 	dwY				= Y���W�i0�`m_fmfHeader.dwHeight - 1�j
// �߂�l�F
// 	����I��	= ���W�̒l
//	�G���[		= -1
//-----------------------------------------------------------------------------
int FileRead::GetValue(BYTE byLayerIndex, DWORD dwX, DWORD dwY) const
{
	int nIndex = -1;

	// �͈̓`�F�b�N
	if (byLayerIndex >= m_fmfHeader.byLayerCount ||
		dwX >= m_fmfHeader.dwWidth ||
		dwY >= m_fmfHeader.dwHeight)
		return nIndex;

	if (m_fmfHeader.byBitCount == 8)
	{
		// 8bit layer
		BYTE* pLayer = (BYTE*)GetLayerAddr(byLayerIndex);
		nIndex = *(pLayer + dwY * m_fmfHeader.dwWidth + dwX);
	}
	else
	{
		// 16bit layer	
		WORD* pLayer = (WORD*)GetLayerAddr(byLayerIndex);
		nIndex = *(pLayer + dwY * m_fmfHeader.dwWidth + dwX);
	}


	return nIndex;
}

//-----------------------------------------------------------------------------
// ���C���ԍ��ƍ��W���w�肵�ăf�[�^���Z�b�g
//-----------------------------------------------------------------------------
void FileRead::SetValue(BYTE byLayerIndex, DWORD dwX, DWORD dwY, int nValue)
{
	// �͈̓`�F�b�N
	if (byLayerIndex >= m_fmfHeader.byLayerCount ||
		dwX >= m_fmfHeader.dwWidth ||
		dwY >= m_fmfHeader.dwHeight)
		return;

	if (m_fmfHeader.byBitCount == 8)
	{
		// 8bit layer
		BYTE* pLayer = (BYTE*)GetLayerAddr(byLayerIndex);
		*(pLayer + dwY * m_fmfHeader.dwWidth + dwX) = (BYTE)nValue;
	}
	else
	{
		// 16bit layer	
		WORD* pLayer = (WORD*)GetLayerAddr(byLayerIndex);
		*(pLayer + dwY * m_fmfHeader.dwWidth + dwX) = (WORD)nValue;
	}
}

//-----------------------------------------------------------------------------
// �}�b�v�̉����𓾂�
//-----------------------------------------------------------------------------
DWORD FileRead::GetMapWidth(void) const
{
	return m_fmfHeader.dwWidth;
}
//-----------------------------------------------------------------------------
// �}�b�v�̍����𓾂�
//-----------------------------------------------------------------------------
DWORD FileRead::GetMapHeight(void) const
{
	return m_fmfHeader.dwHeight;
}
//-----------------------------------------------------------------------------
// �`�b�v�̉����𓾂�
//-----------------------------------------------------------------------------
BYTE FileRead::GetChipWidth(void) const
{
	return m_fmfHeader.byChipWidth;
}
//-----------------------------------------------------------------------------
// �`�b�v�̍����𓾂�
//-----------------------------------------------------------------------------
BYTE FileRead::GetChipHeight(void) const
{
	return m_fmfHeader.byChipHeight;
}
//-----------------------------------------------------------------------------
// ���C���[���𓾂�
//-----------------------------------------------------------------------------
BYTE FileRead::GetLayerCount(void) const
{
	return m_fmfHeader.byLayerCount;
}
//-----------------------------------------------------------------------------
// ���C���[�f�[�^�̃r�b�g�J�E���g�𓾂�
//-----------------------------------------------------------------------------
BYTE FileRead::GetLayerBitCount(void) const
{
	return m_fmfHeader.byBitCount;
}
