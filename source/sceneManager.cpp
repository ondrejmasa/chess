#include "sceneManager.h"


void SceneManager::ChangeScene(const SceneType aType, std::shared_ptr<SceneChangeCommand> aCommand)
{
    std::shared_ptr<Scene> scene = Scene::CreateBySceneType(aType, mRenderer);
    if (aCommand)
        aCommand->execute(scene);
    mScenes.clear();
    mScenes.push_back(scene);
}

void SceneManager::PushScene(const SceneType aType, std::shared_ptr<SceneChangeCommand> aCommand)
{
    std::shared_ptr<Scene> scene = Scene::CreateBySceneType(aType, mRenderer);
    if (aCommand)
        aCommand->execute(scene);
    mScenes.push_back(scene);
}

void SceneManager::Update()
{
    mRenderer.UpdateFPS();
    auto next = mScenes.back()->Update();
    if (next.has_value())
        mNextSceneRequest = next;
    if (mNextSceneRequest.has_value())
    {
        switch (mNextSceneRequest->mOperation)
        {
        case SceneOperation::CHANGE:
            ChangeScene(mNextSceneRequest->mNewScene, mNextSceneRequest->mSceneCommand);
            break;  
        case SceneOperation::POP:
            mScenes.pop_back();
            break;
        case SceneOperation::PUSH:
            PushScene(mNextSceneRequest->mNewScene, mNextSceneRequest->mSceneCommand);
        }
        mNextSceneRequest.reset();
    }
}

void SceneManager::Render()
{
    mRenderer.RenderClear();
    for (auto s : mScenes)
        s->Render();
    mRenderer.RenderPresent();
}

void SceneManager::HandleEvent(const SDL_Event& aEvent)
{
    auto next = mScenes.back()->HandleEvent(aEvent);
    if (next.has_value())
        mNextSceneRequest = next;
}

std::optional<SceneRequest> MenuScene::Update()
{
    auto next = std::nullopt;
    return next;
}

void MenuScene::Render()
{
    mRenderer.RenderMenu();
}

std::optional<SceneRequest> MenuScene::HandleEvent(const SDL_Event& aEvent)
{
    std::optional<SceneRequest> next = std::nullopt;
    if (aEvent.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        if (aEvent.button.button == SDL_BUTTON_LEFT)
        {
            auto black = std::make_unique<User>(mRenderer, false);
		    auto white = std::make_unique<User>(mRenderer, true);
            auto cmd = std::make_shared<SceneChangeNewGameCommand>(std::move(white), std::move(black));
            // mSceneManager.ChangeScene(SceneType::GAME, cmd);
            next = {SceneOperation::CHANGE, SceneType::GAME, cmd};
        }
        else if (aEvent.button.button == SDL_BUTTON_RIGHT)
        {
            auto black = std::make_unique<Computer>(mRenderer, false);
		    auto white = std::make_unique<User>(mRenderer, true);
            auto cmd = std::make_shared<SceneChangeNewGameCommand>(std::move(white), std::move(black));
            // mSceneManager.ChangeScene(SceneType::GAME, cmd);
            next = {SceneOperation::CHANGE, SceneType::GAME, cmd};
        }
    }
    return next;
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

std::optional<SceneRequest> GameScene::Update()
{
    std::optional<SceneRequest> next = std::nullopt;
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
    return next;
}

void GameScene::Render()
{
    mRenderer.RenderGame(mBoard, mGameState);
}

std::optional<SceneRequest> GameScene::HandleEvent(const SDL_Event& aEvent)
{
    std::optional<SceneRequest> next = std::nullopt;
    if (aEvent.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        if (aEvent.button.button == SDL_BUTTON_LEFT)
        {
            HandleLeftClick(aEvent);
        }
        if (aEvent.button.button == SDL_BUTTON_RIGHT)
        {
            if (mBoard.UndoMove())
            {
                mIsWhiteTurn = not mIsWhiteTurn;
            }
        }
    }
    else if (aEvent.type == SDL_EVENT_KEY_DOWN)
    {
        if (aEvent.key.key == SDLK_ESCAPE)
        {
            next = SceneRequest{ SceneOperation::PUSH, SceneType::PAUSE, nullptr };
        }
    }
    return next;
}

void SceneChangeNewGameCommand::execute(std::shared_ptr<Scene> aScene)
{
    auto gameScene = std::dynamic_pointer_cast<GameScene>(aScene);
    if (gameScene)
    {
        gameScene ->StartNewGame(std::move(mWhite), std::move(mBlack));
    }
}

std::shared_ptr<Scene> Scene::CreateBySceneType(const SceneType aType, Renderer& aRenderer)
{
    switch (aType)
    {
    case SceneType::GAME:
        return std::make_unique<GameScene>(aRenderer);
    case SceneType::MENU:
        return std::make_unique<MenuScene>(aRenderer);    
    case SceneType::PAUSE:
        return std::make_unique<PauseScene>(aRenderer); 
    }
    return nullptr;
}

std::optional<SceneRequest> PauseScene::HandleEvent(const SDL_Event &aEvent)
{
    std::optional<SceneRequest> next = std::nullopt;
    if (aEvent.type == SDL_EVENT_KEY_DOWN)
    {
        if (aEvent.key.key == SDLK_ESCAPE)
        {
            return SceneRequest{ SceneOperation::POP, SceneType::NONE, nullptr };
        }
    }
    return next;
}

void PauseScene::Render()
{
    mRenderer.RenderPause();
}
