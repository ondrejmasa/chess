#include "minimax.h"

int Minimax(Board &board, const GameState gamestate, const int depth, const bool isWhite)
 {
    if (gamestate == W_WIN)
        return 9999999;
    else if (gamestate == B_WIN)
        return -9999999;
    else if (gamestate != GAME or depth == 0)
    {
        return board.GetEvaluation();    
    }
    int bestVal = isWhite ? -99999999 : 99999999;
    const std::vector<MoveData>& moves = board.GetAllMovesFromTo(isWhite); 
    for (const MoveData& m : moves)
    {
        board.Move(m.From, m.To);
        GameState gs = board.UpdateAfterMove();     
        int val = Minimax(board, gs, depth-1, not isWhite);   
        board.UndoMove();
        bestVal = isWhite ? std::max(bestVal, val) : std::min(bestVal, val); 
    }
    return bestVal;
}


MinimaxMove GetBestMove(Board &board, const int depth, const bool isWhite)
{
    std::vector<MoveData> moves = board.GetAllMovesFromTo(isWhite);    
    MinimaxMove move;
    int bestVal = isWhite ? -9999999 : 9999999;
    for (const MoveData& m : moves)
    {
        board.Move(m.From, m.To);
        GameState gs = board.UpdateAfterMove();
        int val = Minimax(board, gs, depth-1, not isWhite);
        board.UndoMove();
        if (isWhite)
        {
            if (val > bestVal)
            {
                bestVal = val;
                move.From = m.From;
                move.To = m.To;
                move.Eval = bestVal;
            }
        }
        else
        {
            if (val < bestVal)
            {
                bestVal = val;
                move.From = m.From;
                move.To = m.To;
                move.Eval = bestVal;
            }            
        }
    }
    return move;
}