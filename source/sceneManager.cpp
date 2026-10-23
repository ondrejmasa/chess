#include "sceneManager.h"


void SceneManager::ChangeScene(const SceneType aType, std::shared_ptr<SceneChangeCommand> aCommand)
{
    std::shared_ptr<Scene> scene = Scene::CreateBySceneType(aType, *this, mRenderer);
    if (aCommand)
        aCommand->execute(scene);
    mScenes.clear();
    mScenes.push_back(scene);
}

void SceneManager::Update()
{
    mScenes.back()->Update();
}

void SceneManager::Render()
{
    mScenes.back()->Render();
}

void SceneManager::HandleEvent(const SDL_Event& aEvent)
{
    mScenes.back()->HandleEvent(aEvent);
}

void MenuScene::Update()
{
}

void MenuScene::Render()
{
    mRenderer.RenderMenu();
}

void MenuScene::HandleEvent(const SDL_Event& aEvent)
{
    if (aEvent.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        if (aEvent.button.button == SDL_BUTTON_LEFT)
        {
            auto black = std::make_unique<User>(mRenderer, false);
		    auto white = std::make_unique<User>(mRenderer, true);
            auto cmd = std::make_shared<SceneChangeNewGameCommand>(std::move(white), std::move(black));
            mSceneManager.ChangeScene(SceneType::GAME, cmd);
        }
        else if (aEvent.button.button == SDL_BUTTON_RIGHT)
        {
            auto black = std::make_unique<Computer>(mRenderer, false);
		    auto white = std::make_unique<User>(mRenderer, true);
            auto cmd = std::make_shared<SceneChangeNewGameCommand>(std::move(white), std::move(black));
            mSceneManager.ChangeScene(SceneType::GAME, cmd);
        }
    }
}

void GameScene::HandleLeftClick(const SDL_Event &event)
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

void GameScene::Restart()
{
    mGameState = GameState::GAME;
	mIsWhiteTurn = true;
	mRenderer.Restart();
	mBoard.Restart();
}

void GameScene::StartNewGame(std::unique_ptr<Player> aWhite, std::unique_ptr<Player> aBlack)
{
    mPlayers[0] = std::move(aBlack);
    mPlayers[1] = std::move(aWhite);
    Restart();
}

void GameScene::Update()
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

void GameScene::Render()
{
    mRenderer.Render(mBoard, mGameState);
}

void GameScene::HandleEvent(const SDL_Event& aEvent)
{
    if (aEvent.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        if (aEvent.button.button == SDL_BUTTON_LEFT)
        {
            HandleLeftClick(aEvent);
        }
        if (aEvent.button.button == SDL_BUTTON_RIGHT)
        {
            if (mBoard.UndoMove());
            {
                mIsWhiteTurn = not mIsWhiteTurn;
            }
        }
    }
}

void SceneChangeNewGameCommand::execute(std::shared_ptr<Scene> aScene)
{
    auto gameScene = std::dynamic_pointer_cast<GameScene>(aScene);
    if (gameScene)
    {
        gameScene ->StartNewGame(std::move(mWhite), std::move(mBlack));
    }
}

std::shared_ptr<Scene> Scene::CreateBySceneType(const SceneType aType, SceneManager& aSceneManager, Renderer& aRenderer)
{
    switch (aType)
    {
    case SceneType::GAME:
        return std::make_unique<GameScene>(aSceneManager, aRenderer);
    case SceneType::MENU:
        return std::make_unique<MenuScene>(aSceneManager, aRenderer);    
    }
}
