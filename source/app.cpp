#include "app.h"

void App::HandleLeftClick(const SDL_Event &event)
{
	switch(mGameState)
	{
		case W_PROMOTE:
		case B_PROMOTE:
		{ 
			PieceTypeAndColor pc = mRenderer.GetPromotePieceAtPosition(mGameState == W_PROMOTE, event.button.x, event.button.y);
			if (pc != NONE)
			{
				mBoard.PromotePawn(pc);
				mGameState = mBoard.UpdateAfterMove();
			}
			break;
		}

		case W_WIN:
		case B_WIN:
		case STALEMATE:
			Restart();
			break;
	}
}

void App::Restart()
{
	mGameState = GAME;
	mIsWhiteTurn = true;
	mRenderer.Restart();
	mBoard.Restart();
}

void App::Update()
{
	mRenderer.UpdateFPS();
	if (mGameState == GAME)
	{
		MinimaxMove move = mPlayers[(int)mIsWhiteTurn]->GenerateMove(mBoard);
		if (move.To != -1)
		{
			mBoard.Move(move.From, move.To);
			mGameState = mBoard.UpdateAfterMove();
			if (move.PromotePc != NONE)
			{
				mBoard.PromotePawn(move.PromotePc);
				mGameState = mBoard.UpdateAfterMove();
			}
			mIsWhiteTurn = not mIsWhiteTurn;		
		}
	}
}


void App::Render()
{
	mRenderer.Render(mBoard, mGameState);
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
					if (mBoard.UndoMove());
					{
						mIsWhiteTurn = not mIsWhiteTurn;
					}
				}
			}
		}
		Update();
		Render();
	}
}
