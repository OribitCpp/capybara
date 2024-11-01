import Application;
import <iostream>;
#include <exception>

int main(int argc, char* argv[]) {
	try
	{
		Application app(argc,argv);
		app.init();
		app.ready();
		app.execute();
		app.finish();
		app.exit();
	}catch (std::exception e)
	{
		std::cerr << e.what()<<std::endl;
	}

	return 0;
}
