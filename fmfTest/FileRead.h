#pragma once

#include <windows.h>
#include <vector>

// FMFファイルヘッダ (20 bytes)
typedef struct tag_FMFHeader
{
	DWORD	dwIdentifier;	// ファイル識別子 'FMF_'
	DWORD	dwSize;			// ヘッダを除いたデータサイズ
	DWORD	dwWidth;		// マップの横幅
	DWORD	dwHeight;		// マップの高さ
	BYTE	byChipWidth;	// マップチップ1つの幅(pixel)
	BYTE	byChipHeight;	// マップチップ１つの高さ(pixel)
	BYTE	byLayerCount;	// レイヤーの数
	BYTE	byBitCount;		// レイヤデータのビットカウント
}FMFHEADER;

class FileRead
{
public:
	// コンストラクタ
	FileRead();
	// デストラクタ
	~FileRead();

	/// <summary>
	/// ファイルを開く
	/// </summary>
	/// <param name="szFilePath">ファイルパス</param>
	bool Open(const wchar_t *szFilePath);

	// マップが開かれているか
	bool IsOpen() const;

	// マップメモリ開放
	void Close();

	/// <summary>
	/// 指定レイヤーの戦闘アドレスを得る
	/// <summary>
	/// <param name="byLayerIndex">レイヤーインデックス</param>
	void* GetLayerAddr(byte byLayerIndex) const;

	/// <summary>
	///レイヤ番号と座標を指定して直接データをもらう
	/// </summary>
	/// <param name="byLayerIndex">レイヤーインデックス</param>
	/// <param name="dwX">X座標</param>
	/// <param name="dwY">Y座標</param>
	int GetValue(byte byLayerIndex, DWORD dwX, DWORD dwY) const;

	/// <summary>
	/// レイヤ番号と座標を指定してデータをセット
	/// </summary>
	/// <param name="byLayerIndex">レイヤーインデックス</param>
	/// <param name="dwX">X座標</param>
	/// <param name="dwY">Y座標</param>
	/// <param name="nValue">セットする値</param>
	void SetValue(BYTE byLayerIndex, DWORD dwX, DWORD dwY, int nValue);

	// ヘッダの情報を得る
	DWORD GetMapWidth(void) const;
	DWORD GetMapHeight(void) const;
	BYTE GetChipWidth(void) const;
	BYTE GetChipHeight(void) const;
	BYTE GetLayerCount(void) const;
	BYTE GetLayerBitCount(void) const;

private:
	// FMFファイルヘッダ構造体
	FMFHEADER	m_fmfHeader;
	// レイヤーデータへのポインタ
	BYTE* m_pLayerAddr;

	// ファイルを読み込む
	std::vector<BYTE> m_data1;
	std::vector<WORD> m_data2;
};

