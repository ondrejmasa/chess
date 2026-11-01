#pragma once
#include "board.h"
#include "minimax.h"
#include "renderer.h"
#include <SDL3/SDL.h>
#include <future>

class Player {
protected:
    const bool IsWhite;
    Renderer& mRenderer;
public: 
    Player(Renderer& renderer, const bool isWhite) : IsWhite{isWhite}, mRenderer{renderer} { };
    virtual ~Player() = default;
    virtual MinimaxMove GenerateMove(Board& board) = 0;
    virtual void Stop() {};
};


class User: public Player {
public:
    using Player::Player;
    MinimaxMove GenerateMove(Board& board);
};

class Computer: public Player {
private:
    const int Depth = 4;
    bool IsComputing = false;
    std::future<MinimaxMove> MoveFuture;
    Board ThreadBoard;
    std::atomic<bool> StopComputing{false};
public:
    using Player::Player;
    ~Computer();
    MinimaxMove GenerateMove(Board& board);
    void Stop() override;
};