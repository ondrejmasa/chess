#include "board.h"

void Board::InitBoard()
{
	occupiedBB = 0ULL;
	pieceBB.fill(0ULL);

	for (int sq = A2; sq <= H2; ++sq) pieceBB[W_PAWN] |= (1ULL << sq);
	for (int sq = A7; sq <= H7; ++sq) pieceBB[B_PAWN] |= (1ULL << sq);

	pieceBB[W_ROOK] = (1ULL << A1) | (1ULL << H1);
	pieceBB[B_ROOK] = (1ULL << A8) | (1ULL << H8);
	pieceBB[W_KNIGHT] = (1ULL << B1) | (1ULL << G1);
	pieceBB[B_KNIGHT] = (1ULL << B8) | (1ULL << G8);
	pieceBB[W_BISHOP] = (1ULL << C1) | (1ULL << F1);
	pieceBB[B_BISHOP] = (1ULL << C8) | (1ULL << F8);
	pieceBB[W_QUEEN] = (1ULL << D1);
	pieceBB[B_QUEEN] = (1ULL << D8);
	pieceBB[W_KING] = (1ULL << E1);
	pieceBB[B_KING] = (1ULL << E8);

	whiteBB = pieceBB[W_PAWN] | pieceBB[W_ROOK] | pieceBB[W_KNIGHT] | pieceBB[W_BISHOP] | pieceBB[W_QUEEN];
	blackBB = pieceBB[B_PAWN] | pieceBB[B_ROOK] | pieceBB[B_KNIGHT] | pieceBB[B_BISHOP] | pieceBB[B_QUEEN];
	occupiedBB = whiteBB | blackBB;
}

void Board::InitKnightMoves()
{
	for (int sq = A1; sq < H8; ++sq)
	{
		BitBoard knight = 1ULL << sq;
		BitBoard moves = 0ULL;

		// 2 up, 1 left
		if (knight & notAFile) moves |= knight << 15;
		// 2 up, 1 right
		if (knight & notHFile) moves |= knight << 17;
		// 2 left, 1 up
		if (knight & notABFile) moves |= knight << 6;
		// 2 left, 1 down
		if (knight & notABFile) moves |= knight >> 10;
		// 2 down, 1 left
		if (knight & notAFile) moves |= knight >> 17;
		// 2 down, 1 right
		if (knight & notHFile) moves |= knight >> 15;
		// 2 right, 1 up
		if (knight & notGHFile) moves |= knight << 10;
		// 2 right, 1 down
		if (knight & notGHFile) moves |= knight >> 6;

		knightMoves[sq] = moves;
	}
}

int Board::GetPieceAtIdx(const int8_t idx)
{
	for (int pc = W_PAWN; pc <= B_KING; ++pc)
	{
		if ((pieceBB[pc] >> idx) & 1ULL)
		{
			return pc;
		}
	}
	return -1;
}

BitBoard Board::GetActiveMoves(const int8_t idx)
{
	int pc = GetPieceAtIdx(idx);
	if (pc == -1)
		return 0ULL;
	switch (pc)
	{
		case B_KNIGHT:
			return knightMoves[idx] & ~ blackBB;
		case W_KNIGHT:
			return knightMoves[idx] & ~ whiteBB;
	}
}

void Board::Move(const int from, const int to)
{
	int pc = GetPieceAtIdx(from);
	int pc2 = GetPieceAtIdx(to);
	BitBoard toBB = 1ULL << to;
	BitBoard fromBB = 1ULL << from;
	BitBoard fromToBB = toBB | fromBB;
	pieceBB[pc] ^= fromToBB;
	if (IsWhite(pc))
		whiteBB ^= fromToBB;
	else
		blackBB ^= fromToBB;
	if (pc2 != -1 and not (IsWhite(pc) and IsWhite(pc2)))
	{
		pieceBB[pc2] ^= toBB;
		if (IsWhite(pc2))
			whiteBB ^= toBB;
		else
			blackBB ^= toBB;
		occupiedBB ^= fromBB;
	}
	else
		occupiedBB ^= fromToBB;
}

const bool Board::IsWhite(const int pc) const
{
	return pc < 6;
}

const bool Board::isWhiteAtIdx(const int8_t idx) const
{
	return (whiteBB >> idx) & 1ULL;
}

const bool Board::isOccupiedAtIdx(const int8_t idx) const
{
	return (occupiedBB >> idx) & 1ULL;
}

Board::Board()
{
	InitBoard();
	InitKnightMoves();
}
