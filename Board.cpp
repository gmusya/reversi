#include "Board.h"

#include <iostream>

Board::Board() {
    state_.resize(8, std::vector<char>(8, '*'));
    PlacePiece(Cell(4, 3), 'o');
    PlacePiece(Cell(3, 4), 'o');
    PlacePiece(Cell(3, 3), 'x');
    PlacePiece(Cell(4, 4), 'x');
    /*PlacePiece(Cell(0, 7), 'x');
    PlacePiece(Cell(0, 6), 'x');
    PlacePiece(Cell(1, 6), 'o');
    PlacePiece(Cell(2, 5), 'o');*/
    player_ = Player::First;
}

void Board::PlacePiece(const Cell& cell, const char& symbol) {
    state_[cell.y][cell.x] = symbol;
}

vector<vector<char>>& Board::GetState() {
    return state_;
}

bool Board::IsInBoundingBox(const Cell& cell) {
    return (0 <= cell.x && cell.x <= 7 && 0 <= cell.y && cell.y <= 7);
}

int32_t Board::FinalEvaluation() const {
    int32_t res = 0;
    for (const auto& vec : state_) {
        for (const auto& symbol : vec) {
            if (symbol != '*') {
                if ((symbol == 'o') ^ (player_ == Player::First)) {
                    ++res;
                } else {
                    --res;
                }
            }
        }
    }
    return res;
}

int32_t Board::SmartEvaluation(int32_t depth, int32_t alpha, int32_t beta) const {
    if (depth == 0) {
        return FinalEvaluation();
    }
    int32_t value = -64;
    vector<Cell> possible_moves = PossibleMoves();
    if (possible_moves.empty()) {
        Board new_board = Board(state_, player_ == Player::First ? Player::Second : Player::First);
        int32_t candidate_value = -new_board.SmartEvaluation(depth - 1, -beta, -alpha);
        if (value < candidate_value) {
            value = candidate_value;
        }
    } else {
        for (const Cell& cell : possible_moves) {
            Board board = MakeMove(cell);
            int32_t candidate_value = -board.SmartEvaluation(depth - 1, -beta, -alpha);
            if (value < candidate_value) {
                value = candidate_value;
            }
            if (alpha < value) {
                alpha = value;
            }
        }
    }
    return value;
}

Board Board::MakeMove(const Cell& cell) const {
    int32_t x = cell.x;
    int32_t y = cell.y;
    Board board = *this;
    for (int32_t dx = -1; dx <= 1; ++dx) {
        for (int32_t dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) {
                continue;
            }
            for (const Cell& captured_cell : GetCaptures(x, y, dx, dy, MySymbol())) {
                board.PlacePiece(captured_cell, MySymbol());
            }
        }
    }
    board.PlacePiece(cell, MySymbol());
    board.player_ = (player_ == Player::First ? Player::Second : Player::First);
    return board;
}

vector<Cell> Board::PossibleMoves() const {
    char my_symbol = MySymbol();
    vector<Cell> res;
    for (int32_t x = 0; x < 8; ++x) {
        for (int32_t y = 0; y < 8; ++y) {
            if (state_[y][x] != '*') {
                continue;
            }
            bool possible = false;
            for (int32_t dx = -1; dx <= 1 && !possible; ++dx) {
                for (int32_t dy = -1; dy <= 1 && !possible; ++dy) {
                    if (dx != 0 || dy != 0) {
                        possible = IsThereCaptures(x, y, dx, dy, my_symbol);
                    }
                }
            }
            if (possible) {
                res.emplace_back(x, y);
            }
        }
    }
    return res;
}

char Board::MySymbol() const {
    return (player_ == Player::First ? 'x' : 'o');
}

bool Board::IsThereCaptures(int32_t x, int32_t y, int32_t dx, int32_t dy, char my_symbol) const {
    for (int32_t k = 1; IsInBoundingBox(Cell(x + k * dx, y + k * dy)); ++k) {
        char symbol = state_[y + k * dy][x + k * dx];
        if (symbol == '*') {
            return false;
        }
        if (symbol == my_symbol) {
            if (k != 1) {
                return true;
            } else {
                return false;
            }
        }
    }
    return false;
}

vector<Cell> Board::GetCaptures(int32_t x, int32_t y, int32_t dx, int32_t dy, char my_symbol) const {
    vector<Cell> res;
    for (int32_t k = 1; IsInBoundingBox(Cell(x + k * dx, y + k * dy)); ++k) {
        char symbol = state_[y + k * dy][x + k * dx];
        if (symbol == '*') {
            return {};
        }
        if (symbol == my_symbol) {
            return res;
        }
        res.emplace_back(x + k * dx, y + k * dy);
    }
    return {};
}

Board::Board(const vector<vector<char>>& state, const Player& player) {
    state_ = state;
    player_ = player;
}

Cell Board::GetBestMove(int32_t depth) const {
    vector<Cell> possible_moves = PossibleMoves();
    int32_t value = -64;
    Cell best_move(-1, -1);
    for (const Cell& cell : possible_moves) {
        Board board = MakeMove(cell);
        int32_t candidate_value = -board.SmartEvaluation(depth - 1, -64, 64);
        if (value < candidate_value) {
            value = candidate_value;
            best_move = cell;
        }
    }
    return best_move;
}

