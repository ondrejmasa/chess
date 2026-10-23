#include "renderer.h"


SDL_Texture* Renderer::LoadTexture(const std::string& path, SDL_Renderer* renderer)
{
	SDL_Texture* newTexture = IMG_LoadTexture(renderer, path.c_str());
	if (newTexture == nullptr) {
		std::cout << "Error while loading image " << path << "! Error: " << SDL_GetError() << "\n";
		return nullptr;
	}
	return newTexture;
}

void Renderer::ClearAllTextures()
{
	for (auto const& texture : PieceTextures)
	{
		if (texture != nullptr)
			SDL_DestroyTexture(texture);
	}
	PieceTextures.fill(nullptr);

	for (auto const& texture : WinTextures)
	{
		if (texture != nullptr)
			SDL_DestroyTexture(texture);
	}
	WinTextures.fill(nullptr);	

	if (StalemateTexture != nullptr)
		SDL_DestroyTexture(StalemateTexture);	
}

void Renderer::InitTTF()
{
	if (!TTF_Init()) {
        std::cerr << "TTF_Init ERROR: " << SDL_GetError() << std::endl;
        return;
    }

    pTextEngine = TTF_CreateRendererTextEngine(pSDLRenderer);
    if (!pTextEngine) {
        std::cerr << "TextEngine ERROR: " << SDL_GetError() << std::endl;
    }
}

void Renderer::InitSDL()
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer("", ScreenWidth, ScreenHeight, SDL_WINDOW_RESIZABLE, &pWindow, &pSDLRenderer);
	SDL_SetRenderVSync(pSDLRenderer, 1);
}

