#pragma once
#include "board.h"
#include <climits>
#include <algorithm>
#include <atomic>

struct MinimaxMove {
    int8_t From;
    int8_t To;
    int Eval; 
    PieceTypeAndColor PromotePc;  
};

int Minimax(Board &board, const GameState gamestate, const int depth, const bool isWhite, int alpha, int beta, const std::atomic<bool>& stopFlag);

MinimaxMove GetBestMove(Board& board, const int depth, const bool isWhite, const std::atomic<bool>& stopFlag);


