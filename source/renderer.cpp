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

void Renderer::Render(SDL_Renderer* renderer, const Board& board, const uint8_t activeIdx, const BitBoard activeMoves, const float leftOffset, const float TopOffset, const float gridSize)
{
	x = leftOffset; y = TopOffset; size = gridSize;
	// background
	const float cellSize = size / 8.f;
	SDL_FRect rect = { x, y, size, size };
	SDL_SetRenderDrawColor(renderer, 242, 232, 231, 255);
	SDL_RenderFillRect(renderer, &rect);

	// dark squares
	SDL_SetRenderDrawColor(renderer, 163, 82, 78, 255);
	for (auto i = 0; i < 8; ++i) {
		for (auto j = 1 - (i % 2); j < 8; j += 2) {
			rect = { x + j * cellSize, y + i * cellSize, cellSize, cellSize };
			SDL_RenderFillRect(renderer, &rect);
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
				rect = { x + j * cellSize + o, y + (7 - i) * cellSize + o, sz, sz };
				SDL_SetRenderDrawColor(renderer, 150, 150, 150, 100);
				SDL_RenderFillRect(renderer, &rect);
			}
			// pieces
			for (int pc = W_PAWN; pc <= B_KING; ++pc)
			{
				if ((board.pieceBB[pc] >> idx) & 1ULL)
				{
					rect = { x + j * cellSize, y + (7-i) * cellSize, cellSize, cellSize };
					if (activeIdx == idx)
					{
						SDL_SetRenderDrawColor(renderer, 163, 120, 71, 255);
						SDL_RenderFillRect(renderer, &rect);
					}
					SDL_RenderTexture(renderer, pieceTextures[pc], nullptr, &rect);
					break;
				}
			}
		}
	}
}

void Renderer::loadAllPieces(SDL_Renderer* renderer)
{
	pieceTextures[W_PAWN] = LoadTexture("resources/w-pawn.png", renderer);
	pieceTextures[W_KNIGHT] = LoadTexture("resources/w-knight.png", renderer);
	pieceTextures[W_BISHOP] = LoadTexture("resources/w-bishop.png", renderer);
	pieceTextures[W_ROOK] = LoadTexture("resources/w-rook.png", renderer);
	pieceTextures[W_QUEEN] = LoadTexture("resources/w-queen.png", renderer);
	pieceTextures[W_KING] = LoadTexture("resources/w-king.png", renderer);

	pieceTextures[B_PAWN] = LoadTexture("resources/b-pawn.png", renderer);
	pieceTextures[B_KNIGHT] = LoadTexture("resources/b-knight.png", renderer);
	pieceTextures[B_BISHOP] = LoadTexture("resources/b-bishop.png", renderer);
	pieceTextures[B_ROOK] = LoadTexture("resources/b-rook.png", renderer);
	pieceTextures[B_QUEEN] = LoadTexture("resources/b-queen.png", renderer);
	pieceTextures[B_KING] = LoadTexture("resources/b-king.png", renderer);
}

int8_t Renderer::GetIdxAtPosition(const float posX, const float posY) const
{
	float boardX = (posX - x);
	float boardY = (posY - y);
	if (boardX >= size || boardX < 0 || boardY >= size || boardY < 0)
	{
		return -1;
	}
	float cellSize = size / 8.f;
	int8_t row = boardY / cellSize;
	int8_t col = boardX / cellSize;
	int8_t idx = (7-row) * 8 + col;
	return idx;
}

Renderer::~Renderer()
{
	ClearPiecesTextures();
}
