#include "sceneManager.h"


void SceneManager::ChangeScene(const SceneType aType, std::shared_ptr<SceneChangeCommand> aCommand)
{
    std::shared_ptr<Scene> scene = Scene::CreateBySceneType(aType, mRenderer);
    if (aCommand)
        aCommand->execute(scene);
    scene->Initialize();
    mScenes.clear();
    mScenes.push_back(scene);
}

void SceneManager::PushScene(const SceneType aType, std::shared_ptr<SceneChangeCommand> aCommand)
{
    std::shared_ptr<Scene> scene = Scene::CreateBySceneType(aType, mRenderer);
    if (aCommand)
        aCommand->execute(scene);
    scene->Initialize();
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




