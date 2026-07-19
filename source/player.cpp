#include "player.h"


MinimaxMove User::GenerateMove(Board &board)
{
    MinimaxMove result {-1, -1, -1, NONE};

    static bool isMousePressed;
    float mouseX, mouseY;
    uint32_t mouseButtons = SDL_GetMouseState(&mouseX, &mouseY);
    if (mouseButtons & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)) 
    {
        if (not isMousePressed)
        {
            isMousePressed = true;
            int8_t idx = mRenderer.GetIdxAtPosition(mouseX, mouseY);
            if (idx != mRenderer.ActiveIdx)
            {
                if ((mRenderer.ActiveMoves >> idx) & 1ULL)
                {
                    result.From = mRenderer.ActiveIdx;
                    result.To = idx;
                    mRenderer.ActiveIdx = -1;
                    mRenderer.ActiveMoves = 0ULL;
                }    
                else
                {
                    SquareColor clr = board.GetColorAtIdx(idx);
                    if (clr != EMPTY and IsWhite == (clr == BLACK))
                    {
                        std::cout << "wrong color\n";
                        mRenderer.ActiveIdx = -1;
                        mRenderer.ActiveMoves = 0ULL;
                        return result;
                    }
                    mRenderer.ActiveIdx = idx;
                    mRenderer.ActiveMoves = board.GetActiveMoves(idx);
                }        
            }
        }
    }
    else
        isMousePressed = false;

    return result;
}

MinimaxMove Computer::GenerateMove(Board &board)
{
    return GetBestMove(board, Depth, IsWhite);
}
