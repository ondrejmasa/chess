#include "board.h"
#include "minimax.h"
#include "renderer.h"
#include <SDL3/SDL.h>

class Player {
protected:
    const bool IsWhite;
    Renderer& mRenderer;
public: 
    Player(Renderer& renderer, const bool isWhite) : IsWhite{isWhite}, mRenderer{renderer} { };
    virtual ~Player() = default;
    virtual MinimaxMove GenerateMove(Board& board) = 0;
};


class User: public Player {
public:
    using Player::Player;
    MinimaxMove GenerateMove(Board& board);
};

class Computer: public Player {
private:
    const int Depth = 4;
public:
    using Player::Player;
    MinimaxMove GenerateMove(Board& board);
};