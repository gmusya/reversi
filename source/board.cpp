#include "board.h"

#include <array>

namespace ReversiEngine {

    namespace {
        // clang-format off
        const std::array<std::array<int, 8>, 8> CELL_COST = {{
            {100,  30,  30,  30,  30,  30,  30, 100},
            { 30,   1,   1,   1,   1,   1,   1,  30},
            { 30,   1,   1,   1,   1,   1,   1,  30},
            { 30,   1,   1,   1,   1,   1,   1,  30},
            { 30,   1,   1,   1,   1,   1,   1,  30},
            { 30,   1,   1,   1,   1,   1,   1,  30},
            { 30,   1,   1,   1,   1,   1,   1,  30},
            {100,  30,  30,  30,  30,  30,  30, 100}
        }};
        // clang-format on
    }// namespace

    Board::Board() {
        PlacePiece(Cell{4, 3}, First);
        PlacePiece(Cell{3, 4}, First);
        PlacePiece(Cell{3, 3}, Second);
        PlacePiece(Cell{4, 4}, Second);
        player_ = First;
    }

    void Board::PlacePiece(const Cell& cell, Player player) {
        int32_t position = cell.ToInt();
        Same(player)[position] = true;
        Opposite(player)[position] = false;
    }

    bool Board::IsInBoundingBox(const Cell& cell) {
        return (0 <= cell.col && cell.col <= 7 && 0 <= cell.row && cell.row <= 7);
    }

    int32_t Board::FinalEvaluation() const {
        int32_t result = 0;
        for (int32_t row = 0; row < 8; ++row) {
            for (int32_t col = 0; col < 8; ++col) {
                Cell cell{row, col};
                if (is_first_[cell.ToInt()]) {
                    result += CELL_COST[row][col];
                } else if (is_second_[cell.ToInt()]) {
                    result -= CELL_COST[row][col];
                }
            }
        }
        if (player_ == First) {
            return result;
        } else {
            return -result;
        }
    }

    Board Board::MakeMove(const Cell& cell) const {
        int32_t col = cell.col;
        int32_t row = cell.row;
        Board board = *this;
        if (col == -1 && row == -1) {
            board.player_ = (player_ == Player::First ? Player::Second : Player::First);
            return board;
        }
        for (int32_t dcol = -1; dcol <= 1; ++dcol) {
            for (int32_t drow = -1; drow <= 1; ++drow) {
                if (dcol == 0 && drow == 0) {
                    continue;
                }
                for (const Cell& captured_cell : GetCaptures(row, col, drow, dcol)) {
                    board.PlacePiece(captured_cell, player_);
                }
            }
        }
        board.PlacePiece(cell, player_);
        board.player_ = (player_ == Player::First ? Player::Second : Player::First);
        return board;
    }

    char Board::MySymbol() const {
        return (player_ == Player::First ? 'x' : 'o');
    }

    inline void Board::CheckLine(Cell first, int drow, int dcol,
                                 std::bitset<64>& is_possible) const {
        while (IsInBoundingBox(first)) {
            if (!Same(player_)[first.ToInt()]) {
                first.row += drow;
                first.col += dcol;
                continue;
            }
            int32_t k = 1;
            while (IsInBoundingBox(Cell{first.row + k * drow, first.col + k * dcol}) &&
                   Opposite(player_)[Cell{first.row + k * drow, first.col + k * dcol}.ToInt()]) {
                ++k;
            }
            if (k > 1 && IsInBoundingBox(Cell{first.row + k * drow, first.col + k * dcol}) &&
                !is_first_[Cell{first.row + k * drow, first.col + k * dcol}.ToInt()]) {
                is_possible[Cell{first.row + k * drow, first.col + k * dcol}.ToInt()] = true;
            }
            first = Cell{first.row + (k + 1) * drow, first.col + (k + 1) * dcol};
        }
    }

    std::vector<Cell> Board::PossibleMoves() const {
        std::bitset<64> is_possible;
        for (int32_t row = 0; row < 8; ++row) {
            CheckLine({row, 0}, 0, 1, is_possible);
            CheckLine({row, 7}, 0, -1, is_possible);
        }
        for (int32_t col = 0; col < 8; ++col) {
            CheckLine({0, col}, 1, 0, is_possible);
            CheckLine({7, col}, -1, 0, is_possible);
        }
        for (int32_t col = 0; col < 8; ++col) {
            CheckLine({0, col}, 1, 1, is_possible);
            CheckLine({7 - col, 7}, -1, -1, is_possible);
        }
        for (int32_t row = 1; row < 8; ++row) {
            CheckLine({row, 0}, 1, 1, is_possible);
            CheckLine({7, 7 - row}, -1, -1, is_possible);
        }
        for (int32_t col = 1; col < 8; ++col) {
            CheckLine({0, col}, 1, -1, is_possible);
            CheckLine({col, 0}, -1, 1, is_possible);
        }
        for (int32_t row = 0; row < 8; ++row) {
            CheckLine({row, 7}, 1, -1, is_possible);
            CheckLine({7, row}, -1, 1, is_possible);
        }
        std::vector<Cell> result;
        for (int32_t position = 0; position < 64; ++position) {
            if (is_possible[position]) {
                result.push_back({position >> 3, position & 7});
            }
        }
        return result;
    }

    std::vector<Cell> Board::GetCaptures(int32_t row, int32_t col, int32_t drow,
                                         int32_t dcol) const {
        std::vector<Cell> res;
        for (int32_t k = 1; IsInBoundingBox(Cell{row + k * drow, col + k * dcol}); ++k) {
            int32_t position = Cell{row + k * drow, col + k * dcol}.ToInt();
            if (!is_first_[position] && !is_second_[position]) {
                return {};
            }
            if (Same(player_)[position]) {
                return res;
            }
            res.push_back({row + k * drow, col + k * dcol});
        }
        return {};
    }

    bool Board::GameEnded() const {
        return (PossibleMoves().empty() && (MakeMove(Cell{-1, -1})).PossibleMoves().empty());
    }

    std::bitset<64>& Board::Same(Player player) {
        if (player == First) {
            return is_first_;
        } else {
            return is_second_;
        }
    }

    std::bitset<64>& Board::Opposite(Player player) {
        if (player == First) {
            return is_second_;
        } else {
            return is_first_;
        }
    }

    const std::bitset<64>& Board::Same(Player player) const {
        if (player == First) {
            return is_first_;
        } else {
            return is_second_;
        }
    }

    const std::bitset<64>& Board::Opposite(Player player) const {
        if (player == First) {
            return is_second_;
        } else {
            return is_first_;
        }
    }

    std::ostream& operator<<(std::ostream& os, const Board& board) {
        for (int32_t row = 7; row >= 0; --row) {
            os << static_cast<char>('1' + row) << ' ';
            for (int32_t col = 0; col <= 7; ++col) {
                int32_t position = Cell{row, col}.ToInt();
                if (board.is_first_[position]) {
                    os << "x";
                } else if (board.is_second_[position]) {
                    os << "o";
                } else {
                    os << "*";
                }
                os << " ";
            }
            os << "\n";
        }
        os << "  ";
        for (int32_t col = 0; col <= 7; ++col) {
            os << static_cast<char>('a' + col) << ' ';
        }
        return os;
    }

}// namespace ReversiEngine
