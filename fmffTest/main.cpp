#include<iostream>
#include <fstream>
#include<string>
#include<vector>

namespace
{
	const char* const FILE_NAME = "fmfData/Test.fmf";
}

int main()
{
	//fmf�t�@�C���̓ǂݍ���
	std::ifstream ifs(FILE_NAME);

	//�t�@�C�����J���Ȃ������ꍇ
	if (!ifs)
	{
		//�G���[���b�Z�[�W��\��
		std::cerr << "Error: file not opened." << std::endl;
		return 1;
	}

	//�Ǎ��T�C�Y�𒲂ׂ�
	ifs.seekg(0, std::ios::end);
	int size = ifs.tellg();
	ifs.seekg(0);

	//�T�C�Y��\��
	std::cout << "size: " << size << std::endl;

	//20�o�C�g���̖��ʂȃf�[�^��ǂݔ�΂�
	//�Ȃ����ŏ���20�o�C�g�����ʂȃf�[�^�ɂȂ��Ă���(����)
	for (int i = 0; i < 20; i++)
	{
		char tmp;
		//1�o�C�g���ǂݍ���(�v20�o�C�g�ǂݍ���)
		ifs.read(&tmp, sizeof(char));
	}

	//�f�[�^��ǂݍ���(���ۂɓǂݍ��݂����f�[�^)
	std::vector<char> data;
	for (char i = 0; i < size - 20; i++)
	{
		char tmp;
		ifs.read((char*)&tmp, sizeof(char));
		data.push_back(tmp);
	}

	//�f�[�^��\��
	for (int i = 0; i < data.size(); i++)
	{
		std::cout << "�z��ԍ�" << i << " : " << static_cast<int>(data[i]) << std::endl;
	}

	return 0;
}