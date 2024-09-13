#ifndef  APPLICATION_H
#define APPLICATION_H

#include "Logger.h"

class Application {
public:
	Application(int argc, char* argv[]);
	void init();
	void ready(){}
	void execute();
	void finish(){}
	void exit(){}

	int status() { return m_status; }
	void setStatus(int value) { m_status = value; }
protected:
	virtual bool update();

private:
	void parseConsoleParameters(int argc, char* argv[]);
	void processInput();
private:
	bool keepRun = true;
	int m_status = 0;
	Logger m_logger;
};

#endif 
