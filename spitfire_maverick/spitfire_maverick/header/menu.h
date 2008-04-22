#ifndef __MENU1__
#define __MENU1__

class Menu : public State{
	public:
		Menu();
		~Menu();
		void run();
		void reset();
		NAME getMyName(); //Not implemented yet
	protected:
		void init();
};
#endif

