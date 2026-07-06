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

void Renderer::ClearPiecesTextures()
{
	for (auto const& texture : pieceTextures) {
		if (texture != nullptr) {
			SDL_DestroyTexture(texture);
		}
	}
	pieceTextures.fill(nullptr);
}

void Renderer::RenderWinState(SDL_Renderer* renderer, const bool IsWinnerWhite)
{

}

void Renderer::InitTTF()
{
	pTextEngine = TTF_CreateRendererTextEngine(pSDLRenderer);
}

void Renderer::InitSDL()
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer("", SCREEN_WIDTH, SCREEN_HEIGHT, 0, &pWindow, &pSDLRenderer);
	SDL_SetRenderVSync(pSDLRenderer, 1);
}

void Renderer::Render(const Board &board, const uint8_t activeIdx, const BitBoard activeMoves, const GameState gameState)
{
	SDL_SetRenderDrawBlendMode(pSDLRenderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(pSDLRenderer, 160, 160, 180, 255);
	SDL_RenderClear(pSDLRenderer);


	RenderBoard(board, activeIdx, activeMoves);	
	switch (gameState)
	{
		case W_PROMOTE:
		case B_PROMOTE:
			const MoveData lm = board.GetLastMove();
			const uint8_t col = lm.To % 8;
			RenderPromote(gameState == W_PROMOTE, col);
			break;
	}
	SDL_RenderPresent(pSDLRenderer);
}

void Renderer::RenderBoard(const Board &board, const uint8_t activeIdx, const BitBoard activeMoves)
{
	// background
	const float cellSize = BoardSize / 8.f;
	SDL_FRect rect = { BoardX, BoardY, BoardSize, BoardSize };
	SDL_SetRenderDrawColor(pSDLRenderer, 242, 232, 231, 255);
	SDL_RenderFillRect(pSDLRenderer, &rect);

	// dark squares
	SDL_SetRenderDrawColor(pSDLRenderer, 163, 82, 78, 255);
	for (auto i = 0; i < 8; ++i) {
		for (auto j = 1 - (i % 2); j < 8; j += 2) {
			rect = { BoardX + j * cellSize, BoardY + i * cellSize, cellSize, cellSize };
			SDL_RenderFillRect(pSDLRenderer, &rect);
		}
	}

	for (auto i = 7; i >= 0; --i) {
		for (auto j = 0; j < 8; ++j) {
			auto idx = i * 8 + j;
			// active moves
			if (activeMoves >> idx & 1ULL)
			{
				float sz = cellSize * 0.75f;
				float o = (cellSize - sz) / 2;
				rect = { BoardX + j * cellSize + o, BoardY + (7 - i) * cellSize + o, sz, sz };
				SDL_SetRenderDrawColor(pSDLRenderer, 150, 150, 150, 100);
				SDL_RenderFillRect(pSDLRenderer, &rect);
			}
			// pieces
			for (int pc = W_PAWN; pc <= B_KING; ++pc)
			{
				if ((board.PieceBB[pc] >> idx) & 1ULL)
				{
					rect = { BoardX + j * cellSize, BoardY + (7-i) * cellSize, cellSize, cellSize };
					if (activeIdx == idx)
					{
						SDL_SetRenderDrawColor(pSDLRenderer, 163, 120, 71, 255);
						SDL_RenderFillRect(pSDLRenderer, &rect);
					}
					SDL_RenderTexture(pSDLRenderer, pieceTextures[pc], nullptr, &rect);
					break;
				}
			}
		}
	}
}

void Renderer::RenderPromote(const bool isWhite, const uint8_t col)
{
	SDL_FRect& r = PromoteRect;
	r.w = BoardSize / 8.f;
	r.h = r.w*4.f;
	r.x = BoardX+r.w*col;
	r.y = isWhite ? BoardY : BoardY + BoardSize - r.h; 
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
			SDL_RenderTexture(pSDLRenderer, pieceTextures[pc], nullptr, &r2);
			r2.y += r2.w;
		}
	}
	else
	{
		for (int pc = B_QUEEN; pc >= B_KNIGHT; --pc)
		{
			SDL_RenderTexture(pSDLRenderer, pieceTextures[pc], nullptr, &r2);
			r2.y += r2.w;
		}		
	}
}

void Renderer::loadAllPieces()
{
	pieceTextures[W_PAWN] = LoadTexture("resources/w-pawn.png", pSDLRenderer);
	pieceTextures[W_KNIGHT] = LoadTexture("resources/w-knight.png", pSDLRenderer);
	pieceTextures[W_BISHOP] = LoadTexture("resources/w-bishop.png", pSDLRenderer);
	pieceTextures[W_ROOK] = LoadTexture("resources/w-rook.png", pSDLRenderer);
	pieceTextures[W_QUEEN] = LoadTexture("resources/w-queen.png", pSDLRenderer);
	pieceTextures[W_KING] = LoadTexture("resources/w-king.png", pSDLRenderer);

	pieceTextures[B_PAWN] = LoadTexture("resources/b-pawn.png", pSDLRenderer);
	pieceTextures[B_KNIGHT] = LoadTexture("resources/b-knight.png", pSDLRenderer);
	pieceTextures[B_BISHOP] = LoadTexture("resources/b-bishop.png", pSDLRenderer);
	pieceTextures[B_ROOK] = LoadTexture("resources/b-rook.png", pSDLRenderer);
	pieceTextures[B_QUEEN] = LoadTexture("resources/b-queen.png", pSDLRenderer);
	pieceTextures[B_KING] = LoadTexture("resources/b-king.png", pSDLRenderer);
}

void Renderer::UpdateFPS()
{
	if (mFpsCounter.Update())
	{
		float fps = mFpsCounter.GetFps();
		SDL_SetWindowTitle(pWindow, ("FPS: " + std::to_string(fps)).c_str());
	}
}

int8_t Renderer::GetIdxAtPosition(const float posX, const float posY) const
{
	const float x = posX - BoardX;
	const float y = posY - BoardY;
	if (x >= BoardSize || x < 0 || y >= BoardSize || y < 0)
	{
		return -1;
	}
	const float cellSize = BoardSize / 8.f;
	const int8_t row = y / cellSize;
	const int8_t col = x / cellSize;
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
	loadAllPieces();
}

Renderer::~Renderer()
{
	ClearPiecesTextures();
	// TTF_DestroyText(textHodiny);
	// TTF_DestroyFont(font);
	TTF_DestroySurfaceTextEngine(pTextEngine);
	TTF_Quit();
	SDL_DestroyRenderer(pSDLRenderer);
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}
