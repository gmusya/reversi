#pragma once

#include <vector>

#include "cell.h"
#include <bitset>

namespace ReversiEngine {
    enum Player { First, Second };

    class Board {
    public:
        Board();

        [[nodiscard]] std::vector<std::vector<char>> GetState() const;

        [[nodiscard]] std::vector<Cell> PossibleMoves() const;

        [[nodiscard]] char MySymbol() const;

        [[nodiscard]] Board MakeMove(const Cell& cell) const;

        [[nodiscard]] int32_t FinalEvaluation() const;

        [[nodiscard]] bool GameEnded() const;

        friend std::ostream& operator<<(std::ostream& os, const Board& board);

    private:
        void PlacePiece(const Cell& cell, Player player);

        static bool IsInBoundingBox(const Cell& cell);

        std::bitset<64>& Same(Player player);

        std::bitset<64>& Opposite(Player player);

        [[nodiscard]] const std::bitset<64>& Same(Player player) const;

        [[nodiscard]] const std::bitset<64>& Opposite(Player player) const;

        [[nodiscard]] bool IsThereCaptures(int32_t row, int32_t col, int32_t drow,
                                           int32_t dcol) const;


        [[nodiscard]] std::vector<Cell> GetCaptures(int32_t row, int32_t col, int32_t drow,
                                                    int32_t dcol) const;

        std::bitset<64> is_first_;
        std::bitset<64> is_second_;
        Player player_;
    };

}// namespace ReversiEngine
