#include "app.h"


void App::Update()
{
	mSceneManager.Update();
}


void App::Render()
{
	mSceneManager.Render();
}

App::~App()
{
}

void App::Run()
{
	SDL_Event event;
	bool quit = false;
	while (!quit) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				quit = true;
			}
			else if (event.type == SDL_EVENT_WINDOW_RESIZED)
			{
				mRenderer.ChangeScreenSize(event.window.data1, event.window.data2);
			}

			mSceneManager.HandleEvent(event);

		}
		Update();
		Render();
	}
}
