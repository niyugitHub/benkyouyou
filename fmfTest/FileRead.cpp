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
//	デストラクタ
//-----------------------------------------------------------------------------
FileRead::~FileRead()
{
	Close();
}
//-----------------------------------------------------------------------------
//	マップを開いてデータを読み込む
// 引数：	szFilePath	= マップファイルのパス
// 戻り値:	正常終了	= TRUE
//			エラー		= FALSE
//-----------------------------------------------------------------------------
bool FileRead::Open(const wchar_t *szFilePath)
{
	// 既に開いている場合は閉じる
	Close();

	// ファイルを開く
	std::ifstream ifs(szFilePath, std::ios::binary);

	if (!ifs.is_open()) {
		std::cerr << "Failed to open the file." << std::endl;
	}

	// ヘッダ情報を読む
	ifs.read((char*)&m_fmfHeader, sizeof(FMFHEADER));

	std::cout << "Read " << ifs.gcount() << " bytes." << std::endl;

	// 識別子のチェック
	if (memcmp(&m_fmfHeader.dwIdentifier, "FMF_", 4) != 0)
	{
		//エラーメッセージを表示
		//std::cerr << "Error: file not opened." << std::endl;
		return 1;
	}

	// メモリ確保
	m_pLayerAddr = new BYTE[m_fmfHeader.dwSize];
	if (m_pLayerAddr == NULL)
	{
		//エラーメッセージを表示
		std::cerr << "Error: memory allocation failed." << std::endl;
		return 1;
	}

	//レイヤデータを読み込む
	ifs.read((char*)&m_pLayerAddr, 10);

	std::cout << "Read " << ifs.gcount() << " bytes." << std::endl;

	return true;

//	// ファイルを開く
//	HANDLE hFile = CreateFile(szFilePath, GENERIC_READ, FILE_SHARE_READ, NULL,
//		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//	if (hFile == INVALID_HANDLE_VALUE)
//		return FALSE;
//
//	// ヘッダ情報を読む
//	DWORD dwReadBytes;
//	if (!ReadFile(hFile, &m_fmfHeader, sizeof(FMFHEADER), &dwReadBytes, NULL) ||
//		dwReadBytes != sizeof(FMFHEADER))
//		goto error_return;
//
//	// 識別子のチェック
//	if (memcmp(&m_fmfHeader.dwIdentifier, "FMF_", 4) != 0)
//		goto error_return;
//
//	// メモリ確保
//	m_pLayerAddr = new BYTE[m_fmfHeader.dwSize];
//	if (m_pLayerAddr == NULL)
//		goto error_return;
//
//	// レイヤーデータを読む
//	if (!ReadFile(hFile, m_pLayerAddr, m_fmfHeader.dwSize, &dwReadBytes, NULL) ||
//		dwReadBytes != m_fmfHeader.dwSize)
//		goto error_return;
//
//	// 正常終了
//	CloseHandle(hFile);
//	return TRUE;
//
//error_return:
//	// エラー終了
//	CloseHandle(hFile);
//	Close();
//	return FALSE;
}
//-----------------------------------------------------------------------------
// マップが開かれているか
//-----------------------------------------------------------------------------
bool FileRead::IsOpen() const
{
	return m_pLayerAddr != nullptr;
}
//-----------------------------------------------------------------------------
//	マップメモリを開放
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
//	指定レイヤの先頭アドレスを得る
//	引数：	レイヤ番号
//	戻り値:	正常終了	= レイヤデータのアドレス
//			エラー		= NULL
//	各レイヤデータは連続したメモリ領域に配置されてるので
//	指定レイヤデータのアドレスを計算で求める。
//-----------------------------------------------------------------------------
void* FileRead::GetLayerAddr(byte byLayerIndex) const
{
	// メモリチェック、範囲チェック
	if ((m_pLayerAddr == NULL) || (byLayerIndex >= m_fmfHeader.byLayerCount))
		return NULL;

	BYTE bySize = m_fmfHeader.byBitCount / 8;
	return m_pLayerAddr + m_fmfHeader.dwWidth * m_fmfHeader.dwHeight * bySize * byLayerIndex;
}

//-----------------------------------------------------------------------------
// レイヤ番号と座標を指定して直接データを貰う
// 引数：
// 	byLayerIndex	= レイヤ番号
// 	dwX				= X座標（0～m_fmfHeader.dwWidth - 1）
// 	dwY				= Y座標（0～m_fmfHeader.dwHeight - 1）
// 戻り値：
// 	正常終了	= 座標の値
//	エラー		= -1
//-----------------------------------------------------------------------------
int FileRead::GetValue(BYTE byLayerIndex, DWORD dwX, DWORD dwY) const
{
	int nIndex = -1;

	// 範囲チェック
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
// レイヤ番号と座標を指定してデータをセット
//-----------------------------------------------------------------------------
void FileRead::SetValue(BYTE byLayerIndex, DWORD dwX, DWORD dwY, int nValue)
{
	// 範囲チェック
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
// マップの横幅を得る
//-----------------------------------------------------------------------------
DWORD FileRead::GetMapWidth(void) const
{
	return m_fmfHeader.dwWidth;
}
//-----------------------------------------------------------------------------
// マップの高さを得る
//-----------------------------------------------------------------------------
DWORD FileRead::GetMapHeight(void) const
{
	return m_fmfHeader.dwHeight;
}
//-----------------------------------------------------------------------------
// チップの横幅を得る
//-----------------------------------------------------------------------------
BYTE FileRead::GetChipWidth(void) const
{
	return m_fmfHeader.byChipWidth;
}
//-----------------------------------------------------------------------------
// チップの高さを得る
//-----------------------------------------------------------------------------
BYTE FileRead::GetChipHeight(void) const
{
	return m_fmfHeader.byChipHeight;
}
//-----------------------------------------------------------------------------
// レイヤー数を得る
//-----------------------------------------------------------------------------
BYTE FileRead::GetLayerCount(void) const
{
	return m_fmfHeader.byLayerCount;
}
//-----------------------------------------------------------------------------
// レイヤーデータのビットカウントを得る
//-----------------------------------------------------------------------------
BYTE FileRead::GetLayerBitCount(void) const
{
	return m_fmfHeader.byBitCount;
}
