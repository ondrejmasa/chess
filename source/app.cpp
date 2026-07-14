#include "app.h"

void App::HandleLeftClick(const SDL_Event &event)
{
	switch(mGameState)
	{
		case GAME:
		{
			int8_t idx = mRenderer.GetIdxAtPosition(event.button.x, event.button.y);
			if (idx != mActiveIdx)
			{
				if ((mActiveMoves >> idx) & 1ULL)
				{
					mBoard.Move(mActiveIdx, idx);
					mGameState = mBoard.UpdateAfterMove();
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

		case W_PROMOTE:
		case B_PROMOTE:
		{ 
			PieceTypeAndColor pc = mRenderer.GetPromotePieceAtPosition(mGameState == W_PROMOTE, event.button.x, event.button.y);
			if (pc != NONE)
			{
				mBoard.PromotePawn(pc);
				mGameState = GAME;
			}
			break;
		}

		case W_WIN:
		case B_WIN:
			Restart();
			break;
	}
}

void App::Restart()
{
	mActiveIdx = -1;
	mActiveMoves = 0ULL;
	mGameState = GAME;
	mIsWhiteTurn = true;
	mBoard.Restart();
}

void App::Update()
{
	mRenderer.UpdateFPS();
	if (not mIsWhiteTurn)
	{
		MinimaxMove move = GetBestMove(mBoard, 4, false);
		mBoard.Move(move.From, move.To);
		mBoard.UpdateAfterMove();
		mIsWhiteTurn = not mIsWhiteTurn;
	}
}


void App::Render()
{
	mRenderer.Render(mBoard, mActiveIdx, mActiveMoves, mGameState);
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
				if (event.button.button == SDL_BUTTON_RIGHT)
				{
					mBoard.UndoMove();
					mIsWhiteTurn = not mIsWhiteTurn;
				}
			}
		}
		Update();
		Render();
	}
}
