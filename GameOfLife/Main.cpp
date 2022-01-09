#include <iostream>

#include "App.hpp"

int main(int argc, char** argv)
{
	if(!Init(1280, 720, "XD"))
	{
		std::cerr << "Failed to start the app, terminating now\n";

		return 1;
	}

	Run();

	return 0;
}