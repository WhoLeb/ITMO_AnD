#include "City.h"

#include <iostream>
#include "utility.h"
#include "string"

int main()
{
	do
	{
		system("cls");
		City city;
		city.Run();

		system("cls");
		std::cout << "Хотите сыграть снова? Y/N";
	}
	while (ProcessOneshotInput());
}
