#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <optional>
#include <SDL3/SDL.h>
#include "renderer.h"
#include "player.h"


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
};

class GameScene: public Scene {
private:
	Board mBoard;
	GameState mGameState = GameState::GAME;
    std::array<std::unique_ptr<Player>, 2> mPlayers;
	bool mIsWhiteTurn = true;
	void HandleLeftClick(const SDL_Event& event);
    void Restart();
public:
    using Scene::Scene;
    void StartNewGame(std::unique_ptr<Player> aWhite, std::unique_ptr<Player> aBlack);
    std::optional<SceneRequest> Update();
    std::optional<SceneRequest> HandleEvent(const SDL_Event& aEvent);  
    void Render();
};

class MenuScene: public Scene {
public:
    using Scene::Scene;  
    std::optional<SceneRequest> Update();
    std::optional<SceneRequest> HandleEvent(const SDL_Event& aEvent);   
    void Render();
};

class PauseScene: public Scene {
public:
    using Scene::Scene;  
    std::optional<SceneRequest> HandleEvent(const SDL_Event& aEvent);   
    void Render();    
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

class SceneManager {
private:
    Renderer& mRenderer;
    std::vector<std::shared_ptr<Scene>> mScenes;
    std::optional<SceneRequest> mNextSceneRequest;
public:
    SceneManager(Renderer& aRenderer)
        : mRenderer {aRenderer} {};
    void ChangeScene(const SceneType aType, std::shared_ptr<SceneChangeCommand> aCommand);
    void PushScene(const SceneType aType, std::shared_ptr<SceneChangeCommand> aCommand);
    void Update();
    void Render();
    void HandleEvent(const SDL_Event& aEvent);
};