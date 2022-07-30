#include <iostream>
#include <conio.h>
#include <iostream>
#include "BigFloat.h"

int main()
{
	std::cout << "Press any key to continue...\n";
	(void)_getch();
	system("cls");
	BigFloat f1("A68CFAC6759.6757AB69F", 16);
	BigFloat f2("B550.FF", 16);
	std::cout << f1 / f2 << std::endl;
	return 0;
}