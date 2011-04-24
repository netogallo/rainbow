/// Simple game loop for Lua-scripted games.

/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef DIRECTOR_H_
#define DIRECTOR_H_

#include "ConFuoco/Mixer.h"
#include "Lua.h"

class Director
{
public:
	Director();

	void draw();

	/// Load and initialise game script.
	void init(const char *);

	/// Handle key press events.
	void key_press();

	/// Update world.
	/// \param dt  Time since last update
	void update(const float dt);

	/// Update screen resolution.
	void update_video(const int w = 0, const int h = 0);

private:
	Lua lua;  ///< Lua interpreter
};

inline void Director::key_press()
{
	this->lua.input.update(this->lua.L);
}

inline void Director::update_video(const int w, const int h)
{
	Screen::Instance().set(static_cast<float>(w), static_cast<float>(h));
	this->lua.platform.update(this->lua.L);
}

#endif
