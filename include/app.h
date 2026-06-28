#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include "fpsCounter.h"
#include "renderer.h"
#include <string>
#include <algorithm>
#include <array>
#include <cstdint>
#include "global.h"
#include "board.h"

struct BoardSettings
{
	float OffsetLeft = 15.f;
	float OffsetTop = 15.f;
	float OffsetRight = 15.f;
	float OffsetBottom = 15.f;
};

class App {
private:
	SDL_Window* pWindow;
	SDL_Renderer* pSDLRenderer;
	FpsCounter mFpsCounter;
	Renderer mRenderer;
	BoardSettings mBoardSettings;
	Board mBoard;
	int8_t mActiveIdx = -1;
	BitBoard mActiveMoves = 0ULL;
	bool mIsWhiteTurn = true;
	const int SCREEN_WIDTH = 800;
	const int SCREEN_HEIGHT = 800;
	void HandleLeftClick(const SDL_Event& event);
	void InitSDL();
	void Update();
	void UpdateFPS();
	void Render();
public:
	App();
	virtual ~App();
	void Run();
};