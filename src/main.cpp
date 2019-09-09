#include <iostream>
#include <exception>
#include "Application.h"

int main(int argc, char** argv)
{
	try
	{
		application app{ argc, argv };
		app.run();
	}
	catch (const std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
		system("pause");
		return -1;
	}

	system("pause");
	return 0;
}
