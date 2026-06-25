# pragma once
#include <cstdint>
#include <array>

typedef uint64_t BitBoard;

enum PieceTypeAndColor { W_PAWN = 0, W_KNIGHT = 1, W_BISHOP = 2, W_ROOK = 3, W_QUEEN = 4, W_KING = 5, B_PAWN = 6, B_KNIGHT = 7, B_BISHOP = 8, B_ROOK = 9, B_QUEEN = 10, B_KING = 11};

enum Squares {
    A1, B1, C1, D1, E1, F1, G1, H1,  // 0 - 7
    A2, B2, C2, D2, E2, F2, G2, H2,  // 8 - 15
    A3, B3, C3, D3, E3, F3, G3, H3,  // 16 - 23
    A4, B4, C4, D4, E4, F4, G4, H4,  // 24 - 31
    A5, B5, C5, D5, E5, F5, G5, H5,  // 32 - 39
    A6, B6, C6, D6, E6, F6, G6, H6,  // 40 - 47
    A7, B7, C7, D7, E7, F7, G7, H7,  // 48 - 55
    A8, B8, C8, D8, E8, F8, G8, H8   // 56 - 63
};

class Board {
private:
    std::array<BitBoard, 64> knightMoves;
	void InitBoard();
    void InitKnightMoves();
    const BitBoard notAFile = 0xFEFEFEFEFEFEFEFEULL;
    const BitBoard notABFile = 0xFCFCFCFCFCFCFCFCULL;
    const BitBoard notHFile = 0x7F7F7F7F7F7F7F7FULL;
    const BitBoard notGHFile = 0x3F3F3F3F3F3F3F3FULL;
public:
    std::array<BitBoard, 12> pieceBB;
    BitBoard whiteBB;
    BitBoard blackBB;
    BitBoard occupiedBB;
    int GetPieceAtIdx(const int8_t idx);
    BitBoard GetActiveMoves(const int8_t idx);
    void Move(const int from, const int to);
    const bool IsWhite(const int pc) const;
    const bool isWhiteAtIdx(const int8_t idx) const;
    const bool isOccupiedAtIdx(const int8_t idx) const;
    Board();
    
};