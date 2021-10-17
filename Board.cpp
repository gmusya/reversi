#include "Board.h"

Board::Board(int32_t depth) : depth_(depth), last_move_(-2, -2) {
    state_.resize(8, std::vector<char>(8, '*'));
    PlacePiece(Cell(4, 3), 'o');
    PlacePiece(Cell(3, 4), 'o');
    PlacePiece(Cell(3, 3), 'x');
    PlacePiece(Cell(4, 4), 'x');
    player_ = Player::First;
    print_board_mode_ = true;
    print_move_mode_ = true;
}

void Board::PlacePiece(const Cell& cell, const char& symbol) {
    state_[cell.y][cell.x] = symbol;
}

const vector<vector<char>>& Board::GetState() const {
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
        Board new_board = MakeMove(Cell(-1, -1));
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
    board.last_move_ = cell;
    if (x == -1 && y == -1) {
        board.player_ = (player_ == Player::First ? Player::Second : Player::First);
        return board;
    }
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

int32_t Board::GetDepth() const {
    return depth_;
}

void Board::ChangeDepth(int32_t depth) {
    depth_ = depth;
}

bool& Board::PrintMoveMode() {
    return print_move_mode_;
}

bool& Board::PrintBoardMode() {
    return print_board_mode_;
}

void Board::PrintMoveBoard(const Writer& writer) const {
    if (print_board_mode_) {
        writer.Print(GetState());
    }
    if (print_move_mode_) {
        writer.Print(last_move_);
    }
}

