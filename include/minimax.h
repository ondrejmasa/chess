#pragma once
#include "board.h"

struct MinimaxMove {
    int8_t From;
    int8_t To;
    int Eval; 
    PieceTypeAndColor PromotePc;  
};

int Minimax(Board& board, const GameState gamestate, const uint8_t depth, const bool isWhite);

MinimaxMove GetBestMove(Board& board, const int depth, const bool isWhite);


