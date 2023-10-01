#ifndef FPS_HPP
#define FPS_HPP 
#include <SFML/System/Clock.hpp>

class FPS
{ 
public:
	FPS() : mFrame(0), mFps(0) {}

	const unsigned int getFPS() const { return mFps; }

private:
	unsigned int mFrame;
	unsigned int mFps;
	sf::Clock mClock;

public:
	void update()
	{
		if(mClock.getElapsedTime().asSeconds() >= 1.f)
		{
			mFps = mFrame;
			mFrame = 0;
			mClock.restart();
		}
 
		++mFrame;
	}
};
#endif 
