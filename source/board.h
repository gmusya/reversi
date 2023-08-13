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

        [[nodiscard]] Board MakeMoveLast(const Cell& cell) const;

        [[nodiscard]] int32_t FinalEvaluation() const;

        [[nodiscard]] bool GameEnded() const;

        friend std::ostream& operator<<(std::ostream& os, const Board& board);

        void InitPrecalc() const;

    private:
        void PlacePiece(size_t position, Player player);

        void PlacePiece(const Cell& cell, Player player);


        Bitset64 is_first_;
        Bitset64 is_second_;
        Bitset64 is_first_vertical;
        Bitset64 is_second_vertical;
        Bitset64 is_first_diagonal1;
        Bitset64 is_second_diagonal1;
        Bitset64 is_first_diagonal2;
        Bitset64 is_second_diagonal2;
        Player player_;
        void DeletePieces();
        void PlacePiece(size_t position);
        void DeletePiecesLast();
        void PlacePieceLast(size_t position);
    };

}// namespace ReversiEngine
