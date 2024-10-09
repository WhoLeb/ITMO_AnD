#pragma once

#include <conio.h>
#include <iostream>
#include <string>


inline bool ProcessOneshotInput(bool isValidated = true, char acceptChar = 'Y', char denyChar = 'N')
{
	if (isValidated)
	{
		while (true)
		{
			char input = 0;

			input = std::toupper(_getch());

			if (input == std::toupper(acceptChar))
				return true;
			if (input == std::toupper(denyChar))
				return false;
		}
	}
	_getch();
	return true;
}

inline uint32_t ProcessIntegerInput(std::string& input, const std::string& errorOutput = "Only integer input is allowed. Try again. ")
{
	while (std::cin >> input)
	{
		try
		{
			return std::stoi(input);
		}
		catch (const std::exception& err)
		{
			std::cout << errorOutput;
		}
	}
	return -1;
}
