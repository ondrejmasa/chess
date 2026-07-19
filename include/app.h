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

class App {
private:
	Renderer mRenderer;
	Board mBoard;
	GameState mGameState = GAME;
	std::array<std::unique_ptr<Player>, 2> mPlayers;
	bool mIsWhiteTurn = true;
	void HandleLeftClick(const SDL_Event& event);
	void Restart();
	void Update();
	void Render();
public:
	App() : mPlayers{
		std::make_unique<User>(mRenderer, false),    
		std::make_unique<User>(mRenderer, true) 
	} {};
	virtual ~App();
	void Run();
};