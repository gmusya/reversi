#pragma once

#include <vector>

#include "cell.h"
#include <bitset>

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

    private:
        void PlacePiece(size_t position, Player player);

        void PlacePiece(const Cell& cell, Player player);

        static bool IsInBoundingBox(const Cell& cell);

        std::bitset<64>& Same(Player player);

        std::bitset<64>& Opposite(Player player);

        [[nodiscard]] const std::bitset<64>& Same(Player player) const;

        [[nodiscard]] const std::bitset<64>& Opposite(Player player) const;

        [[nodiscard]] std::bitset<64> GetCaptures(int32_t row, int32_t col, int32_t drow,
                                                    int32_t dcol) const;

        void CheckLine(Cell first, int dcol, int drow, std::bitset<64>& is_possible,
                       int32_t line_length) const;

        bool IsThereCaptures(int32_t row, int32_t col, int32_t drow, int32_t dcol) const;

        std::bitset<64> is_first_;
        std::bitset<64> is_second_;
        Player player_;
    };

}// namespace ReversiEngine
