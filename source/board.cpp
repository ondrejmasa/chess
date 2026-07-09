#include "board.h"

void Board::InitRayFromTo()
{
	for (uint8_t i = A1; i <= H8; ++i)
	{
		for (uint8_t j = A1; j <= H8; ++j)	
		{
			BitBoard& bb = RayFromTo[i][j];
			bb = 0ULL;	

			uint8_t start; uint8_t stop;
			if (i < j)
			{
				start = i;
				stop = j;
			}
			else
			{
				start = j;
				stop = i;
			}
			uint8_t r1 = start / 8;
			uint8_t c1 = start % 8;
			uint8_t r2 = stop / 8;
			uint8_t c2 = stop % 8;	

			uint8_t step;
			if (r1 == r2) // same row
			{
				step = 1;
			}
			else if (c1 == c2) // same col
			{
				step = 8;				
			}
			else if (std::abs(r1-r2) == std::abs(c1-c2)) // same diag
			{
				step = c1 < c2 ? 9 : 7;
			}
			else continue; // none above

			while (start != stop)
			{
				bb |= 1ULL << start;
				start += step;
			}
			bb |= 1ULL << stop;	
		}
	}
}

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

	WhiteAttacks = GetAllWhiteAttacks();
	BlackAttacks = GetAllBlackAttacks();
}

void Board::InitKnightMoves()
{
	for (int sq = A1; sq <= H8; ++sq)
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
	for (int sq = A1; sq <= H8; ++sq)
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

BitBoard Board::GetRookMoves(const uint8_t idx) const
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

BitBoard Board::GetBishopMoves(const uint8_t idx) const
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

BitBoard Board::GetRookMask(const uint8_t idx) const
{
	BitBoard mask = 0ULL;
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
			mask |= bit;
			nr += dr[i];
			nc += dc[i];
		}
	}
	return mask;
}

BitBoard Board::GetBishopMask(const uint8_t idx) const
{
	BitBoard mask = 0ULL;
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
			mask |= bit;
			nr += dr[i];
			nc += dc[i];
		}
	}
	return mask;
}

BitBoard Board::GetPawnMoves(const uint8_t idx) const
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

BitBoard Board::GetPawnAttacks(const uint8_t idx) const
{
	const BitBoard pos = 1ULL << idx;
	BitBoard moves = 0ULL;
	SquareColor clr = GetColorAtIdx(idx);
	if (clr == WHITE)
	{
		// jump
		if (((pos << 7) & NotHFile)) moves |= (pos << 7);
		if (((pos << 9) & NotAFile)) moves |= (pos << 9);
	}
	else
	{
		if (((pos >> 7) & NotAFile)) moves |= (pos >> 7);
		if (((pos >> 9) & NotHFile)) moves |= (pos >> 9);
	}
	return moves;
}

BitBoard Board::GetAllWhiteAttacks() 
{
	BitBoard moves {0ULL};
	BitBoard bb = WhiteBB; 
	OccupiedBB ^= PieceBB[B_KING];
	while (bb > 0) {
		const int idx = std::countr_zero(bb);
		moves |= GetActiveAttacks(idx);
		bb &= (bb - 1);
	}
	OccupiedBB ^= PieceBB[B_KING];
	return moves;
}

BitBoard Board::GetAllBlackAttacks()
{
	BitBoard moves {0ULL};
	BitBoard bb = BlackBB; 
	OccupiedBB ^= PieceBB[W_KING];
	while (bb > 0) {
		const int idx = std::countr_zero(bb);
		moves |= GetActiveAttacks(idx);
		bb &= (bb - 1);
	}
	OccupiedBB ^= PieceBB[W_KING];
	return moves;
}

BitBoard Board::GetWhiteCheckers() const
{
	BitBoard checkers {0ULL};
	BitBoard bb = WhiteBB; 
	while (bb > 0) {
		const int idx = std::countr_zero(bb);
		const BitBoard moves = GetActiveAttacks(idx);
		if (moves & PieceBB[B_KING])
			checkers |= (1ULL << idx);
		bb &= (bb - 1);
	}
	return checkers;
}

BitBoard Board::GetBlackCheckers() const
{
	BitBoard checkers {0ULL};
	BitBoard bb = BlackBB; 
	while (bb > 0) {
		const int idx = std::countr_zero(bb);
		const BitBoard moves = GetActiveAttacks(idx);
		if (moves & PieceBB[W_KING])
			checkers |= (1ULL << idx);
		bb &= (bb - 1);
	}
	return checkers;
}

