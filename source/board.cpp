#include "board.h"

void Board::InitBoard()
{
	occupiedBB = 0ULL;
	pieceBB.fill(0ULL);

	// for (int sq = A2; sq <= H2; ++sq) pieceBB[W_PAWN] |= (1ULL << sq);
	// for (int sq = A7; sq <= H7; ++sq) pieceBB[B_PAWN] |= (1ULL << sq);

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

	whiteBB = pieceBB[W_PAWN] | pieceBB[W_ROOK] | pieceBB[W_KNIGHT] | pieceBB[W_BISHOP] | pieceBB[W_QUEEN] | pieceBB[W_KING];
	blackBB = pieceBB[B_PAWN] | pieceBB[B_ROOK] | pieceBB[B_KNIGHT] | pieceBB[B_BISHOP] | pieceBB[B_QUEEN] | pieceBB[B_KING];
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

void Board::InitKingMoves()
{
	for (int sq = A1; sq < H8; ++sq)
	{
		BitBoard king = 1ULL << sq;
		BitBoard moves = 0ULL;

		// 1 up, 1 left
		if (king & notAFile) moves |= king << 7;
		// 1 up
		moves |= king << 8;
		// 1 up, 1 right
		if (king & notHFile) moves |= king << 9;
		// 1 right
		if (king & notHFile) moves |= king << 1;
		// 1 down, 1 right
		if (king & notHFile) moves |= king >> 7;
		// 1 down
		moves |= king >> 8;
		// 1 down, 1 left
		if (king & notAFile) moves |= king >> 9;
		// 1 left
		if (king & notAFile) moves |= king >> 1;

		kingMoves[sq] = moves;
	}
}

const BitBoard Board::getRookMoves(const int8_t idx) const
{
	BitBoard moves = 0ULL;
	auto r = idx / 8;
	auto c = idx % 8;

	int dr[4] = { 1, -1, 0, 0 };
	int dc[4] = { 0, 0, 1, -1 };

	for (auto i = 0; i < 4; ++i)
	{
		int nr = r + dr[i];
		int nc = c + dc[i];
		while (nr >= 0 and nr < 8 and nc >= 0 and nc < 8)
		{
			BitBoard bit = 1ULL << (8 * nr + nc);
			moves |= bit;
			if (occupiedBB & bit) break;
			nr += dr[i];
			nc += dc[i];
		}
	}
	return moves;
}

const BitBoard Board::getBishopMoves(const int8_t idx) const
{
	BitBoard moves = 0ULL;
	auto r = idx / 8;
	auto c = idx % 8;

	int dr[4] = { 1, -1, 1, -1 };
	int dc[4] = { 1, 1, -1, -1 };

	for (auto i = 0; i < 4; ++i)
	{
		int nr = r + dr[i];
		int nc = c + dc[i];
		while (nr >= 0 and nr < 8 and nc >= 0 and nc < 8)
		{
			BitBoard bit = 1ULL << (8 * nr + nc);
			moves |= bit;
			if (occupiedBB & bit) break;
			nr += dr[i];
			nc += dc[i];
		}
	}
	return moves;
}

const BitBoard Board::getPawnMoves(const int8_t idx) const
{
	const BitBoard pos = 1ULL << idx;
	BitBoard moves = 0ULL;
	auto r = idx / 8;
	SquareColor clr = GetColorAtIdx(idx);
	if (clr == WHITE)
	{
		if (not ((pos << 8) & occupiedBB))
		{
			moves |= (pos << 8);
			if (r == 1 and not ((pos << 16) & occupiedBB)) moves |= (pos << 16);
		}
		if (((pos << 7) & notHFile) & blackBB) moves |= (pos << 7);
		if (((pos << 9) & notAFile) & blackBB) moves |= (pos << 9);
	}
	else
	{
		if (not ((pos >> 8) & occupiedBB))
		{
			moves |= (pos >> 8);
			if (r == 6 and not ((pos >> 16) & occupiedBB)) moves |= (pos >> 16);
		}
		if (((pos >> 7) & notAFile) & whiteBB) moves |= (pos >> 7);
		if (((pos >> 9) & notHFile) & whiteBB) moves |= (pos >> 9);
	}
	return moves;
}

const bool Board::canWhiteCastleRight() const
{
    if (whiteCastle.KingMoved or whiteCastle.RightRookMoved) return false;
	if (((occupiedBB >> F1) & 1ULL) or ((occupiedBB >> G1) & 1ULL)) return false;
	return true;
}

const bool Board::canWhiteCastleLeft() const
{
    if (whiteCastle.KingMoved or whiteCastle.LeftRookMoved) return false;
	if (((occupiedBB >> B1) & 1ULL) or ((occupiedBB >> C1) & 1ULL) or ((occupiedBB >> D1) & 1ULL)) return false;
	return true;
}

const bool Board::canBlackCastleRight() const
{
    if (blackCastle.KingMoved or blackCastle.RightRookMoved) return false;
	if (((occupiedBB >> F8) & 1ULL) or ((occupiedBB >> G8) & 1ULL)) return false;
	return true;
}

const bool Board::canBlackCastleLeft() const
{
    if (blackCastle.KingMoved or blackCastle.LeftRookMoved) return false;
	if (((occupiedBB >> B8) & 1ULL) or ((occupiedBB >> C8) & 1ULL) or ((occupiedBB >> D8) & 1ULL)) return false;
	return true;
}

const int Board::GetPieceAtIdx(const int8_t idx)
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

const BitBoard Board::GetActiveMoves(const int8_t idx)
{
	const int pc = GetPieceAtIdx(idx);
	if (pc == -1)
		return 0ULL;
	BitBoard castle {0ULL};
	switch (pc)
	{
		case B_KNIGHT:
			return knightMoves[idx] & ~ blackBB;
		case W_KNIGHT:
			return knightMoves[idx] & ~ whiteBB;
		case B_KING:
			castle = canBlackCastleLeft() ? (1ULL << C8) : 0ULL;
			castle |= canBlackCastleRight() ? (1ULL << G8) : 0ULL;
			return (kingMoves[idx] & ~blackBB) | castle;
		case W_KING:
			castle = canWhiteCastleLeft() ? (1ULL << C1) : 0ULL;
			castle |= canWhiteCastleRight() ? (1ULL << G1) : 0ULL;		
			return (kingMoves[idx] & ~whiteBB) | castle;
		case W_ROOK:
			return getRookMoves(idx) & ~whiteBB;
		case B_ROOK:
			return getRookMoves(idx) & ~blackBB;
		case W_BISHOP:
			return getBishopMoves(idx) & ~whiteBB;
		case B_BISHOP:
			return getBishopMoves(idx) & ~blackBB;
		case W_QUEEN:
			return (getRookMoves(idx) | getBishopMoves(idx)) & ~whiteBB;
		case B_QUEEN:
			return (getRookMoves(idx) | getBishopMoves(idx)) & ~blackBB;
		case B_PAWN:
		case W_PAWN:
			return getPawnMoves(idx);
	}
	return 0ULL;
}

void Board::Move(const int from, const int to)
{
	const int pc = GetPieceAtIdx(from);
	const int pc2 = GetPieceAtIdx(to);
	const BitBoard toBB = 1ULL << to;
	const BitBoard fromBB = 1ULL << from;
	const BitBoard fromToBB = fromBB ^ toBB;
	// move pc
	pieceBB[pc] ^= fromToBB;
	const SquareColor pcCol = GetColor(pc);
	if (pcCol == WHITE)
		whiteBB ^= fromToBB;
	else
		blackBB ^= fromToBB;
	// if "to" square is occupied by enemy, eliminate them
	const SquareColor pc2Col = GetColor(pc2);
	if (pc2Col != EMPTY and pcCol != pc2Col)
	{
		pieceBB[pc2] ^= toBB;
		if (pc2Col == WHITE)
			whiteBB ^= toBB;
		else
			blackBB ^= toBB;
	}

	// check for castle moves
	if (pc == W_KING)
	{
		BitBoard rm {0ULL};
		if (to == G1)
		{
			rm = (1ULL << H1) | (1ULL << F1);
			pieceBB[W_ROOK] ^= rm;
		}
		else if (to == C1)
		{
			rm = (1ULL << A1) | (1ULL << D1);
			pieceBB[W_ROOK] ^= rm;
		}
		whiteBB ^= rm;
	}
	else if (pc == B_KING)
	{
		BitBoard rm {0ULL};
		if (to == G8)
		{
			rm = (1ULL << H8) | (1ULL << F8);
			pieceBB[B_ROOK] ^= rm;
		}
		else if (to == C8)
		{
			rm = (1ULL << A8) | (1ULL << D8);
			pieceBB[B_ROOK] ^= rm;
		}
		blackBB ^= rm;		
	}

	occupiedBB = whiteBB | blackBB;

	// update castle info
	switch(pc){
		case W_KING: whiteCastle.KingMoved = true; break;
		case B_KING: blackCastle.KingMoved = true; break;
		case W_ROOK:
			from == A1 ? whiteCastle.LeftRookMoved = true : whiteCastle.RightRookMoved = true; break;
		case B_ROOK:
			from == A8 ? blackCastle.LeftRookMoved = true : blackCastle.RightRookMoved = true; break;
	}
}

const SquareColor Board::GetColor(const int pc) const
{
	if (pc == -1)
		return EMPTY;
	if (pc < 6)
		return WHITE;
	return BLACK;
}

const SquareColor Board::GetColorAtIdx(const int8_t idx) const
{
	if (not isOccupiedAtIdx(idx))
		return EMPTY;
	if ((whiteBB >> idx) & 1ULL)
		return WHITE;
	return BLACK;
}

const bool Board::isOccupiedAtIdx(const int8_t idx) const
{
	return (occupiedBB >> idx) & 1ULL;
}

Board::Board()
{
	InitBoard();
	InitKnightMoves();
	InitKingMoves();
}
