#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <optional>
#include <SDL3/SDL.h>
#include "renderer.h"
#include "scene.h"



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
    void PopScene(std::shared_ptr<SceneChangeCommand> aCommand);
    void Update();
    void Render();
    void HandleEvent(const SDL_Event& aEvent);
};