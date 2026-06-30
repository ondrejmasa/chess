#include "app.h"

void App::HandleLeftClick(const SDL_Event &event)
{
	int8_t idx = mRenderer.GetIdxAtPosition(event.button.x, event.button.y);
	if (idx != mActiveIdx)
	{
		if ((mActiveMoves >> idx) & 1ULL)
		{
			mBoard.Move(mActiveIdx, idx);
			mActiveIdx = -1;
			mActiveMoves = 0ULL;
			mIsWhiteTurn = not mIsWhiteTurn;
		}
		else
		{
			SquareColor clr = mBoard.GetColorAtIdx(idx);
			if (clr != EMPTY and mIsWhiteTurn == (clr == BLACK))
			{
				std::cout << "wrong color\n";
				mActiveIdx = -1;
				mActiveMoves = 0ULL;
				return;
			}
			mActiveIdx = idx;
			mActiveMoves = mBoard.GetActiveMoves(idx);
		}
	}
	
}

void App::InitSDL()
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer("", SCREEN_WIDTH, SCREEN_HEIGHT, 0, &pWindow, &pSDLRenderer);
	SDL_SetRenderVSync(pSDLRenderer, 1);
}

void App::Update()
{
	UpdateFPS();
}

void App::UpdateFPS()
{
	if (mFpsCounter.Update())
	{
		float fps = mFpsCounter.GetFps();
		SDL_SetWindowTitle(pWindow, ("FPS: " + std::to_string(fps)).c_str());
	}
}

void App::Render()
{
	SDL_SetRenderDrawBlendMode(pSDLRenderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(pSDLRenderer, 160, 160, 180, 255);
	SDL_RenderClear(pSDLRenderer);

	float l = mBoardSettings.OffsetLeft; float t = mBoardSettings.OffsetTop;
	float r = mBoardSettings.OffsetRight; float b = mBoardSettings.OffsetBottom;
	float sz = std::min(SCREEN_WIDTH - l - r, SCREEN_HEIGHT - t - b);
	mRenderer.Render(pSDLRenderer, mBoard, mActiveIdx, mActiveMoves, l, t, sz);

	SDL_RenderPresent(pSDLRenderer);
}

App::App()
{
	InitSDL();
	mRenderer.loadAllPieces(pSDLRenderer);
}

App::~App()
{
	SDL_DestroyRenderer(pSDLRenderer);
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
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

			else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
			{
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					HandleLeftClick(event);
				}
			}
		}
		Update();
		Render();
	}
}
