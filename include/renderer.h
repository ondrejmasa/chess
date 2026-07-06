#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <iostream>
#include <map>
#include "global.h"
#include "board.h"
#include "fpsCounter.h"

struct BoardSettings
{
	float OffsetLeft = 15.f;
	float OffsetTop = 15.f;
	float OffsetRight = 15.f;
	float OffsetBottom = 15.f;
};

class Renderer {
private:
	const int SCREEN_WIDTH = 800;
	const int SCREEN_HEIGHT = 800;
	SDL_Window* pWindow;
	SDL_Renderer* pSDLRenderer;
	TTF_TextEngine* pTextEngine;
	FpsCounter mFpsCounter;
	BoardSettings mBoardSettings;
	float BoardX = mBoardSettings.OffsetLeft;
	float BoardY = mBoardSettings.OffsetTop;
	float BoardSize = std::min(SCREEN_WIDTH - mBoardSettings.OffsetLeft - mBoardSettings.OffsetRight, SCREEN_HEIGHT - mBoardSettings.OffsetTop - mBoardSettings.OffsetBottom);
	std::array<SDL_Texture*, 12> pieceTextures;
	SDL_FRect PromoteRect;
	SDL_Texture* LoadTexture(const std::string& path, SDL_Renderer* renderer);
	void ClearPiecesTextures();
	void RenderWinState(SDL_Renderer* renderer, const bool IsWinnerWhite);
	void RenderBoard(const Board& board, const uint8_t activeIdx, const BitBoard activeMoves);
	void RenderPromote(const bool isWhite, const uint8_t col);
	void InitTTF();
	void InitSDL();
public:
	void Render(const Board& board, const uint8_t activeIdx, const BitBoard activeMoves, const GameState gameState);
	void loadAllPieces();
	void UpdateFPS();
	int8_t GetIdxAtPosition(const float posX, const float posY) const;
	PieceTypeAndColor GetPromotePieceAtPosition(const bool isWhite, const float posX, const float posY) const;
	Renderer();
	~Renderer();
};