# pragma once
#include <cstdint>
#include <array>
#include <vector>
#include <iostream>
#include "global.h"


enum PieceTypeAndColor { W_PAWN = 0, W_KNIGHT = 1, W_BISHOP = 2, W_ROOK = 3, W_QUEEN = 4, W_KING = 5, B_PAWN = 6, B_KNIGHT = 7, B_BISHOP = 8, B_ROOK = 9, B_QUEEN = 10, B_KING = 11, NONE = -1};

enum SquareColor {WHITE, BLACK, EMPTY};

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

struct Castle {
    bool KingMoved;
    bool LeftRookMoved;
    bool RightRookMoved;
    const bool CanCastleRight() const {return !RightRookMoved and !KingMoved;};
    const bool CanCastleLeft() const {return !LeftRookMoved and !KingMoved;};
    void Restart() {
        KingMoved = false;
        LeftRookMoved = false;
        RightRookMoved = false;
    }
};

struct MoveData {
    uint8_t From;
    uint8_t To;
    bool IsWhite;
    int Pc;
};

struct BoardState {
    BitBoard WhiteEnPassant;
    BitBoard BlackEnPassant;
    BitBoard WhiteCheckers;
    BitBoard BlackCheckers;
    BitBoard WhiteAttacks;
    BitBoard BlackAttacks;
    BitBoard WhitePins;
    BitBoard BlackPins;
    Castle WhiteCastle;
    Castle BlackCastle;
    MoveData Move;
    PieceTypeAndColor capturedPiece;
};

class Board {
private:
    const BitBoard NotAFile = 0xFEFEFEFEFEFEFEFEULL;
    const BitBoard NotABFile = 0xFCFCFCFCFCFCFCFCULL;
    const BitBoard NotHFile = 0x7F7F7F7F7F7F7F7FULL;
    const BitBoard NotGHFile = 0x3F3F3F3F3F3F3F3FULL;
    BitBoard WhiteEnPassant = 0ULL;
    BitBoard BlackEnPassant = 0ULL;
    BitBoard WhiteCheckers = 0ULL;
    BitBoard BlackCheckers = 0ULL;
    BitBoard WhiteAttacks;
    BitBoard BlackAttacks;
    BitBoard WhitePins;
    BitBoard BlackPins;
    Castle WhiteCastle;
    Castle BlackCastle;
    std::array<BitBoard, 64> KnightMoves;
    std::array<BitBoard, 64> KingMoves;
    std::array<std::array<BitBoard, 64>, 64> RayFromTo;
    MoveData LastMove;
    std::vector<BoardState> BoardHistory;
    void InitRayFromTo();
	void InitBoard();
    void InitKnightMoves();
    void InitKingMoves();
    BitBoard GetRookMoves(const uint8_t idx) const;
    BitBoard GetBishopMoves(const uint8_t idx) const;
    BitBoard GetRookMask(const uint8_t idx) const;
    BitBoard GetBishopMask(const uint8_t idx) const;
    BitBoard GetPawnMoves(const uint8_t idx) const;
    BitBoard GetPawnAttacks(const uint8_t idx) const;
    BitBoard GetAllWhiteAttacks();
    BitBoard GetAllBlackAttacks();
    BitBoard GetWhiteCheckers() const;
    BitBoard GetBlackCheckers() const;
    BitBoard GetActiveAttacks(const uint8_t idx) const;
    BitBoard GetWhitePins() const;
    BitBoard GetBlackPins() const;
    BitBoard GetAllWhiteMoves() const;
    BitBoard GetAllBlackMoves() const;
    BitBoard GetWhitePinRay(const uint8_t idx) const;
    BitBoard GetBlackPinRay(const uint8_t idx) const;
    bool CanWhiteCastleRight() const;
    bool CanWhiteCastleLeft() const;
    bool CanBlackCastleRight() const;
    bool CanBlackCastleLeft() const; 
    int GetEvaluationPieceValue(const bool isWhite) const;
    int GetEvaluationByColor(const bool isWhite) const;
    std::vector<MoveData> GetAllWhiteMovesFromTo() const;
    std::vector<MoveData> GetAllBlackMovesFromTo() const;
public:
    std::array<BitBoard, 12> PieceBB;
    BitBoard WhiteBB;
    BitBoard BlackBB;
    BitBoard OccupiedBB;
    int GetPieceAtIdx(const uint8_t idx) const;
    BitBoard GetActiveMoves(const uint8_t idx) const;
    void Move(const uint8_t from, const uint8_t to);
    bool UndoMove();
    GameState UpdateAfterMove();
    SquareColor GetColor(const int pc) const;
    SquareColor GetColorAtIdx(const uint8_t idx) const;
    bool IsOccupiedAtIdx(const uint8_t idx) const;
    const MoveData& GetLastMove() const; 
    void PromotePawn(const PieceTypeAndColor toPc);
    void Restart();
    std::vector<MoveData> GetAllMovesFromTo(const bool isWhite) const;
    int GetEvaluation() const;
    Board();
};