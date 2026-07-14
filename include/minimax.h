#include "board.h"

struct MinimaxMove {
    uint8_t From;
    uint8_t To;
    int Eval;    
};

int Minimax(Board& board, const GameState gamestate, const uint8_t depth, const bool isWhite);

MinimaxMove GetBestMove(Board& board, const int depth, const bool isWhite);


