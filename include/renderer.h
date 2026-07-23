#pragma once
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


class Animator {
private:
	const Uint64 ANIMATION_TIME = 100;
	Uint64 StartTime;
	float FromX;
	float FromY;
	float ToX;
	float ToY;
public:
	void StartAnimation(const float fromX, const float fromY,const float toX,const float toY)
	{
		StartTime = SDL_GetTicks();
		FromX = fromX;
		FromY = fromY;
		ToX = toX;
		ToY = toY;
	};

	void GetActualPos(float& x, float& y)
	{
		Uint64 actTime = SDL_GetTicks();
		Uint64 elapsedTime = actTime - StartTime;
		if (elapsedTime > ANIMATION_TIME)
		{
			x = ToX;
			y = ToY;
			return;
		}	

		float t = static_cast<float>(elapsedTime) / static_cast<float>(ANIMATION_TIME);
		x = FromX + t * (ToX - FromX);
		y = FromY + t * (ToY - FromY);
	};

	bool isFinished() 
	{
		return SDL_GetTicks() - StartTime > ANIMATION_TIME;
	}
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
	std::array<SDL_Texture*, 12> PieceTextures;
	std::array<SDL_Texture*, 2> WinTextures;
	SDL_Texture* StalemateTexture;
	SDL_FRect PromoteRect;
	MoveData LastMove;
	Animator mAnimator;
	SDL_Texture* LoadTexture(const std::string& path, SDL_Renderer* renderer);
	void ClearAllTextures();
	void RenderBoard(const Board& board);
	void RenderPromote(const bool isWhite, const uint8_t col);
	void RenderGameEnd(const GameState gameState);
	void InitTTF();
	void InitSDL();
public:
	uint8_t ActiveIdx = -1;
	BitBoard ActiveMoves = 0ULL;
	void Render(const Board& board, const GameState gameState);
	void loadAllTextures();
	void UpdateFPS();
	void Restart();
	int8_t GetIdxAtPosition(const float posX, const float posY) const;
	PieceTypeAndColor GetPromotePieceAtPosition(const bool isWhite, const float posX, const float posY) const;
	Renderer();
	~Renderer();
};