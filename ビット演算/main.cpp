#include <iostream>

void f(int* x, int* y)
{
	if (x && y)
	{
		*x ^= *y;
		*y ^= *x;
		*x ^= *y;
	}
}


int main()
{
	int a = 5;
	int b = 10;

	f(&a, &b);

	std::cout << "a = " << a << std::endl;
	std::cout << "b = " << b << std::endl;

	return 0;
}