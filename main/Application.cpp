#include "Application.h"
#include <iostream>

Application::Application(int argc, char* argv[])
{
	parseConsoleParameters(argc, argv);
}

void Application::init() {
	Logger::info("Appplication Init Stage");
}

void Application::execute()
{
	while (keepRun)
	{
		processInput();
		update();
	}
}

bool Application::update() {
	return true;
}

void Application::parseConsoleParameters(int argc, char* argv[])
{
	// -proj-dir: the project directory
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i],"-help")==0 || strcmp(argv[i],"-h") == 0) {
		}
		else if (argv[i] == "") {

		}
	}
}

void Application::processInput()
{
	std::string inputData;
	std::getline(std::cin, inputData);

	if (inputData == "exit")
	{
		keepRun = false;
	}
	else if (inputData == "help") {
		std::cout << "All of parameters are displayed as follow:" << std::endl;
	}


}