BitBoard Board::GetActiveAttacks(const uint8_t idx) const
{
	const int pc = GetPieceAtIdx(idx);
	if (pc == -1)
		return 0ULL;

	switch (pc)
	{
		case B_KNIGHT:
		case W_KNIGHT:
			return KnightMoves[idx];
		case B_KING:
		case W_KING:
			return (KingMoves[idx]);
		case W_ROOK:
		case B_ROOK:
			return GetRookMoves(idx);
		case W_BISHOP:
		case B_BISHOP:
			return GetBishopMoves(idx);
		case W_QUEEN:
		case B_QUEEN:
			return (GetRookMoves(idx) | GetBishopMoves(idx));
		case B_PAWN:
		case W_PAWN:
			return GetPawnAttacks(idx);
	}
	return 0ULL;
}

BitBoard Board::GetWhitePins() const
{
	BitBoard pins = 0ULL;
	const int kingIdx = std::countr_zero(PieceBB[W_KING]);
	BitBoard orthoSliders = (PieceBB[B_QUEEN] | PieceBB[B_ROOK]) & GetRookMask(kingIdx);
	while (orthoSliders != 0ULL)
	{
		int sliderIdx = std::countr_zero(orthoSliders);
		BitBoard ray = RayFromTo[kingIdx][sliderIdx];
		BitBoard pcOnRay = ray & OccupiedBB & ~PieceBB[W_KING] & ~(1ULL << sliderIdx);
		if (std::popcount(pcOnRay) == 1)
		{
			if (pcOnRay & WhiteBB)
			{
				pins |= pcOnRay;
			}
		}
		orthoSliders &= orthoSliders-1;
	}
	BitBoard diagSliders = (PieceBB[B_QUEEN] | PieceBB[B_BISHOP]) & GetBishopMask(kingIdx);
	while (diagSliders != 0ULL)
	{
		int sliderIdx = std::countr_zero(diagSliders);
		BitBoard ray = RayFromTo[kingIdx][sliderIdx];
		BitBoard pcOnRay = ray & OccupiedBB & ~PieceBB[W_KING] & ~(1ULL << sliderIdx);
		if (std::popcount(pcOnRay) == 1)
		{
			if (pcOnRay & WhiteBB)
			{
				pins |= pcOnRay;
			}
		}
		diagSliders &= diagSliders-1;
	}
	return pins;
}

BitBoard Board::GetBlackPins() const
{
	BitBoard pins = 0ULL;
	const int kingIdx = std::countr_zero(PieceBB[B_KING]);
	BitBoard orthoSliders = (PieceBB[W_QUEEN] | PieceBB[W_ROOK]) & GetRookMask(kingIdx);
	while (orthoSliders != 0ULL)
	{
		int sliderIdx = std::countr_zero(orthoSliders);
		BitBoard ray = RayFromTo[kingIdx][sliderIdx];
		BitBoard pcOnRay = ray & OccupiedBB & ~PieceBB[B_KING] & ~(1ULL << sliderIdx);
		if (std::popcount(pcOnRay) == 1)
		{
			if (pcOnRay & BlackBB)
			{
				pins |= pcOnRay;
			}
		}
		orthoSliders &= orthoSliders-1;
	}
	BitBoard diagSliders = (PieceBB[W_QUEEN] | PieceBB[W_BISHOP]) & GetBishopMask(kingIdx);
	while (diagSliders != 0ULL)
	{
		int sliderIdx = std::countr_zero(diagSliders);
		BitBoard ray = RayFromTo[kingIdx][sliderIdx];
		BitBoard pcOnRay = ray & OccupiedBB & ~PieceBB[B_KING] & ~(1ULL << sliderIdx);
		if (std::popcount(pcOnRay) == 1)
		{
			if (pcOnRay & BlackBB)
			{
				pins |= pcOnRay;
			}
		}
		diagSliders &= diagSliders-1;
	}
	return pins;
}

BitBoard Board::GetAllWhiteMoves() const
{
	BitBoard moves {0ULL};
	BitBoard bb = WhiteBB; 
	while (bb > 0) {
		const int idx = std::countr_zero(bb);
		moves |= GetActiveMoves(idx);
		bb &= (bb - 1);
	}
	return moves;
}

BitBoard Board::GetAllBlackMoves() const
{
	BitBoard moves {0ULL};
	BitBoard bb = BlackBB; 
	while (bb > 0) {
		const int idx = std::countr_zero(bb);
		moves |= GetActiveMoves(idx);
		bb &= (bb - 1);
	}
	return moves;
}

BitBoard Board::GetWhitePinRay(const uint8_t idx) const
{
	const int kingIdx = std::countr_zero(PieceBB[W_KING]);
	BitBoard pcBB = 1ULL << idx;
	BitBoard orthoSliders = (PieceBB[B_QUEEN] | PieceBB[B_ROOK]) & GetRookMask(kingIdx);
	while (orthoSliders != 0ULL)
	{
		int sliderIdx = std::countr_zero(orthoSliders);
		BitBoard ray = RayFromTo[kingIdx][sliderIdx];
		if (ray & pcBB)
			return ray;
		orthoSliders &= orthoSliders-1;
	}
	BitBoard diagSliders = (PieceBB[B_QUEEN] | PieceBB[B_BISHOP]) & GetBishopMask(kingIdx);
	while (diagSliders != 0ULL)
	{
		int sliderIdx = std::countr_zero(diagSliders);
		BitBoard ray = RayFromTo[kingIdx][sliderIdx];
		if (ray & pcBB)
			return ray;
		diagSliders &= diagSliders-1;
	}
	return 0ULL;
}

