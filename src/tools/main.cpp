#include <iostream>
#include <SDL.h>
#include <GL/glew.h>
#include <string>
#include <fstream>
#include <vector>
#include <SDL_ttf.h>

#include "input.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl_gl3.h"

#include "editor/editor_main.h"


#define MAX_DELTA_TIME 0.1f

namespace render
{
	void GLAPIENTRY OpenGLCallback(GLenum source,
			GLenum type,
			GLuint id,
			GLenum severity,
			GLsizei length,
			const GLchar* message,
			const void* userParam)
	{
		fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
			type, severity, message);
	}
	void initGL(SDL_Window* window)
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_CreateContext(window);

		glEnable(GL_TEXTURE_2D);

		glewExperimental = GL_TRUE;
		const GLenum err = glewInit();

		if (GLEW_OK != err)
		{
			std::cout << "GLEW Error: " << glewGetErrorString(err) << std::endl;
		}

		if (TTF_Init() == -1)
		{
			std::cerr << "Error during SDL_TTF initialization : " << TTF_GetError() << std::endl;
		}


		// During init, enable debug output
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(OpenGLCallback, 0);
	}

	void initImGUI()
	{

	}
}


int main(int argc, char *argv[])
{
	SDL_Window* window;
	bool isRunning = true;

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		printf("Unable to init SDL : %s", SDL_GetError());
		return 1;
	}

	window = SDL_CreateWindow("MyWindow", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1500, 800, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (window == NULL)
	{
		printf("Unable to create window : %s", SDL_GetError());
		return 1;
	}

	render::initGL(window);

	ImGui_ImplSdlGL3_Init(window);

	// Setup style
	ImGui::StyleColorsClassic();

	irect screenSize = { 0, 0, 0, 0 };

	Uint32 frameStartTime = SDL_GetTicks();

	while (!input::isWindowClosed())
	{
		input::update();
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		glClearDepth(0);
		Uint32 newFrameStartTime = SDL_GetTicks();
		float deltaTime = (newFrameStartTime - frameStartTime) / 1000.0f;
		frameStartTime = newFrameStartTime;
		if (deltaTime > MAX_DELTA_TIME)
			deltaTime = MAX_DELTA_TIME;

		renderFrame(window);

		editor::drawEditor(deltaTime);

		int display_w = 800, display_h = 600;
		glViewport(screenSize.x, screenSize.y, screenSize.w, screenSize.h);
		ImGui::Render();
		SDL_GL_SwapWindow(window);
	}

	ImGui_ImplSdlGL3_Shutdown();
	SDL_GL_DeleteContext(window);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}