#ifndef __SPLASH1__
#define __SPLASH1__

class Splash : public State{
	public:
		Splash();
		~Splash();
		void run();
		void reset();
		NAME getMyName(); //Not implemented yet
	protected:
		void init();
	private:
};
#endif

