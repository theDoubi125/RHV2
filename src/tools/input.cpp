#include <SDL.h>
#include "input.h"
#include "name.h"
#include "vec.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl_gl3.h"

#define INPUT_QUEUE_SIZE 10

namespace
{
	std::map<SDL_Keycode, int> m_keyStates;
	bool m_closeWindow = false;
	int m_frame = 1;
	SDL_Event m_event;
	vec2 m_mousePos;
	int m_buttonStates[BUTTON_COUNT];
	int m_buttonHandled = 0;
}

std::map<Name, SDL_Keycode> m_availableKeys;
namespace input
{

	void init()
	{
		m_availableKeys[Name("up")] = SDLK_UP;
		m_availableKeys[Name("down")] = SDLK_DOWN;
		m_availableKeys[Name("left")] = SDLK_LEFT;
		m_availableKeys[Name("right")] = SDLK_RIGHT;
		m_availableKeys[Name("a")] = SDLK_a;
		m_availableKeys[Name("b")] = SDLK_b;
		m_availableKeys[Name("c")] = SDLK_c;
		m_availableKeys[Name("d")] = SDLK_d;
		m_availableKeys[Name("e")] = SDLK_e;
		m_availableKeys[Name("f")] = SDLK_f;
		m_availableKeys[Name("g")] = SDLK_g;
		m_availableKeys[Name("h")] = SDLK_h;
		m_availableKeys[Name("i")] = SDLK_i;
		m_availableKeys[Name("j")] = SDLK_j;
		m_availableKeys[Name("k")] = SDLK_k;
		m_availableKeys[Name("l")] = SDLK_l;
		m_availableKeys[Name("m")] = SDLK_m;
		m_availableKeys[Name("n")] = SDLK_n;
		m_availableKeys[Name("o")] = SDLK_o;
		m_availableKeys[Name("p")] = SDLK_p;
		m_availableKeys[Name("q")] = SDLK_q;
		m_availableKeys[Name("r")] = SDLK_r;
		m_availableKeys[Name("s")] = SDLK_s;
		m_availableKeys[Name("t")] = SDLK_t;
		m_availableKeys[Name("u")] = SDLK_u;
		m_availableKeys[Name("v")] = SDLK_v;
		m_availableKeys[Name("w")] = SDLK_w;
		m_availableKeys[Name("x")] = SDLK_x;
		m_availableKeys[Name("y")] = SDLK_y;
		m_availableKeys[Name("z")] = SDLK_z;
	}

	SDL_Keycode getKey(std::string name)
	{
		if (m_availableKeys.count(name) > 0)
			return m_availableKeys[name];
		return -1;
	}

	void update()
	{
		m_frame++;

		resetButtonHandling();


		while (SDL_PollEvent(&m_event))
		{
			switch (m_event.type)
			{
			case SDL_QUIT:
				m_closeWindow = true;
				break;
			case SDL_TEXTINPUT:
				if (ImGui::GetIO().WantTextInput)
					ImGui_ImplSdlGL3_ProcessEvent(&m_event);
				break;
			case SDL_KEYDOWN:
				if(ImGui::GetIO().WantCaptureKeyboard)
					ImGui_ImplSdlGL3_ProcessEvent(&m_event);
				else
					m_keyStates[m_event.key.keysym.sym] = m_frame;
				break;
			case SDL_KEYUP:
				ImGui_ImplSdlGL3_ProcessEvent(&m_event);
				m_keyStates[m_event.key.keysym.sym] = -m_frame;
				break;
			case SDL_MOUSEMOTION:
				m_mousePos.x = (float)m_event.motion.x;
				m_mousePos.y = (float)m_event.motion.y;
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (ImGui::GetIO().WantCaptureMouse)
					ImGui_ImplSdlGL3_ProcessEvent(&m_event);
				else
					m_buttonStates[m_event.button.button] = m_frame;
				break;
			case SDL_MOUSEBUTTONUP:
				if (ImGui::GetIO().WantCaptureMouse)
					ImGui_ImplSdlGL3_ProcessEvent(&m_event);
				else
					m_buttonStates[m_event.button.button] = -m_frame;
				break;
			default:
				ImGui_ImplSdlGL3_ProcessEvent(&m_event);
				break;
			}
		}
	}

	bool isWindowClosed()
	{
		return m_closeWindow;
	}

	bool isKeyPressed(SDL_Keycode key)
	{
		if (m_keyStates.count(key) > 0)
			return m_keyStates.at(key) == m_frame;
		return false;
	}

	SDL_Keycode getKeyFromName(Name name)
	{
		if (m_availableKeys.count(name) > 0)
			return m_availableKeys[name];
		else return -1;
	}

	bool isKeyPressed(Name keyName)
	{
		return isKeyPressed(getKeyFromName(keyName));
	}

	bool isKeyDown(SDL_Keycode key)
	{
		if (m_keyStates.count(key) > 0)
			return m_keyStates.at(key) > 0;
		return false;
	}

	bool isKeyDown(Name keyName)
	{
		return isKeyDown(getKeyFromName(keyName));
	}

	bool isKeyUp(SDL_Keycode key)
	{
		if (m_keyStates.count(key) > 0)
			return m_keyStates.at(key) < 0;
		return true;
	}

	bool isKeyUp(Name keyName)
	{
		return isKeyUp(getKeyFromName(keyName));
	}

	bool isKeyReleased(SDL_Keycode key)
	{
		if (m_keyStates.count(key) > 0)
			return m_keyStates.at(key) == -m_frame;
		return true;
	}

	bool isKeyReleased(Name keyName)
	{
		return isKeyReleased(getKeyFromName(keyName));
	}

	vec2 getMousePos()
	{
		return m_mousePos;
	}

	bool isButtonDown(int button)
	{
		return m_buttonStates[button] > 0;
	}

	bool isButtonPressed(int button)
	{
		return m_buttonStates[button] == m_frame;
	}

	bool isButtonReleased(int button)
	{
		return m_buttonStates[button] == -m_frame;
	}

	bool isButtonHandled(int button)
	{
		return (m_buttonHandled & (1 << button)) != 0;
	}
	
	void handleButton(int button)
	{
		m_buttonHandled |= (1 << button);
	}

	void resetButtonHandling()
	{
		m_buttonHandled = 0;
	}
}