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

class App {
private:
	Renderer mRenderer;
	Board mBoard;
	int8_t mActiveIdx = -1;
	BitBoard mActiveMoves = 0ULL;
	GameState mGameState = GAME;
	bool mIsWhiteTurn = true;
	void HandleLeftClick(const SDL_Event& event);
	void Update();
	void Render();
public:
	App();
	virtual ~App();
	void Run();
};