#include "board.h"

void Board::InitBoard()
{
	OccupiedBB = 0ULL;
	PieceBB.fill(0ULL);

	for (int sq = A2; sq <= H2; ++sq) PieceBB[W_PAWN] |= (1ULL << sq);
	for (int sq = A7; sq <= H7; ++sq) PieceBB[B_PAWN] |= (1ULL << sq);

	PieceBB[W_ROOK] = (1ULL << A1) | (1ULL << H1);
	PieceBB[B_ROOK] = (1ULL << A8) | (1ULL << H8);
	PieceBB[W_KNIGHT] = (1ULL << B1) | (1ULL << G1);
	PieceBB[B_KNIGHT] = (1ULL << B8) | (1ULL << G8);
	PieceBB[W_BISHOP] = (1ULL << C1) | (1ULL << F1);
	PieceBB[B_BISHOP] = (1ULL << C8) | (1ULL << F8);
	PieceBB[W_QUEEN] = (1ULL << D1);
	PieceBB[B_QUEEN] = (1ULL << D8);
	PieceBB[W_KING] = (1ULL << E1);
	PieceBB[B_KING] = (1ULL << E8);

	WhiteBB = PieceBB[W_PAWN] | PieceBB[W_ROOK] | PieceBB[W_KNIGHT] | PieceBB[W_BISHOP] | PieceBB[W_QUEEN] | PieceBB[W_KING];
	BlackBB = PieceBB[B_PAWN] | PieceBB[B_ROOK] | PieceBB[B_KNIGHT] | PieceBB[B_BISHOP] | PieceBB[B_QUEEN] | PieceBB[B_KING];
	OccupiedBB = WhiteBB | BlackBB;
}

void Board::InitKnightMoves()
{
	for (int sq = A1; sq < H8; ++sq)
	{
		BitBoard knight = 1ULL << sq;
		BitBoard moves = 0ULL;

		// 2 up, 1 left
		if (knight & NotAFile) moves |= knight << 15;
		// 2 up, 1 right
		if (knight & NotHFile) moves |= knight << 17;
		// 2 left, 1 up
		if (knight & NotABFile) moves |= knight << 6;
		// 2 left, 1 down
		if (knight & NotABFile) moves |= knight >> 10;
		// 2 down, 1 left
		if (knight & NotAFile) moves |= knight >> 17;
		// 2 down, 1 right
		if (knight & NotHFile) moves |= knight >> 15;
		// 2 right, 1 up
		if (knight & NotGHFile) moves |= knight << 10;
		// 2 right, 1 down
		if (knight & NotGHFile) moves |= knight >> 6;

		KnightMoves[sq] = moves;
	}
}

void Board::InitKingMoves()
{
	for (int sq = A1; sq < H8; ++sq)
	{
		BitBoard king = 1ULL << sq;
		BitBoard moves = 0ULL;

		// 1 up, 1 left
		if (king & NotAFile) moves |= king << 7;
		// 1 up
		moves |= king << 8;
		// 1 up, 1 right
		if (king & NotHFile) moves |= king << 9;
		// 1 right
		if (king & NotHFile) moves |= king << 1;
		// 1 down, 1 right
		if (king & NotHFile) moves |= king >> 7;
		// 1 down
		moves |= king >> 8;
		// 1 down, 1 left
		if (king & NotAFile) moves |= king >> 9;
		// 1 left
		if (king & NotAFile) moves |= king >> 1;

		KingMoves[sq] = moves;
	}
}

BitBoard Board::GetRookMoves(const int8_t idx) const
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
			if (OccupiedBB & bit) break;
			nr += dr[i];
			nc += dc[i];
		}
	}
	return moves;
}

BitBoard Board::GetBishopMoves(const int8_t idx) const
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
			if (OccupiedBB & bit) break;
			nr += dr[i];
			nc += dc[i];
		}
	}
	return moves;
}

