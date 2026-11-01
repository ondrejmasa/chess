#include "minimax.h"

int Minimax(Board &board, const GameState gamestate, const int depth, const bool isWhite, int alpha, int beta, const std::atomic<bool>& stopFlag)
 {
    if (gamestate == W_WIN)
        return 9999999 + depth;
    else if (gamestate == B_WIN)
        return -9999999 - depth;
    else if (gamestate != GAME or depth == 0)
    {
        return board.GetEvaluation();    
    }
    int bestVal = isWhite ? -99999998 : 99999998;

    std::vector<MoveData> moves = board.GetAllMovesFromTo(isWhite); 

    std::sort(moves.begin(), moves.end(), [board](const MoveData& a, const MoveData& b) {
        int valA = board.GetScoreMove(a);
        int valB = board.GetScoreMove(b);
        return valA > valB;
    });

    for (const MoveData& m : moves)
    {
        if (stopFlag.load()) 
        {
            return 0;
        }
        int val;
        board.Move(m.From, m.To);
        GameState gs = board.UpdateAfterMove();  
        if (gs == W_PROMOTE)
        {
            board.UndoMove();
            bool stop = false;
            for (int pc = W_KNIGHT; pc <= W_QUEEN; ++pc)
            {
                board.Move(m.From, m.To);
                // board.UpdateAfterMove(); 
                board.PromotePawn((PieceTypeAndColor)pc);
                val = Minimax(board, GAME, depth-1, not isWhite, alpha, beta, stopFlag);   
                bestVal = std::max(bestVal, val); 
                board.UndoMove();
                alpha = std::max(alpha, val);
                if (beta <= alpha)
                {
                    stop = true;
                    break;
                }
            }
            if (stop) break;
        } 
        else if (gs == B_PROMOTE)
        {
            board.UndoMove();

            bool stop = false;
            for (int pc = B_KNIGHT; pc <= B_QUEEN; ++pc)
            {
                board.Move(m.From, m.To);
                // board.UpdateAfterMove(); 
                board.PromotePawn((PieceTypeAndColor)pc);
                val = Minimax(board, GAME, depth-1, not isWhite, alpha, beta, stopFlag);   
                bestVal = std::min(bestVal, val); 
                board.UndoMove();
                beta = std::min(beta, val);
                if (beta <= alpha)
                {
                    stop = true;
                    break;
                }
            }
            if (stop) break;
        }   
        else 
        {
            val = Minimax(board, gs, depth-1, not isWhite, alpha, beta, stopFlag);   
            board.UndoMove();
            if (isWhite)
            {
                bestVal = std::max(bestVal, val);
                alpha = std::max(alpha, val);     
            }
            else
            {
                bestVal = std::min(bestVal, val);
                beta = std::min(beta, val);

            }
            if (beta <= alpha) break;
           
        }
    }
    return bestVal;
}


MinimaxMove GetBestMove(Board &board, const int depth, const bool isWhite, const std::atomic<bool>& stopFlag)
{
    std::vector<MoveData> moves = board.GetAllMovesFromTo(isWhite);    
    MinimaxMove move;
    int bestVal = isWhite ? -99999998 : 99999998;
    int alpha = INT_MIN;
    int beta = INT_MAX;

    if (!moves.empty())
    {
        move.From = moves[0].From;
        move.To = moves[0].To;
        move.Eval = bestVal;
        move.PromotePc = NONE;
    }
    
    std::sort(moves.begin(), moves.end(), [board](const MoveData& a, const MoveData& b) {
        int valA = board.GetScoreMove(a);
        int valB = board.GetScoreMove(b);
        return valA > valB;
    });

    for (const MoveData& m : moves)
    {
        if (stopFlag.load()) 
        {
            return {-1, -1, -1, NONE};
        }
        int val;
        board.Move(m.From, m.To);
        GameState gs = board.UpdateAfterMove();  
        if (gs == W_PROMOTE)
        {
            board.UndoMove();

            for (int pc = W_KNIGHT; pc <= W_QUEEN; ++pc)
            {
                board.Move(m.From, m.To);
                // board.UpdateAfterMove(); 
                board.PromotePawn((PieceTypeAndColor)pc);
                val = Minimax(board, GAME, depth-1, not isWhite, alpha, beta, stopFlag);   
                board.UndoMove();
                if (val > bestVal)
                {
                    bestVal = val;
                    move.From = m.From;
                    move.To = m.To;
                    move.Eval = bestVal;
                    move.PromotePc = (PieceTypeAndColor)pc;
                }
                alpha = std::max(alpha, val);
            }
        } 
        else if (gs == B_PROMOTE)
        {
            board.UndoMove();

            for (int pc = B_KNIGHT; pc <= B_QUEEN; ++pc)
            {
                board.Move(m.From, m.To);
                // board.UpdateAfterMove(); 
                board.PromotePawn((PieceTypeAndColor)pc);
                val = Minimax(board, GAME, depth-1, not isWhite, alpha, beta, stopFlag);   
                board.UndoMove();
                if (val < bestVal)
                {
                    bestVal = val;
                    move.From = m.From;
                    move.To = m.To;
                    move.Eval = bestVal;
                    move.PromotePc = (PieceTypeAndColor)pc;
                }  
                beta = std::min(beta, val);
            }
        }   
        else 
        {
            val = Minimax(board, gs, depth-1, not isWhite, alpha, beta, stopFlag);   
            board.UndoMove();
            if (isWhite)
            {
                alpha = std::max(alpha, val);
                if (val > bestVal)
                {
                    bestVal = val;
                    move.From = m.From;
                    move.To = m.To;
                    move.Eval = bestVal;
                    move.PromotePc = NONE;
                }
            }
            else
            {
                beta = std::min(beta, val);
                if (val < bestVal)
                {
                    bestVal = val;
                    move.From = m.From;
                    move.To = m.To;
                    move.Eval = bestVal;
                    move.PromotePc = NONE;
                }
            }
        }
    }
    return move;
}