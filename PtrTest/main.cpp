#include<iostream>

int main()
{
	//�|�C���^���g���ĕϐ���錾
	int* p;

	//�z��œ��I�������m��
	p = new int[100];

	//�z��ɒl����
	for (int i = 0; i < 100; i++)
	{
		p[i] = i;
	}

	//�z��̒l��\��
	for (int i = 0; i < 100; i++)
	{
		std::cout << p[i] << std::endl;
	}

	//�z��̃������T�C�Y��\��
	std::cout << sizeof(p) << std::endl;
}