#include<iostream>

int main()
{
	//ポインタを使って変数を宣言
	int* p;

	//配列で動的メモリ確保
	p = new int[100];

	//配列に値を代入
	for (int i = 0; i < 100; i++)
	{
		p[i] = i;
	}

	//配列の値を表示
	for (int i = 0; i < 100; i++)
	{
		std::cout << p[i] << std::endl;
	}

	//配列のメモリサイズを表示
	std::cout << sizeof(p) << std::endl;
}