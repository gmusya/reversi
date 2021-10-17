#pragma once

#include <vector>

#include "Cell.h"
#include "Writer.h"

using std::vector;

enum Player {
    First,
    Second
};

class Board {
public:
    explicit Board(int32_t depth);

    const vector<vector<char>>& GetState() const;

    vector<Cell> PossibleMoves() const;

    char MySymbol() const;

    Cell GetBestMove(int32_t depth) const;

    Board MakeMove(const Cell& cell) const;

    int32_t GetDepth() const;

    void ChangeDepth(int32_t depth);

    int32_t SmartEvaluation(int32_t depth, int32_t alpha, int32_t beta) const;

    bool& PrintMoveMode();

    bool& PrintBoardMode();

    void PrintMoveBoard(const Writer& writer) const;

private:
    void PlacePiece(const Cell& cell, const char& symbol);

    static bool IsInBoundingBox(const Cell& cell);

    bool IsThereCaptures(int32_t x, int32_t y, int32_t dx, int32_t dy, char my_symbol) const;

    int32_t FinalEvaluation() const;

    vector<Cell> GetCaptures(int32_t x, int32_t y, int32_t dx, int32_t dy, char my_symbol) const;

    vector<vector<char>> state_;
    Player player_;
    int32_t depth_ = 6;
    bool print_move_mode_ = false;
    bool print_board_mode_ = false;
    Cell last_move_;
};