BitBoard Board::GetPawnMoves(const int8_t idx) const
{
	const BitBoard pos = 1ULL << idx;
	BitBoard moves = 0ULL;
	auto r = idx / 8;
	SquareColor clr = GetColorAtIdx(idx);
	if (clr == WHITE)
	{
		if (not ((pos << 8) & OccupiedBB))
		{
			moves |= (pos << 8);
			if (r == 1 and not ((pos << 16) & OccupiedBB)) moves |= (pos << 16);
		}
		// jump
		if (((pos << 7) & NotHFile) & BlackBB) moves |= (pos << 7);
		if (((pos << 9) & NotAFile) & BlackBB) moves |= (pos << 9);
		// en passant
		if (((pos << 7) & NotHFile) & BlackEnPassant) moves |= (pos << 7);
		if (((pos << 9) & NotAFile) & BlackEnPassant) moves |= (pos << 9);
	}
	else
	{
		if (not ((pos >> 8) & OccupiedBB))
		{
			moves |= (pos >> 8);
			if (r == 6 and not ((pos >> 16) & OccupiedBB)) moves |= (pos >> 16);
		}
		// jump
		if (((pos >> 7) & NotAFile) & WhiteBB) moves |= (pos >> 7);
		if (((pos >> 9) & NotHFile) & WhiteBB) moves |= (pos >> 9);
		// en passant
		if (((pos >> 7) & NotAFile) & WhiteEnPassant) moves |= (pos >> 7);
		if (((pos >> 9) & NotHFile) & WhiteEnPassant) moves |= (pos >> 9);
	}
	return moves;
}

BitBoard Board::GetAllWhiteMoves() const
{
    BitBoard moves {0ULL};
	for (uint8_t i = 0; i < 64; i++)
	{
		if ((WhiteBB >> i) & 1ULL)
			moves |= GetActiveMoves(i);	
	}
	return moves;
}

BitBoard Board::GetAllBlackMoves() const
{
    BitBoard moves {0ULL};
	for (uint8_t i = 0; i < 64; i++)
	{
		if ((BlackBB >> i) & 1ULL)
			moves |= GetActiveMoves(i);	
	}
	return moves;
}

bool Board::CanWhiteCastleRight() const
{
    if (WhiteCastle.KingMoved or WhiteCastle.RightRookMoved) return false;
	if (((OccupiedBB >> F1) & 1ULL) or ((OccupiedBB >> G1) & 1ULL)) return false;
	return true;
}

bool Board::CanWhiteCastleLeft() const
{
    if (WhiteCastle.KingMoved or WhiteCastle.LeftRookMoved) return false;
	if (((OccupiedBB >> B1) & 1ULL) or ((OccupiedBB >> C1) & 1ULL) or ((OccupiedBB >> D1) & 1ULL)) return false;
	return true;
}

bool Board::CanBlackCastleRight() const
{
    if (BlackCastle.KingMoved or BlackCastle.RightRookMoved) return false;
	if (((OccupiedBB >> F8) & 1ULL) or ((OccupiedBB >> G8) & 1ULL)) return false;
	return true;
}

bool Board::CanBlackCastleLeft() const
{
    if (BlackCastle.KingMoved or BlackCastle.LeftRookMoved) return false;
	if (((OccupiedBB >> B8) & 1ULL) or ((OccupiedBB >> C8) & 1ULL) or ((OccupiedBB >> D8) & 1ULL)) return false;
	return true;
}

int Board::GetPieceAtIdx(const int8_t idx) const
{
	for (int pc = W_PAWN; pc <= B_KING; ++pc)
	{
		if ((PieceBB[pc] >> idx) & 1ULL)
		{
			return pc;
		}
	}
	return -1;
}

