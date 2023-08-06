#pragma once

#include <vector>

#include "bitset64.h"
#include "cell.h"
#include <array>

namespace ReversiEngine {
    enum Player { First, Second };

    class Board {
    public:
        Board();

        void PossibleMoves(std::vector<Cell>& result) const;

        [[nodiscard]] std::vector<Cell> PossibleMoves() const;

        [[nodiscard]] char MySymbol() const;

        [[nodiscard]] Board MakeMove(const Cell& cell) const;

        [[nodiscard]] int32_t FinalEvaluation() const;

        [[nodiscard]] bool GameEnded() const;

        friend std::ostream& operator<<(std::ostream& os, const Board& board);

        void InitPrecalc() const;

    private:
        void PlacePiece(size_t position, Player player);

        void PlacePiece(const Cell& cell, Player player);

        static bool IsInBoundingBox(const Cell& cell);

        [[nodiscard]] const Bitset64& Same(Player player) const;

        [[nodiscard]] const Bitset64& Opposite(Player player) const;

        [[nodiscard]] const Bitset64& SameVertical(Player player) const;

        [[nodiscard]] const Bitset64& OppositeVertical(Player player) const;

        [[nodiscard]] const Bitset64& SameDiagonal1(Player player) const;

        [[nodiscard]] const Bitset64& OppositeDiagonal1(Player player) const;

        [[nodiscard]] const Bitset64& SameDiagonal2(Player player) const;

        [[nodiscard]] const Bitset64& OppositeDiagonal2(Player player) const;

        [[nodiscard]] Bitset64 GetCaptures(int32_t row, int32_t col, int32_t drow,
                                           int32_t dcol) const;

        Bitset64 is_first_;
        Bitset64 is_second_;
        Bitset64 is_first_vertical;
        Bitset64 is_second_vertical;
        Bitset64 is_first_diagonal1;
        Bitset64 is_second_diagonal1;
        Bitset64 is_first_diagonal2;
        Bitset64 is_second_diagonal2;
        Player player_;
        int32_t eval;
        void DeletePieces(Player player);
    };

}// namespace ReversiEngine
