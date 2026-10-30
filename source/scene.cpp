#include "scene.h"

std::optional<SceneRequest> MenuScene::Update()
{
    int w = mRenderer.GetScreenWidth();
    int h = mRenderer.GetScreenHeight();
    float sz = std::min(w, h)*0.3;

    mWhiteFigTexture->Update(w/2 - w/9 - sz, h / 10, sz, sz);
    mBlackFigTexture->Update(w/2 + w/9, h / 10, sz, sz);

    sz *= 0.5;

    mSelRobotWhite->Update(
        mWhiteFigTexture->Left - sz/5, 
        mWhiteFigTexture->Top + mWhiteFigTexture->Height,
        sz, sz);
    mSelRobotWhite->IsSelected = false;  

    mSelPersonWhite->Update(
        mWhiteFigTexture->Left + mWhiteFigTexture->Width - (sz-sz/5), 
        mWhiteFigTexture->Top + mWhiteFigTexture->Height,
        sz, sz);
    mSelPersonWhite->IsSelected = true; 

    mSelRobotBlack->Update(
        mBlackFigTexture->Left - sz/5, 
        mBlackFigTexture->Top + mBlackFigTexture->Height,
        sz, sz);
    mSelRobotBlack->IsSelected = true;

    mSelPersonBlack->Update(
        mBlackFigTexture->Left + mBlackFigTexture->Width - (sz-sz/5), 
        mBlackFigTexture->Top + mBlackFigTexture->Height,
        sz, sz);
    mSelPersonBlack->IsSelected = false;   
    
    float btn_w = std::max(sz * 1.2f, 80.f);
    float btn_h = sz * 0.2;
    mStartButton->Update((w-btn_w) / 2.f, 3*h/4.f, btn_w, btn_h);


    return std::nullopt;
}

void MenuScene::Render()
{
    mRenderer.RenderMenu(mUIElements);
}

void MenuScene::Initialize()
{
    int w = mRenderer.GetScreenWidth();
    int h = mRenderer.GetScreenHeight();
    const auto& menuTextures = mRenderer.GetMenuTextures();
    mWhiteFigTexture = std::make_shared<TextureUIElement>(0.f, 0.f, 0.f, 0.f, mRenderer.GetPieceTexture(W_QUEEN));
    mBlackFigTexture = std::make_shared<TextureUIElement>(0.f, 0.f, 0.f, 0.f, mRenderer.GetPieceTexture(B_QUEEN));
    mSelRobotWhite = std::make_shared<SelectableUIElement>(0.f, 0.f, 0.f, 0.f, menuTextures.at("robot"));
    mSelRobotBlack = std::make_shared<SelectableUIElement>(0.f, 0.f, 0.f, 0.f, menuTextures.at("robot"));
    mSelPersonWhite = std::make_shared<SelectableUIElement>(0.f, 0.f, 0.f, 0.f, menuTextures.at("person"));
    mSelPersonBlack = std::make_shared<SelectableUIElement>(0.f, 0.f, 0.f, 0.f, menuTextures.at("person"));
    mStartButton = std::make_shared<ButtonUIElement>(0.f, 0.f, 0.f, 0.f, mRenderer.GetFont(), "START");
    mUIElements.push_back(mWhiteFigTexture);
    mUIElements.push_back(mBlackFigTexture);
    mUIElements.push_back(mSelRobotWhite);
    mUIElements.push_back(mSelRobotBlack);
    mUIElements.push_back(mSelPersonWhite);
    mUIElements.push_back(mSelPersonBlack);
    mUIElements.push_back(mStartButton);
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
            next = {SceneOperation::CHANGE, SceneType::GAME, cmd};
        }
        else if (aEvent.button.button == SDL_BUTTON_RIGHT)
        {
            auto black = std::make_unique<Computer>(mRenderer, false);
		    auto white = std::make_unique<User>(mRenderer, true);
            auto cmd = std::make_shared<SceneChangeNewGameCommand>(std::move(white), std::move(black));
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

void SceneChangeNewGameCommand::execute(std::shared_ptr<Scene> aScene)
{
    auto gameScene = std::dynamic_pointer_cast<GameScene>(aScene);
    if (gameScene)
    {
        gameScene ->StartNewGame(std::move(mWhite), std::move(mBlack));
    }
}