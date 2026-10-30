# pragma once
#include <array>
#include <cstdint>

typedef uint64_t BitBoard;

enum GameState { GAME, W_WIN, B_WIN, STALEMATE, W_PROMOTE, B_PROMOTE };

enum PieceTypeAndColor { W_PAWN = 0, W_KNIGHT = 1, W_BISHOP = 2, W_ROOK = 3, W_QUEEN = 4, W_KING = 5, B_PAWN = 6, B_KNIGHT = 7, B_BISHOP = 8, B_ROOK = 9, B_QUEEN = 10, B_KING = 11, NONE = -1};

inline void printBitboard(BitBoard bitBoard)
{
    for (auto i = 7; i >= 0; --i)
    {
        for (auto j = 0; j < 8; ++j)
        {
            auto idx = 8*i + j;
            std::cout << (((bitBoard >> idx) & 1ULL) ? '1' : '.') << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}