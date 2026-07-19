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
    int bestVal = isWhite ? -99999998 : 99999998;

    const std::vector<MoveData>& moves = board.GetAllMovesFromTo(isWhite); 
    for (const MoveData& m : moves)
    {
        int val;
        board.Move(m.From, m.To);
        GameState gs = board.UpdateAfterMove();  
        if (gs == W_PROMOTE)
        {
            board.UndoMove();

            for (int pc = W_KNIGHT; pc <= W_QUEEN; ++pc)
            {
                board.Move(m.From, m.To);
                board.UpdateAfterMove(); 
                board.PromotePawn((PieceTypeAndColor)pc);
                val = Minimax(board, GAME, depth-1, not isWhite);   
                bestVal = std::max(bestVal, val); 
                board.UndoMove();
            }
        } 
        else if (gs == B_PROMOTE)
        {
            board.UndoMove();

            for (int pc = B_KNIGHT; pc <= B_QUEEN; ++pc)
            {
                board.Move(m.From, m.To);
                board.UpdateAfterMove(); 
                board.PromotePawn((PieceTypeAndColor)pc);
                val = Minimax(board, GAME, depth-1, not isWhite);   
                bestVal = std::min(bestVal, val); 
                board.UndoMove();
            }
        }   
        else 
        {
            val = Minimax(board, gs, depth-1, not isWhite);   
            board.UndoMove();
            bestVal = isWhite ? std::max(bestVal, val) : std::min(bestVal, val); 
        }
    }
    return bestVal;
}


MinimaxMove GetBestMove(Board &board, const int depth, const bool isWhite)
{
    std::vector<MoveData> moves = board.GetAllMovesFromTo(isWhite);    
    MinimaxMove move;
    int bestVal = isWhite ? -99999998 : 99999998;

    if (!moves.empty())
    {
        move.From = moves[0].From;
        move.To = moves[0].To;
        move.Eval = bestVal;
        move.PromotePc = NONE;
    }

    for (const MoveData& m : moves)
    {
        int val;
        board.Move(m.From, m.To);
        GameState gs = board.UpdateAfterMove();  
        if (gs == W_PROMOTE)
        {
            board.UndoMove();

            for (int pc = W_KNIGHT; pc <= W_QUEEN; ++pc)
            {
                board.Move(m.From, m.To);
                board.UpdateAfterMove(); 
                board.PromotePawn((PieceTypeAndColor)pc);
                val = Minimax(board, GAME, depth-1, not isWhite);   
                board.UndoMove();
                if (val > bestVal)
                {
                    bestVal = val;
                    move.From = m.From;
                    move.To = m.To;
                    move.Eval = bestVal;
                    move.PromotePc = (PieceTypeAndColor)pc;
                }
            }
        } 
        else if (gs == B_PROMOTE)
        {
            board.UndoMove();

            for (int pc = B_KNIGHT; pc <= B_QUEEN; ++pc)
            {
                board.Move(m.From, m.To);
                board.UpdateAfterMove(); 
                board.PromotePawn((PieceTypeAndColor)pc);
                val = Minimax(board, GAME, depth-1, not isWhite);   
                board.UndoMove();
                if (val < bestVal)
                {
                    bestVal = val;
                    move.From = m.From;
                    move.To = m.To;
                    move.Eval = bestVal;
                    move.PromotePc = (PieceTypeAndColor)pc;
                }  
            }
        }   
        else 
        {
            val = Minimax(board, gs, depth-1, not isWhite);   
            board.UndoMove();
            if (isWhite and val > bestVal)
            {
                bestVal = val;
                move.From = m.From;
                move.To = m.To;
                move.Eval = bestVal;
                move.PromotePc = NONE;
            }
            else if (not isWhite and val < bestVal)
            {
                bestVal = val;
                move.From = m.From;
                move.To = m.To;
                move.Eval = bestVal;
                move.PromotePc = NONE;
            }
        }
    }
    return move;
}