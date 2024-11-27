#include<iostream>
#include <fstream>
#include<string>
#include<vector>
#include"FileRead.h"
#include"fmfmap.h"

namespace
{
	const wchar_t* FILE_NAME = L"fmfData/Map.fmf";
}

int main()
{
	////fmfファイルの読み込み
	//std::ifstream ifs(FILE_NAME);

	////ファイルが開けなかった場合
	//if (!ifs)
	//{
	//	//エラーメッセージを表示
	//	std::cerr << "Error: file not opened." << std::endl;
	//	return 1;
	//}

	////読込サイズを調べる
	//ifs.seekg(0, std::ios::end);
	//int size = ifs.tellg();
	//ifs.seekg(0);

	////サイズを表示
	//std::cout << "size: " << size << std::endl;

	////20バイト分の無駄なデータを読み飛ばす
	////なぜか最初の20バイトが無駄なデータになっている(ごみ)
	//for (int i = 0; i < 20; i++)
	//{
	//	char tmp;

	//	//1バイトずつ読み込む(計20バイト読み込む)
	//	ifs.read(&tmp, sizeof(char));
	//}

	////データを読み込む(実際に読み込みたいデータ)
	//std::vector<unsigned __int8> data;
	//for (char i = 0; i < size - 20; i++)
	//{
	//	unsigned __int8 tmp;
	//	ifs.read((char*)&tmp, sizeof(char));

	//	//値がマイナスの場合
	//	data.push_back(tmp);
	//}

	////データを表示
	//for (int i = 0; i < data.size(); i++)
	//{
	//	std::cout << "配列番号" << i << " : " << static_cast<int>(data[i]) << std::endl;
	//}












	////ファイル読み込みクラスのインスタンスを生成
	//FileRead fileRead;

	////ファイルを開く
	//if (!fileRead.Open(FILE_NAME))	


	////ファイル情報を見る
	//std::cout << "Width: " << fileRead.GetMapWidth() << std::endl;
	//std::cout << "Height: " << fileRead.GetMapHeight() << std::endl;
	//std::cout << "ChipWidth: " << static_cast<int>(fileRead.GetChipWidth()) << std::endl;
	//std::cout << "ChipHeight: " << static_cast<int>(fileRead.GetChipHeight()) << std::endl;
	//std::cout << "LayerCount: " << static_cast<int>(fileRead.GetLayerCount()) << std::endl;
	//std::cout << "LayerBitCount: " << static_cast<int>(fileRead.GetLayerBitCount()) << std::endl;


	

	//CFmfMap fmfMap;
	FileRead fmfMap;

	////ファイルを開く
	//if (!fmfMap.Open(L"fmfData/Map.fmf"))
	//{
	//	return 1;
	//}

	//ファイルを開く
	if (!fmfMap.Open(FILE_NAME))
	{
		return 1;
	}

	//ファイル情報を見る
	std::cout << "Width: " << fmfMap.GetMapWidth() << std::endl;
	std::cout << "Height: " << fmfMap.GetMapHeight() << std::endl;
	std::cout << "ChipWidth: " << static_cast<int>(fmfMap.GetChipWidth()) << std::endl;
	std::cout << "ChipHeight: " << static_cast<int>(fmfMap.GetChipHeight()) << std::endl;
	std::cout << "LayerCount: " << static_cast<int>(fmfMap.GetLayerCount()) << std::endl;
	std::cout << "LayerBitCount: " << static_cast<int>(fmfMap.GetLayerBitCount()) << std::endl;

	//値を取得
	for (int i = 0; i < fmfMap.GetMapHeight(); i++)
	{
		for (int j = 0; j < fmfMap.GetMapWidth(); j++)
		{
			std::cout << fmfMap.GetValue(0, j, i) << " ";
		}
		std::cout << std::endl;
	}
	//std::cout << "Value: " << fmfMap.GetValue(0, 0, 0) << std::endl;


	return 0;
}