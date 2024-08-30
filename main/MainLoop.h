#ifndef  MAIN_LOOP_H
#define MAIN_LOOP_H

class MainLoop {
public:
	MainLoop();
	virtual void init(){}
	virtual void ready(){}
	virtual void execute();
	virtual void finish(){}
	virtual void exit(){}

protected:
	virtual bool update();
};

#endif // ! MAIN_LOOP_H
