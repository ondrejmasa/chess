#include "app.h"

void App::HandleLeftClick(const SDL_Event &event)
{
	switch(mGameState)
	{
		case GAME:
			int8_t idx = mRenderer.GetIdxAtPosition(event.button.x, event.button.y);
			if (idx != mActiveIdx)
			{
				if ((mActiveMoves >> idx) & 1ULL)
				{
					mBoard.Move(mActiveIdx, idx);
					GameState gs = mBoard.UpdateAfterMove();
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
			break;
	}
}

void App::Update()
{
	mRenderer.UpdateFPS();
}


void App::Render()
{
	mRenderer.Render(mBoard, mActiveIdx, mActiveMoves);
}

App::App()
{
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
