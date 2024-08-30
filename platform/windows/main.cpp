#include "SystemWindows.h"

int main(int argc, char* argv) {
	SystemWindows sysWin;
	sysWin.init();
	sysWin.ready();
	sysWin.execute();
	sysWin.finish();
	sysWin.exit();
	return 0;
}