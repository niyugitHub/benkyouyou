#include<iostream>
#include <fstream>
#include<string>
#include<vector>

namespace
{
	const char* const FILE_NAME = "fmfData/Test_2.fmf";
}

int main()
{
	//fmfファイルの読み込み
	std::ifstream ifs(FILE_NAME);

	//ファイルが開けなかった場合
	if (!ifs)
	{
		//エラーメッセージを表示
		std::cerr << "Error: file not opened." << std::endl;
		return 1;
	}

	//読込サイズを調べる
	ifs.seekg(0, std::ios::end);
	int size = ifs.tellg();
	ifs.seekg(0);

	//サイズを表示
	std::cout << "size: " << size << std::endl;

	//20バイト分の無駄なデータを読み飛ばす
	//なぜか最初の20バイトが無駄なデータになっている(ごみ)
	for (int i = 0; i < 20; i++)
	{
		char tmp;

		//1バイトずつ読み込む(計20バイト読み込む)
		ifs.read(&tmp, sizeof(char));
	}

	//データを読み込む(実際に読み込みたいデータ)
	std::vector<unsigned __int8> data;
	for (char i = 0; i < size - 20; i++)
	{
		unsigned __int8 tmp;
		ifs.read((char*)&tmp, sizeof(char));

		//値がマイナスの場合
		data.push_back(tmp);
	}

	//データを表示
	for (int i = 0; i < data.size(); i++)
	{
		std::cout << "配列番号" << i << " : " << static_cast<int>(data[i]) << std::endl;
	}

	return 0;
}