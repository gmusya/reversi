#pragma once

#include <vector>

#include "bitset64.h"
#include "cell.h"

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

        [[nodiscard]] int32_t OldFinalEvaluation() const;

        [[nodiscard]] bool GameEnded() const;

        friend std::ostream& operator<<(std::ostream& os, const Board& board);

        std::vector<Cell> OldPossibleMoves() const;

        void InitPrecalc() const;

    private:
        void PlacePiece(size_t position, Player player);

        void PlacePiece(const Cell& cell, Player player);

        static bool IsInBoundingBox(const Cell& cell);

        Bitset64& Same(Player player);

        Bitset64& Opposite(Player player);

        [[nodiscard]] const Bitset64& Same(Player player) const;

        [[nodiscard]] const Bitset64& Opposite(Player player) const;

        Bitset64& SameVertical(Player player);

        Bitset64& OppositeVertical(Player player);

        [[nodiscard]] const Bitset64& SameVertical(Player player) const;

        [[nodiscard]] const Bitset64& OppositeVertical(Player player) const;

        [[nodiscard]] Bitset64 GetCaptures(int32_t row, int32_t col, int32_t drow,
                                           int32_t dcol) const;

        void CheckLine(Cell first, int dcol, int drow, Bitset64& is_possible,
                       int32_t line_length) const;

        bool IsThereCaptures(int32_t row, int32_t col, int32_t drow, int32_t dcol) const;

        Bitset64 is_first_;
        Bitset64 is_second_;
        Bitset64 is_first_vertical;
        Bitset64 is_second_vertical;
        Player player_;
        int32_t eval;
        void PossibleMovesDiagonal(Bitset64& is_possible) const;
        void PossibleMovesVertical(Bitset64& is_possible) const;
        void PossibleMovesHorizontal(Bitset64& is_possible) const;
    };

}// namespace ReversiEngine
