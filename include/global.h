# pragma once
#include <array>
#include <cstdint>

typedef uint64_t BitBoard;

enum GameState { GAME, W_WIN, B_WIN, STALEMATE, W_PROMOTE, B_PROMOTE };

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