#pragma once

#include <map>
#include <SDL.h>
#include "vec.h"
#define BUTTON_COUNT 3

namespace input
{
	void init();
	void update();
	bool isKeyPressed(SDL_Keycode key);
	bool isKeyReleased(SDL_Keycode key);
	bool isKeyUp(SDL_Keycode key);
	bool isKeyDown(SDL_Keycode key);
	bool isWindowClosed();
	bool isButtonDown(int button);
	bool isButtonPressed(int button);
	bool isButtonReleased(int button);
	vec2 getMousePos();
	SDL_Keycode getKey(std::string name);

	bool isButtonHandled(int button);
	void handleButton(int button);
	void resetButtonHandling();
};