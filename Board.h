#pragma once

#include <vector>

#include "Cell.h"

using std::vector;

enum Player {
    First,
    Second
};

class Board {
public:
    Board();
    Board(const vector<vector<char>>& state, const Player& player);
    vector<vector<char>>& GetState();
    vector<Cell> PossibleMoves() const;
    char MySymbol() const;
    Cell GetBestMove(int32_t depth) const;
    Board MakeMove(const Cell& cell) const;

private:
    void PlacePiece(const Cell& cell, const char& symbol);
    static bool IsInBoundingBox(const Cell& cell);
    bool IsThereCaptures(int32_t x, int32_t y, int32_t dx, int32_t dy, char my_symbol) const;

    int32_t FinalEvaluation() const;
    int32_t SmartEvaluation(int32_t depth, int32_t alpha, int32_t beta) const;

    vector<Cell> GetCaptures(int32_t x, int32_t y, int32_t dx, int32_t dy, char my_symbol) const;

    vector<vector<char>> state_;
    Player player_;
};


