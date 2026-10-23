#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include "renderer.h"
#include <string>
#include <algorithm>
#include <array>
#include <cstdint>
#include "global.h"
#include "board.h"
#include "minimax.h"
#include "player.h"
#include <memory>
#include "sceneManager.h"

class App {
private:
	Renderer mRenderer;
	SceneManager mSceneManager;
	void Update();
	void Render();
public:
	App()
		: mSceneManager {mRenderer}
	{
		mSceneManager.ChangeScene(SceneType::MENU, nullptr);	
	};
	virtual ~App();
	void Run();
};