#include <SDL3/SDL.h>
#include <optional>
#include <memory>
#include "player.h"
#include "uiElements.h"
#include "global.h"


class SceneChangeCommand;

enum class SceneType { NONE = 0, GAME, MENU, PAUSE };

enum class SceneOperation { CHANGE, PUSH, POP };

struct SceneRequest {
    SceneOperation mOperation;
    SceneType mNewScene;
    std::shared_ptr<SceneChangeCommand> mSceneCommand;   
};

class Scene {
protected:
    Renderer& mRenderer;
public:
    Scene(Renderer& aRenderer)
        : mRenderer {aRenderer} {};
    static std::shared_ptr<Scene> CreateBySceneType(const SceneType aType, Renderer& aRenderer);
    virtual std::optional<SceneRequest> Update() { return std::nullopt; };
    virtual std::optional<SceneRequest> HandleEvent(const SDL_Event& aEvent) = 0;    
    virtual void Render() = 0;
    virtual void Initialize() {};
};

class UIScene: public Scene {
protected:
    using Scene::Scene;
    std::vector<std::shared_ptr<UIElement>> mUIElements;
    std::optional<SceneRequest> HandleEvent(const SDL_Event& aEvent);
    void Render();
};

class GameScene: public Scene {
private:
	Board mBoard;
	GameState mGameState = GameState::GAME;
    std::array<std::unique_ptr<Player>, 2> mPlayers;
	bool mIsWhiteTurn = true;
	void HandleLeftClick(const SDL_Event& event);
public:
    using Scene::Scene;
    void StartNewGame(std::unique_ptr<Player> aWhite, std::unique_ptr<Player> aBlack);
    std::optional<SceneRequest> Update();
    std::optional<SceneRequest> HandleEvent(const SDL_Event& aEvent);  
    void Render();
    void Restart();
};

class MenuScene: public UIScene {
private:
    std::shared_ptr<SelectableUIElement> mSelRobotWhite;
    std::shared_ptr<SelectableUIElement> mSelRobotBlack;
    std::shared_ptr<SelectableUIElement> mSelPersonWhite;
    std::shared_ptr<SelectableUIElement> mSelPersonBlack;
    std::shared_ptr<TextureUIElement> mWhiteFigTexture;
    std::shared_ptr<TextureUIElement> mBlackFigTexture;
    std::shared_ptr<ButtonUIElement> mStartButton;
    bool mIsWhiteRobot = false;
    bool mIsBlackRobot = true;
    bool mIsNextClicked = false;
public:
    using UIScene::UIScene;
    std::optional<SceneRequest> Update();
    void Render();
    void Initialize();
};

class PauseScene: public UIScene {
private:
    std::shared_ptr<ButtonUIElement> mRestartButton;
    std::shared_ptr<ButtonUIElement> mMenuButton;    
    bool mIsRestartClicked;
    bool mIsMenuClicked;
public:
    using UIScene::UIScene;  
    std::optional<SceneRequest> HandleEvent(const SDL_Event& aEvent);  
    std::optional<SceneRequest> Update(); 
    void Render();    
    void Initialize();
};

class SceneChangeCommand {
public:
    virtual void execute(std::shared_ptr<Scene> aScene) = 0;
};

class SceneChangeNewGameCommand: public SceneChangeCommand {
private:
    std::unique_ptr<Player> mWhite;
    std::unique_ptr<Player> mBlack;
public:
    SceneChangeNewGameCommand(std::unique_ptr<Player> aWhite, std::unique_ptr<Player> aBlack)
    {
        mWhite = std::move(aWhite);
        mBlack = std::move(aBlack); 
    };
    void execute(std::shared_ptr<Scene> aScene);    
};

class SceneChangeRestartGameCommand: public SceneChangeCommand {
public:
    void execute(std::shared_ptr<Scene> aScene);  
};