void Renderer::Render(const Board &board, const GameState gameState)
{
	SDL_SetRenderDrawBlendMode(pSDLRenderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(pSDLRenderer, 160, 160, 180, 255);
	SDL_RenderClear(pSDLRenderer);

	RenderBoard(board);	
	switch (gameState)
	{
		case W_PROMOTE:
		case B_PROMOTE:
		{ 
			const MoveData lm = board.GetLastMove();
			const uint8_t col = lm.To % 8;
			RenderPromote(gameState == W_PROMOTE, col);
			break;
		}
		case W_WIN:
		case B_WIN:
		case STALEMATE:
			RenderGameEnd(gameState);	
			break;
	}
	SDL_RenderPresent(pSDLRenderer);
}

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>

void Renderer::RenderMenu()
{

	// TEMPORARY

	SDL_SetRenderDrawBlendMode(pSDLRenderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(pSDLRenderer, 160, 160, 180, 255);
	SDL_RenderClear(pSDLRenderer);

    if (!pFont) {
        // std::cerr << "Font není načten!" << std::endl;
        return;
    }

    // Barva v SDL3 (RGBA float hodnoty 0.0f až 1.0f)
    SDL_Color textColor = { 255, 255, 255, 255 }; 

    const char* menuText = "Left Click: Player vs Player | Right Click: Player vs Bot";

    // 1. Vytvoření Surface (délka 0 = null-terminated string)
    SDL_Surface* textSurface = TTF_RenderText_Blended(pFont, menuText, 0, textColor);
    if (!textSurface) {
        std::cerr << "Chyba při vytváření surface: " << SDL_GetError() << std::endl;
        return;
    }

    // 2. Převod Surface na Texturu pro mRenderer (v SDL3 předáváte mRenderer a surface)
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(pSDLRenderer, textSurface);
    
    // Surface po převedení na texturu ihned uvolníme
    SDL_DestroySurface(textSurface); // V SDL3 je to SDL_DestroySurface (místo SDL_FreeSurface)

    if (!textTexture) {
        std::cerr << "Chyba při vytváření textury: " << SDL_GetError() << std::endl;
        return;
    }

    // 3. Zjištění rozměrů textury v SDL3
    float textWidth = 0.0f;
    float textHeight = 0.0f;
    SDL_GetTextureSize(textTexture, &textWidth, &textHeight);

    // 4. Nastavení pozice pomocí floatů (SDL_FRect)
    SDL_FRect renderQuad;
    renderQuad.x = (static_cast<float>(ScreenWidth) - textWidth) / 2.0f;
    renderQuad.y = (static_cast<float>(ScreenHeight) - textHeight) / 2.0f;
    renderQuad.w = textWidth;
    renderQuad.h = textHeight;

    // 5. Vykreslení do SDL3 Rendereru
    SDL_RenderTexture(pSDLRenderer, textTexture, nullptr, &renderQuad);

    // 6. Úklid textury
    SDL_DestroyTexture(textTexture);

	SDL_RenderPresent(pSDLRenderer);
}

void Renderer::RenderBoard(const Board &board)
{
	const float cellSize = BoardSize / 8.f;
	const auto ro = IsRotated ? 7 : 0;

	const MoveData& lm = board.GetLastMove();
	if (lm != LastMove)
	{
		uint8_t fr = std::abs(ro - lm.From / 8);	
		uint8_t fc = std::abs(ro - lm.From % 8);	
		uint8_t tr = std::abs(ro - lm.To / 8);	
		uint8_t tc = std::abs(ro - lm.To % 8);

		float fx = BoardX + fc * cellSize;
		float fy = BoardY + (7 - fr) * cellSize;
		float tx = BoardX + tc * cellSize;
		float ty = BoardY + (7 - tr) * cellSize;

		mAnimator.StartAnimation(fx, fy, tx, ty);
		LastMove = lm;
	}


	// background
	SDL_FRect rect = { BoardX, BoardY, BoardSize, BoardSize };
	SDL_SetRenderDrawColor(pSDLRenderer, 242, 232, 231, 255);
	SDL_RenderFillRect(pSDLRenderer, &rect);

	// dark squares
	SDL_SetRenderDrawColor(pSDLRenderer, 163, 82, 78, 255);
	for (auto i = 0; i < 8; ++i) {
		auto i_rot = std::abs(ro-i);
		for (auto j = 1 - (i % 2); j < 8; j += 2) {
			auto j_rot = std::abs(ro-j);
			rect = { BoardX + j_rot * cellSize, BoardY + i_rot * cellSize, cellSize, cellSize };
			SDL_RenderFillRect(pSDLRenderer, &rect);
		}
	}

	for (auto i = 7; i >= 0; --i) {
		auto i_rot = std::abs(ro-i);
		for (auto j = 0; j < 8; ++j) {
			auto idx = i * 8 + j;
			auto j_rot = std::abs(ro-j);
			// last move
			if (LastMove.From == idx)
			{
				float sz = cellSize * 0.90f;
				float o = (cellSize - sz) / 2;
				rect = { BoardX + j_rot * cellSize + o, BoardY + (7 - i_rot) * cellSize + o, sz, sz };
				SDL_SetRenderDrawColor(pSDLRenderer, 255, 237, 189, 255);
				SDL_RenderFillRect(pSDLRenderer, &rect);
			}
			else if (LastMove.To == idx)
			{
				float sz = cellSize * 0.90f;
				float o = (cellSize - sz) / 2;
				rect = { BoardX + j_rot * cellSize + o, BoardY + (7 - i_rot) * cellSize + o, sz, sz };
				SDL_SetRenderDrawColor(pSDLRenderer, 255, 203, 166, 255);
				SDL_RenderFillRect(pSDLRenderer, &rect);
			}

			// active moves
			if (ActiveMoves >> idx & 1ULL)
			{
				float sz = cellSize * 0.75f;
				float o = (cellSize - sz) / 2;
				rect = { BoardX + j_rot * cellSize + o, BoardY + (7 - i_rot) * cellSize + o, sz, sz };
				SDL_SetRenderDrawColor(pSDLRenderer, 150, 150, 150, 100);
				SDL_RenderFillRect(pSDLRenderer, &rect);
			}
			// pieces
			for (int pc = W_PAWN; pc <= B_KING; ++pc)
			{
				if ((board.PieceBB[pc] >> idx) & 1ULL)
				{
					rect = { BoardX + j_rot * cellSize, BoardY + (7-i_rot) * cellSize, cellSize, cellSize };
					if (ActiveIdx == idx)
					{
						SDL_SetRenderDrawColor(pSDLRenderer, 163, 120, 71, 255);
						SDL_RenderFillRect(pSDLRenderer, &rect);
					}
					if (idx != LastMove.To or mAnimator.isFinished())
						SDL_RenderTexture(pSDLRenderer, PieceTextures[pc], nullptr, &rect);
					break;
				}
			}
		}
	}

	// animation of last move
	if (not mAnimator.isFinished())
	{
		float x, y;
		mAnimator.GetActualPos(x, y);
		rect = {x, y, cellSize, cellSize};
		SDL_RenderTexture(pSDLRenderer, PieceTextures[LastMove.Pc], nullptr, &rect);
	}
}

void Renderer::RenderPromote(const bool isWhite, const uint8_t col)
{
	const auto ro = IsRotated ? 7 : 0;
	SDL_FRect& r = PromoteRect;
	r.w = BoardSize / 8.f;
	r.h = r.w*4.f;
	r.x = BoardX+r.w*std::abs(ro-col);
	r.y = (isWhite != IsRotated) ? BoardY : BoardY + BoardSize - r.h; 
	SDL_SetRenderDrawColor(pSDLRenderer, 20, 20, 20, 255);
	SDL_RenderFillRect(pSDLRenderer, &r);
	SDL_FRect r2 = r;
	const float bw = 4.f;
	r2.w -= bw;
	r2.h -= bw;
	r2.x += bw/2;
	r2.y += bw/2;	
	SDL_SetRenderDrawColor(pSDLRenderer, 255, 255, 255, 255);
	SDL_RenderFillRect(pSDLRenderer, &r2);
	r2 = r;
	r2.h = r2.w;
	if (isWhite)
	{
		for (int pc = W_KNIGHT; pc <= W_QUEEN; ++pc)
		{
			SDL_RenderTexture(pSDLRenderer, PieceTextures[pc], nullptr, &r2);
			r2.y += r2.w;
		}
	}
	else
	{
		for (int pc = B_QUEEN; pc >= B_KNIGHT; --pc)
		{
			SDL_RenderTexture(pSDLRenderer, PieceTextures[pc], nullptr, &r2);
			r2.y += r2.w;
		}		
	}
}

void Renderer::RenderGameEnd(const GameState gameState)
{
	SDL_Texture* t;
	if (gameState == STALEMATE)
	{
		SDL_SetRenderDrawColor(pSDLRenderer, 0, 0, 0, 80);
		t = StalemateTexture;
	}
	else if (gameState == W_WIN or gameState == B_WIN)
	{
		const bool isWhite = gameState == W_WIN;
		isWhite ? SDL_SetRenderDrawColor(pSDLRenderer, 0, 0, 0, 80) : SDL_SetRenderDrawColor(pSDLRenderer, 255, 255, 255, 80);
		t = WinTextures[static_cast<int>(isWhite)];
	}
	else return;
	SDL_FRect r {0, 0, ScreenWidth, ScreenHeight};
	SDL_RenderFillRect(pSDLRenderer, &r);
	r.w = BoardSize * 0.7f;
	r.h = r.w * (static_cast<float>(t->h) / t->w);
	r.x = BoardX + (BoardSize-r.w) / 2.f;
	r.y = BoardY + (BoardSize-r.h) / 2.f;
	SDL_RenderTexture(pSDLRenderer, t, nullptr, &r);
}

void Renderer::loadAllTextures()
{
	PieceTextures[W_PAWN] = LoadTexture("resources/w-pawn.png", pSDLRenderer);
	PieceTextures[W_KNIGHT] = LoadTexture("resources/w-knight.png", pSDLRenderer);
	PieceTextures[W_BISHOP] = LoadTexture("resources/w-bishop.png", pSDLRenderer);
	PieceTextures[W_ROOK] = LoadTexture("resources/w-rook.png", pSDLRenderer);
	PieceTextures[W_QUEEN] = LoadTexture("resources/w-queen.png", pSDLRenderer);
	PieceTextures[W_KING] = LoadTexture("resources/w-king.png", pSDLRenderer);

	PieceTextures[B_PAWN] = LoadTexture("resources/b-pawn.png", pSDLRenderer);
	PieceTextures[B_KNIGHT] = LoadTexture("resources/b-knight.png", pSDLRenderer);
	PieceTextures[B_BISHOP] = LoadTexture("resources/b-bishop.png", pSDLRenderer);
	PieceTextures[B_ROOK] = LoadTexture("resources/b-rook.png", pSDLRenderer);
	PieceTextures[B_QUEEN] = LoadTexture("resources/b-queen.png", pSDLRenderer);
	PieceTextures[B_KING] = LoadTexture("resources/b-king.png", pSDLRenderer);

	WinTextures[0] = LoadTexture("resources/win_black.png", pSDLRenderer);
	WinTextures[1] = LoadTexture("resources/win_white.png", pSDLRenderer);
	StalemateTexture = LoadTexture("resources/stalemate.png", pSDLRenderer);
}

void Renderer::UpdateFPS()
{
	if (mFpsCounter.Update())
	{
		float fps = mFpsCounter.GetFps();
		SDL_SetWindowTitle(pWindow, ("FPS: " + std::to_string(fps)).c_str());
	}
}

void Renderer::Restart()
{
	ActiveIdx = -1;
	ActiveMoves = 0ULL;
	LastMove.Restart();
}

void Renderer::ChangeScreenSize(const int width, const int height)
{
	
	ScreenWidth = width;
	ScreenHeight = height;
	BoardSize = std::min(ScreenWidth - mBoardSettings.OffsetLeft - mBoardSettings.OffsetRight, ScreenHeight - mBoardSettings.OffsetTop - mBoardSettings.OffsetBottom);
}

int8_t Renderer::GetIdxAtPosition(const float posX, const float posY) const
{
	const float x = posX - BoardX;
	const float y = posY - BoardY;
	if (x >= BoardSize or x < 0 or y >= BoardSize or y < 0)
	{
		return -1;
	}
	const float cellSize = BoardSize / 8.f;
	const auto ro = IsRotated ? 7 : 0;
	const int8_t row = std::abs(ro - int(y / cellSize));
	const int8_t col = std::abs(ro - int(x / cellSize));
	const int8_t idx = (7-row) * 8 + col;
	return idx;
}

PieceTypeAndColor Renderer::GetPromotePieceAtPosition(const bool isWhite, const float posX, const float posY) const
{
	const float x = (posX - BoardX);
	const float y = (posY - BoardY);
	const SDL_FRect& r = PromoteRect;
	if (x < r.x || x > r.x+r.w || y < r.y || y > r.y+r.h)
		return NONE;
	const float promY = y - r.y;
	const int8_t row = promY / r.w;
	int8_t i = 0;
	if (isWhite)
	{
		for (int pc = W_KNIGHT; pc <= W_QUEEN; ++pc)
		{
			if (i == row)
				return PieceTypeAndColor(pc);
			++i;
		}
	}
	else
	{
		for (int pc = B_QUEEN; pc >= B_KNIGHT; --pc)
		{
			if (i == row)
				return PieceTypeAndColor(pc);
			++i;
		}		
	}	
	return NONE;
}

Renderer::Renderer()
{
	InitSDL();
	InitTTF();
	pFont = TTF_OpenFont("resources/Coolvetica.otf", 20);
	if (!pFont) {
		std::cout << "Chyba při načítání fontu: " << SDL_GetError() << std::endl;
	}
	loadAllTextures();
}

Renderer::~Renderer()
{
	ClearAllTextures();
	TTF_DestroySurfaceTextEngine(pTextEngine);
	if (pFont) {
        TTF_CloseFont(pFont);
        pFont = nullptr;
    }
	TTF_Quit();
	SDL_DestroyRenderer(pSDLRenderer);
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

void Animator::StartAnimation(const float fromX, const float fromY, const float toX, const float toY)
{
	StartTime = SDL_GetTicks();
	FromX = fromX;
	FromY = fromY;
	ToX = toX;
	ToY = toY;
}

void Animator::GetActualPos(float &x, float &y)
{
	Uint64 actTime = SDL_GetTicks();
	Uint64 elapsedTime = actTime - StartTime;

	double t = std::min(static_cast<double>(elapsedTime) / static_cast<double>(ANIMATION_TIME), 1.0);
	t = 1.0 - pow(1.0-t, 3.0);
	x = FromX + t * (ToX - FromX);
	y = FromY + t * (ToY - FromY);
}

bool Animator::isFinished()
{
	return SDL_GetTicks() - StartTime > ANIMATION_TIME;
}