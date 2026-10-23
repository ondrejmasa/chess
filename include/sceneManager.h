#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <SDL3/SDL.h>
#include "renderer.h"
#include "player.h"


class SceneManager;

enum class SceneType { GAME, MENU, PAUSE };

class Scene {
protected:
    SceneManager& mSceneManager;
    Renderer& mRenderer;
public:
    Scene(SceneManager& aSceneManager, Renderer& aRenderer)
        : mRenderer {aRenderer}, 
          mSceneManager {aSceneManager} {};
    static std::shared_ptr<Scene> CreateBySceneType(const SceneType aType, SceneManager& aSceneManager, Renderer& aRenderer);
    virtual void Update() = 0;
    virtual void Render() = 0;
    virtual void HandleEvent(const SDL_Event& aEvent) = 0;    
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
    void Update();
    void Render();
    void HandleEvent(const SDL_Event& aEvent);  
};

class MenuScene: public Scene {
public:
    using Scene::Scene;  
    void Update();
    void Render();
    void HandleEvent(const SDL_Event& aEvent);   
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
public:
    SceneManager(Renderer& aRenderer)
        : mRenderer {aRenderer} {};
    void ChangeScene(const SceneType aType, std::shared_ptr<SceneChangeCommand> aCommand);
    void Update();
    void Render();
    void HandleEvent(const SDL_Event& aEvent);
};