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
	////fmf�t�@�C���̓ǂݍ���
	//std::ifstream ifs(FILE_NAME);

	////�t�@�C�����J���Ȃ������ꍇ
	//if (!ifs)
	//{
	//	//�G���[���b�Z�[�W��\��
	//	std::cerr << "Error: file not opened." << std::endl;
	//	return 1;
	//}

	////�Ǎ��T�C�Y�𒲂ׂ�
	//ifs.seekg(0, std::ios::end);
	//int size = ifs.tellg();
	//ifs.seekg(0);

	////�T�C�Y��\��
	//std::cout << "size: " << size << std::endl;

	////20�o�C�g���̖��ʂȃf�[�^��ǂݔ�΂�
	////�Ȃ����ŏ���20�o�C�g�����ʂȃf�[�^�ɂȂ��Ă���(����)
	//for (int i = 0; i < 20; i++)
	//{
	//	char tmp;

	//	//1�o�C�g���ǂݍ���(�v20�o�C�g�ǂݍ���)
	//	ifs.read(&tmp, sizeof(char));
	//}

	////�f�[�^��ǂݍ���(���ۂɓǂݍ��݂����f�[�^)
	//std::vector<unsigned __int8> data;
	//for (char i = 0; i < size - 20; i++)
	//{
	//	unsigned __int8 tmp;
	//	ifs.read((char*)&tmp, sizeof(char));

	//	//�l���}�C�i�X�̏ꍇ
	//	data.push_back(tmp);
	//}

	////�f�[�^��\��
	//for (int i = 0; i < data.size(); i++)
	//{
	//	std::cout << "�z��ԍ�" << i << " : " << static_cast<int>(data[i]) << std::endl;
	//}












	////�t�@�C���ǂݍ��݃N���X�̃C���X�^���X�𐶐�
	//FileRead fileRead;

	////�t�@�C�����J��
	//if (!fileRead.Open(FILE_NAME))	


	////�t�@�C����������
	//std::cout << "Width: " << fileRead.GetMapWidth() << std::endl;
	//std::cout << "Height: " << fileRead.GetMapHeight() << std::endl;
	//std::cout << "ChipWidth: " << static_cast<int>(fileRead.GetChipWidth()) << std::endl;
	//std::cout << "ChipHeight: " << static_cast<int>(fileRead.GetChipHeight()) << std::endl;
	//std::cout << "LayerCount: " << static_cast<int>(fileRead.GetLayerCount()) << std::endl;
	//std::cout << "LayerBitCount: " << static_cast<int>(fileRead.GetLayerBitCount()) << std::endl;


	

	//CFmfMap fmfMap;
	FileRead fmfMap;

	////�t�@�C�����J��
	//if (!fmfMap.Open(L"fmfData/Map.fmf"))
	//{
	//	return 1;
	//}

	//�t�@�C�����J��
	if (!fmfMap.Open(FILE_NAME))
	{
		return 1;
	}

	//�t�@�C����������
	std::cout << "Width: " << fmfMap.GetMapWidth() << std::endl;
	std::cout << "Height: " << fmfMap.GetMapHeight() << std::endl;
	std::cout << "ChipWidth: " << static_cast<int>(fmfMap.GetChipWidth()) << std::endl;
	std::cout << "ChipHeight: " << static_cast<int>(fmfMap.GetChipHeight()) << std::endl;
	std::cout << "LayerCount: " << static_cast<int>(fmfMap.GetLayerCount()) << std::endl;
	std::cout << "LayerBitCount: " << static_cast<int>(fmfMap.GetLayerBitCount()) << std::endl;

	//�l���擾
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