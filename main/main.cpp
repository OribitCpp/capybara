#include "Application.h"

int main(int argc, char* argv[]) {
	Application app(argc,argv);
	app.init();
	app.ready();
	app.execute();
	app.finish();
	app.exit();
	return 0;
}