BitBoard Board::GetBlackPinRay(const uint8_t idx) const
{
	const int kingIdx = std::countr_zero(PieceBB[B_KING]);
	BitBoard pcBB = 1ULL << idx;
	BitBoard orthoSliders = (PieceBB[W_QUEEN] | PieceBB[W_ROOK]) & GetRookMask(kingIdx);
	while (orthoSliders != 0ULL)
	{
		int sliderIdx = std::countr_zero(orthoSliders);
		BitBoard ray = RayFromTo[kingIdx][sliderIdx];
		if (ray & pcBB)
			return ray;
		orthoSliders &= orthoSliders-1;
	}
	BitBoard diagSliders = (PieceBB[W_QUEEN] | PieceBB[W_BISHOP]) & GetBishopMask(kingIdx);
	while (diagSliders != 0ULL)
	{
		int sliderIdx = std::countr_zero(diagSliders);
		BitBoard ray = RayFromTo[kingIdx][sliderIdx];
		if (ray & pcBB)
			return ray;
		diagSliders &= diagSliders-1;
	}
	return 0ULL;
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

int Board::GetPieceAtIdx(const uint8_t idx) const
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

BitBoard Board::GetActiveMoves(const uint8_t idx) const
{
	const int pc = GetPieceAtIdx(idx);
	if (pc == -1)
		return 0ULL;
	
	BitBoard PinMask {-1ULL};
	if ((1ULL << idx) & WhitePins)
		PinMask = GetWhitePinRay(idx);
	if ((1ULL << idx) & BlackPins)
		PinMask = GetBlackPinRay(idx);
	
	BitBoard safeMask {-1ULL};
	const int numWhiteCheckers = std::popcount(WhiteCheckers);
	const int numBlackCheckers = std::popcount(BlackCheckers);
	if (numWhiteCheckers == 1)
	{
		int checkerIdx = std::countr_zero(WhiteCheckers);
		int kingIdx = std::countr_zero(PieceBB[B_KING]);
		int checkerPc = GetPieceAtIdx(checkerIdx);
		if (checkerPc == W_BISHOP or checkerPc == W_QUEEN or checkerPc == W_ROOK)
			safeMask = (GetActiveAttacks(checkerIdx) | WhiteCheckers) & RayFromTo[checkerIdx][kingIdx];
		else
			safeMask = WhiteCheckers;
	}
	else if (numWhiteCheckers > 1)
		safeMask = 0ULL;

	if (numBlackCheckers == 1)
	{
		int checkerIdx = std::countr_zero(BlackCheckers);
		int kingIdx = std::countr_zero(PieceBB[W_KING]);
		int checkerPc = GetPieceAtIdx(checkerIdx);
		if (checkerPc == B_BISHOP or checkerPc == B_QUEEN or checkerPc == B_ROOK)
			safeMask = (GetActiveAttacks(checkerIdx) | BlackCheckers) & RayFromTo[checkerIdx][kingIdx];
		else
			safeMask = BlackCheckers;
	}
	else if (numBlackCheckers > 1)
		safeMask = 0ULL;	

	BitBoard castle {0ULL};
	switch (pc)
	{
		case B_KNIGHT:
			return KnightMoves[idx] & ~ BlackBB & safeMask & PinMask;
		case W_KNIGHT:
			return KnightMoves[idx] & ~ WhiteBB & safeMask & PinMask;
		case B_KING:
			if (numWhiteCheckers == 0)
			{
				castle = CanBlackCastleLeft() ? (1ULL << C8) : 0ULL;
				castle |= CanBlackCastleRight() ? (1ULL << G8) : 0ULL;
			}
			return (KingMoves[idx] & ~BlackBB & ~ WhiteAttacks) | castle;
		case W_KING:
			if (numBlackCheckers == 0)
			{
				castle = CanWhiteCastleLeft() ? (1ULL << C1) : 0ULL;
				castle |= CanWhiteCastleRight() ? (1ULL << G1) : 0ULL;		
			}
			return (KingMoves[idx] & ~WhiteBB & ~ BlackAttacks) | castle;
		case W_ROOK:
			return GetRookMoves(idx) & ~WhiteBB & safeMask & PinMask;
		case B_ROOK:
			return GetRookMoves(idx) & ~BlackBB & safeMask & PinMask;
		case W_BISHOP:
			return GetBishopMoves(idx) & ~WhiteBB & safeMask & PinMask;
		case B_BISHOP:
			return GetBishopMoves(idx) & ~BlackBB & safeMask & PinMask;
		case W_QUEEN:
			return (GetRookMoves(idx) | GetBishopMoves(idx)) & ~WhiteBB & safeMask & PinMask;
		case B_QUEEN:
			return (GetRookMoves(idx) | GetBishopMoves(idx)) & ~BlackBB & safeMask & PinMask;
		case B_PAWN:
			return GetPawnMoves(idx) & safeMask & PinMask;
		case W_PAWN:
			return GetPawnMoves(idx) & safeMask & PinMask;
	}
	return 0ULL;
}

void Board::Move(const uint8_t from, const uint8_t to)
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
	LastMove = {from, to, pcCol == WHITE, pc};
}

