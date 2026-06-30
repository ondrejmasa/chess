#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include <iostream>
#include <map>
#include "global.h"
#include "board.h"
#include "Vector2.h"


class Renderer {
private:
	float X;
	float Y;
	float Size;
	std::array<SDL_Texture*, 12> pieceTextures;
	SDL_Texture* LoadTexture(const std::string& path, SDL_Renderer* renderer);
	void ClearPiecesTextures();
public:
	void Render(SDL_Renderer* renderer, const Board& board, const uint8_t activeIdx, const BitBoard activeMoves, const float x, const float y, const float size);
	void loadAllPieces(SDL_Renderer* renderer);
	int8_t GetIdxAtPosition(const float posX, const float posY) const;
	~Renderer();
};