BitBoard Board::GetActiveMoves(const int8_t idx) const
{
	const int pc = GetPieceAtIdx(idx);
	if (pc == -1)
		return 0ULL;
	BitBoard castle {0ULL};
	switch (pc)
	{
		case B_KNIGHT:
			return KnightMoves[idx] & ~ BlackBB;
		case W_KNIGHT:
			return KnightMoves[idx] & ~ WhiteBB;
		case B_KING:
			castle = CanBlackCastleLeft() ? (1ULL << C8) : 0ULL;
			castle |= CanBlackCastleRight() ? (1ULL << G8) : 0ULL;
			return (KingMoves[idx] & ~BlackBB) | castle;
		case W_KING:
			castle = CanWhiteCastleLeft() ? (1ULL << C1) : 0ULL;
			castle |= CanWhiteCastleRight() ? (1ULL << G1) : 0ULL;		
			return (KingMoves[idx] & ~WhiteBB) | castle;
		case W_ROOK:
			return GetRookMoves(idx) & ~WhiteBB;
		case B_ROOK:
			return GetRookMoves(idx) & ~BlackBB;
		case W_BISHOP:
			return GetBishopMoves(idx) & ~WhiteBB;
		case B_BISHOP:
			return GetBishopMoves(idx) & ~BlackBB;
		case W_QUEEN:
			return (GetRookMoves(idx) | GetBishopMoves(idx)) & ~WhiteBB;
		case B_QUEEN:
			return (GetRookMoves(idx) | GetBishopMoves(idx)) & ~BlackBB;
		case B_PAWN:
		case W_PAWN:
			return GetPawnMoves(idx);
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
	PieceBB[pc] ^= fromToBB;
	const SquareColor pcCol = GetColor(pc);
	if (pcCol == WHITE)
		WhiteBB ^= fromToBB;
	else
		BlackBB ^= fromToBB;

	// if "to" square is occupied by enemy, eliminate them
	const SquareColor pc2Col = GetColor(pc2);
	if (pc2Col != EMPTY and pcCol != pc2Col)
	{
		PieceBB[pc2] ^= toBB;
		if (pc2Col == WHITE)
			WhiteBB ^= toBB;
		else
			BlackBB ^= toBB;
	}

	// check for castle moves
	if (pc == W_KING)
	{
		BitBoard rm {0ULL};
		if (to == G1)
		{
			rm = (1ULL << H1) | (1ULL << F1);
			PieceBB[W_ROOK] ^= rm;
		}
		else if (to == C1)
		{
			rm = (1ULL << A1) | (1ULL << D1);
			PieceBB[W_ROOK] ^= rm;
		}
		WhiteBB ^= rm;
	}
	else if (pc == B_KING)
	{
		BitBoard rm {0ULL};
		if (to == G8)
		{
			rm = (1ULL << H8) | (1ULL << F8);
			PieceBB[B_ROOK] ^= rm;
		}
		else if (to == C8)
		{
			rm = (1ULL << A8) | (1ULL << D8);
			PieceBB[B_ROOK] ^= rm;
		}
		BlackBB ^= rm;		
	}

	// check for en passant moves
	if (pc == W_PAWN)
	{
		if ((BlackEnPassant >> to) & 1ULL)
		{
			BitBoard rm = 1ULL << (to-8);
			PieceBB[B_PAWN] ^= rm;	
			BlackBB ^= rm;
		}
	}
	else if (pc == B_PAWN)
	{
		if ((WhiteEnPassant >> to) & 1ULL)
		{
			BitBoard rm = 1ULL << (to+8);
			PieceBB[W_PAWN] ^= rm;	
			WhiteBB ^= rm;
		}		
	}

	OccupiedBB = WhiteBB | BlackBB;

	// update castle info
	switch(pc){
		case W_KING: WhiteCastle.KingMoved = true; break;
		case B_KING: BlackCastle.KingMoved = true; break;
		case W_ROOK:
			from == A1 ? WhiteCastle.LeftRookMoved = true : WhiteCastle.RightRookMoved = true; break;
		case B_ROOK:
			from == A8 ? BlackCastle.LeftRookMoved = true : BlackCastle.RightRookMoved = true; break;
	}

	// update en passant info
	WhiteEnPassant = 0ULL;
	BlackEnPassant = 0ULL;
	if (std::abs(to - from) == 16)
	{
		if (pc == W_PAWN) WhiteEnPassant = (1ULL << (to-8));
		else if (pc == B_PAWN) BlackEnPassant = 1ULL << (to+8);
	}


	if (pcCol == WHITE)
	{
		const BitBoard whiteMoves = GetAllWhiteMoves();
		if (PieceBB[B_KING] & whiteMoves)
			std::cout << "White check \n";
	}
	else
	{
		const BitBoard blackMoves = GetAllBlackMoves();
		if (PieceBB[W_KING] & blackMoves)
			std::cout << "Black check \n";
	}
}

SquareColor Board::GetColor(const int pc) const
{
	if (pc == -1)
		return EMPTY;
	if (pc < 6)
		return WHITE;
	return BLACK;
}

SquareColor Board::GetColorAtIdx(const int8_t idx) const
{
	if (not IsOccupiedAtIdx(idx))
		return EMPTY;
	if ((WhiteBB >> idx) & 1ULL)
		return WHITE;
	return BLACK;
}

bool Board::IsOccupiedAtIdx(const int8_t idx) const
{
	return (OccupiedBB >> idx) & 1ULL;
}

Board::Board()
{
	InitBoard();
	InitKnightMoves();
	InitKingMoves();
}
