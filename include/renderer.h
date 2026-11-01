#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_gfx/SDL3_gfxPrimitives.h>
#include <string>
#include <iostream>
#include <unordered_map>
#include <cmath>
#include <memory>
#include "global.h"
#include "board.h"
#include "fpsCounter.h"
#include "uiElements.h"

struct BoardSettings
{
	float OffsetLeft = 15.f;
	float OffsetTop = 15.f;
	float OffsetRight = 15.f;
	float OffsetBottom = 15.f;
};

class Animator {
private:
	const Uint64 ANIMATION_TIME = 180;
	Uint64 StartTime;
	float FromX;
	float FromY;
	float ToX;
	float ToY;
public:
	void StartAnimation(const float fromX, const float fromY,const float toX,const float toY);
	void GetActualPos(float& x, float& y);
	bool isFinished();
};


class Renderer {
private:
	int ScreenWidth = 800;
	int ScreenHeight = 800;
	TTF_Font* pFont = nullptr;
	SDL_Window* pWindow;
	SDL_Renderer* pSDLRenderer;
	TTF_TextEngine* pTextEngine;
	FpsCounter mFpsCounter;
	BoardSettings mBoardSettings;
	float BoardX = mBoardSettings.OffsetLeft;
	float BoardY = mBoardSettings.OffsetTop;
	float BoardSize = std::min(ScreenWidth - mBoardSettings.OffsetLeft - mBoardSettings.OffsetRight, ScreenHeight - mBoardSettings.OffsetTop - mBoardSettings.OffsetBottom);
	std::array<SDL_Texture*, 12> PieceTextures;
	std::array<SDL_Texture*, 2> WinTextures;
	std::unordered_map<std::string, SDL_Texture*> MenuTextures;
	SDL_Texture* StalemateTexture;
	SDL_FRect PromoteRect;
	MoveData LastMove;
	Animator mAnimator;
	SDL_Texture* LoadTexture(const std::string& path, SDL_Renderer* renderer);
	bool IsRotated = false;
	void ClearAllTextures();
	void RenderBoard(const Board& board);
	void RenderPromote(const bool isWhite, const uint8_t col);
	void RenderGameEnd(const GameState gameState);
	void InitTTF();
	void InitSDL();
public:
	uint8_t ActiveIdx = -1;
	BitBoard ActiveMoves = 0ULL;
	void RenderGame(const Board& board, const GameState gameState);
	void RenderClear();
	void RenderPresent();
	void RenderPause();
	void RenderMenu();
	void RenderUI(const std::vector<std::shared_ptr<UIElement>>& aUIElements);
	void loadAllTextures();
	void UpdateFPS();
	void Restart();
	void ChangeScreenSize(const int width, const int height);
	int8_t GetIdxAtPosition(const float posX, const float posY) const;
	PieceTypeAndColor GetPromotePieceAtPosition(const bool isWhite, const float posX, const float posY) const;
	int GetScreenWidth() const;
	int GetScreenHeight() const;
	TTF_Font* GetFont() const;
	const std::unordered_map<std::string, SDL_Texture*>& GetMenuTextures() const;
	SDL_Texture* GetPieceTexture(const PieceTypeAndColor pc);
	Renderer();
	~Renderer();
};