GameState Board::UpdateAfterMove()
{
	int pc = LastMove.Pc;
	uint8_t from = LastMove.From;
	uint8_t to = LastMove.To;
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

	WhiteAttacks = GetAllWhiteAttacks();
	BlackAttacks = GetAllBlackAttacks();

	WhiteCheckers = GetWhiteCheckers();
	BlackCheckers = GetBlackCheckers();

	WhitePins = GetWhitePins();
	BlackPins = GetBlackPins();

	BitBoard whiteMoves = GetAllWhiteMoves();
	BitBoard blackMoves = GetAllBlackMoves();

	if (!whiteMoves)
	{
		if (BlackCheckers == 0)
		{
			std::cout << "Stalemate! \n";
			return STALEMATE;
		}
		else
		{
			std::cout << "Black win! \n";
			return B_WIN;
		}
	}
	if (!blackMoves)
	{
		if (WhiteCheckers == 0)
		{
			std::cout << "Stalemate! \n";
			return STALEMATE;
		}
		else
		{
			std::cout << "White win! \n";
			return W_WIN;
		}
	}
	if (pc == W_PAWN and to >= A8)
	{
		std::cout << "W_PROMOTE \n";
		return W_PROMOTE;
	}
	else if (pc == B_PAWN and to <= H1)
	{
		std::cout << "B_PROMOTE \n";
		return B_PROMOTE;
	}
	return GAME;
}

SquareColor Board::GetColor(const int pc) const
{
	if (pc == -1)
		return EMPTY;
	if (pc < 6)
		return WHITE;
	return BLACK;
}

SquareColor Board::GetColorAtIdx(const uint8_t idx) const
{
	if (not IsOccupiedAtIdx(idx))
		return EMPTY;
	if ((WhiteBB >> idx) & 1ULL)
		return WHITE;
	return BLACK;
}

bool Board::IsOccupiedAtIdx(const uint8_t idx) const
{
	return (OccupiedBB >> idx) & 1ULL;
}

const MoveData& Board::GetLastMove() const
{
   return LastMove;
}

void Board::PromotePawn(const PieceTypeAndColor toPc)
{
	BitBoard pcBB = 1ULL << LastMove.To;
	PieceTypeAndColor pawn = LastMove.IsWhite ? W_PAWN : B_PAWN;
	PieceBB[pawn] ^= pcBB;
	PieceBB[toPc] ^= pcBB;
}

void Board::Restart()
{
    WhiteEnPassant = 0ULL;
    BlackEnPassant = 0ULL;
    WhiteCheckers = 0ULL;
    BlackCheckers = 0ULL;
    WhiteAttacks = 0ULL;
    BlackAttacks = 0ULL;
    WhitePins = 0ULL;
    BlackPins = 0ULL;
    WhiteCastle.Restart();
    BlackCastle.Restart();
	InitBoard();
}

std::vector<MoveData> Board::GetAllWhiteMovesFromTo() const
{
    std::vector<MoveData> moves;
	BitBoard bb = WhiteBB; 
	while (bb > 0) {
		uint8_t from = std::countr_zero(bb);
		BitBoard am = GetActiveMoves(from);
		while (am > 0) {
			MoveData move;
			move.From = from;
			move.To = std::countr_zero(am);
			am &= (am - 1);
		}
		bb &= (bb - 1);
	}
	return moves;	
}

std::vector<MoveData> Board::GetAllBlackMovesFromTo() const
{
    std::vector<MoveData> moves;
	BitBoard bb = BlackBB; 
	while (bb > 0) {
		uint8_t from = std::countr_zero(bb);
		BitBoard am = GetActiveMoves(from);
		while (am > 0) {
			MoveData move;
			move.From = from;
			move.To = std::countr_zero(am);
			moves.push_back(move);
			am &= (am - 1);
		}
		bb &= (bb - 1);
	}
	return moves;	
}

Board::Board()
{
	InitBoard();
	InitKnightMoves();
	InitKingMoves();
	InitRayFromTo();
}
