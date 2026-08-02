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
    if (IsComputing)
    {
        if (MoveFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
        {
            IsComputing = false;
            return MoveFuture.get();
        }
        else
        {
            return {-1, -1, -1, NONE};
        }
    }

    IsComputing = true;
    ThreadBoard = board;
    MoveFuture = std::async(std::launch::async, GetBestMove, std::ref(ThreadBoard), Depth, IsWhite);
    return {-1, -1, -1, NONE};
}
