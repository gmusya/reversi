#include "board.h"

#include <array>
#include <cassert>

std::array<Bitset64, 1 << 16> precalced_check_line;
std::array<std::array<int32_t, 1 << 16>, 8> precalced_row_costs;

namespace ReversiEngine {

    namespace {
        // clang-format off
        const std::array<int, 64> CELL_COST = {
            100,  30,  30,  30,  30,  30,  30, 100,
             30,   1,   1,   1,   1,   1,   1,  30,
             30,   1,   1,   1,   1,   1,   1,  30,
             30,   1,   1,   1,   1,   1,   1,  30,
             30,   1,   1,   1,   1,   1,   1,  30,
             30,   1,   1,   1,   1,   1,   1,  30,
             30,   1,   1,   1,   1,   1,   1,  30,
            100,  30,  30,  30,  30,  30,  30, 100
        };

        const std::array<int, 64> CONV_ROW_TABLE = {
                0,  8,  16, 24, 32, 40, 48, 56,
                1,  9,  17, 25, 33, 41, 49, 57,
                2,  10, 18, 26, 34, 42, 50, 58,
                3,  11, 19, 27, 35, 43, 51, 59,
                4,  12, 20, 28, 36, 44, 52, 60,
                5,  13, 21, 29, 37, 45, 53, 61,
                6,  14, 22, 30, 38, 46, 54, 62,
                7,  15, 23, 31, 39, 47, 55, 63};
        // clang-format on
    }// namespace

    void Board::InitPrecalc() const {
        for (int32_t mask_first = 0; mask_first < (1 << 8); ++mask_first) {
            for (int32_t mask_second = 0; mask_second < (1 << 8); ++mask_second) {
                if (mask_first & mask_second) {
                    continue;
                }
                Bitset64 is_first(mask_first);
                Bitset64 is_second(mask_second);
                Bitset64 is_possible;
                int32_t start_pos = 0;
                int32_t to_add = 1;
                int32_t current_position = 0;
                int32_t line_length = 8;
                while (current_position + 2 < line_length) {
                    if (!is_first[start_pos + current_position * to_add]) {
                        ++current_position;
                        continue;
                    }
                    int32_t k = 1;
                    while (current_position + k < line_length &&
                           is_second[start_pos + to_add * (current_position + k)]) {
                        ++k;
                    }
                    if (k > 1 && current_position + k < line_length &&
                        !is_first[start_pos + to_add * (current_position + k)]) {
                        is_possible[start_pos + to_add * (current_position + k)] = true;
                        current_position += k + 1;
                    } else {
                        current_position += k;
                    }
                }
                precalced_check_line[(mask_first << 8) + mask_second] = is_possible;
            }
        }
        for (int32_t mask_first = 0; mask_first < (1 << 8); ++mask_first) {
            for (int32_t mask_second = 0; mask_second < (1 << 8); ++mask_second) {
                if (mask_first & mask_second) {
                    continue;
                }
                Bitset64 is_first(mask_first);
                Bitset64 is_second(mask_second);
                Bitset64 is_possible;
                int32_t start_pos = 7;
                int32_t to_add = -1;
                int32_t current_position = 0;
                int32_t line_length = 8;
                while (current_position + 2 < line_length) {
                    if (!is_first[start_pos + current_position * to_add]) {
                        ++current_position;
                        continue;
                    }
                    int32_t k = 1;
                    while (current_position + k < line_length &&
                           is_second[start_pos + to_add * (current_position + k)]) {
                        ++k;
                    }
                    if (k > 1 && current_position + k < line_length &&
                        !is_first[start_pos + to_add * (current_position + k)]) {
                        is_possible[start_pos + to_add * (current_position + k)] = true;
                        current_position += k + 1;
                    } else {
                        current_position += k;
                    }
                }
                precalced_check_line[(mask_first << 8) + mask_second] |= is_possible;
            }
        }
        for (int32_t row = 0; row < 8; ++row) {
            for (int32_t mask_first = 0; mask_first < (1 << 8); ++mask_first) {
                for (int32_t mask_second = 0; mask_second < (1 << 8); ++mask_second) {
                    if (mask_first & mask_second) {
                        continue;
                    }
                    Bitset64 is_first(mask_first);
                    Bitset64 is_second(mask_second);
                    int32_t cost = 0;
                    for (int32_t col = 0; col < 8; ++col) {
                        if (is_first[col]) {
                            cost += CELL_COST[(row << 3) + col];
                        } else if (is_second[col]) {
                            cost -= CELL_COST[(row << 3) + col];
                        }
                    }
                    precalced_row_costs[row][(mask_first << 8) + mask_second] = cost;
                }
            }
        }
    }

    Board::Board() {
        eval = 0;
        PlacePiece(Cell{4, 3}, First);
        PlacePiece(Cell{3, 4}, First);
        PlacePiece(Cell{3, 3}, Second);
        PlacePiece(Cell{4, 4}, Second);
        player_ = First;
    }


    void Board::PlacePiece(size_t position, Player player) {
        Same(player)[position] = true;
        Opposite(player)[position] = false;
        SameVertical(player)[CONV_ROW_TABLE[position]] = true;
        OppositeVertical(player)[CONV_ROW_TABLE[position]] = false;
    }

    void Board::PlacePiece(const Cell& cell, Player player) {
        int32_t position = cell.ToInt();
        PlacePiece(position, player);
    }

    bool Board::IsInBoundingBox(const Cell& cell) {
        return (0 <= cell.col && cell.col <= 7 && 0 <= cell.row && cell.row <= 7);
    }

    int32_t Board::OldFinalEvaluation() const {
        int32_t result = 0;
        for (int32_t row = 0; row < 8; ++row) {
            for (int32_t col = 0; col < 8; ++col) {
                Cell cell{row, col};
                if (is_first_[cell.ToInt()]) {
                    result += CELL_COST[(row << 3) + col];
                } else if (is_second_[cell.ToInt()]) {
                    result -= CELL_COST[(row << 3) + col];
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
        Bitset64 captured;
        for (int32_t dcol = -1; dcol <= 1; ++dcol) {
            for (int32_t drow = -1; drow <= 1; ++drow) {
                if (dcol == 0 && drow == 0) {
                    continue;
                }
                captured |= GetCaptures(row, col, drow, dcol);
            }
        }
        for (size_t position = captured._Find_first(); position < 64;
             position = captured._Find_next(position)) {
            board.PlacePiece(position, player_);
        }
        board.PlacePiece(cell, player_);
        board.player_ = (player_ == Player::First ? Player::Second : Player::First);
        return board;
    }

    char Board::MySymbol() const {
        return (player_ == Player::First ? 'x' : 'o');
    }

    inline void Board::CheckLine(Cell first, int drow, int dcol, Bitset64& is_possible,
                                 int32_t line_length) const {
        int32_t start_pos = first.ToInt();
        int32_t to_add = Cell{drow, dcol}.ToInt();
        int32_t current_position = 0;
        while (current_position + 2 < line_length) {
            if (!Same(player_)[start_pos + current_position * to_add]) {
                ++current_position;
                continue;
            }
            int32_t k = 1;
            while (current_position + k < line_length &&
                   Opposite(player_)[start_pos + to_add * (current_position + k)]) {
                ++k;
            }
            if (k > 1 && current_position + k < line_length &&
                !Same(player_)[start_pos + to_add * (current_position + k)]) {
                is_possible[start_pos + to_add * (current_position + k)] = true;
                current_position += k + 1;
            } else {
                current_position += k;
            }
        }
    }

    namespace {
        void BitsetToVector(const Bitset64& is_possible, std::vector<Cell>& result) {
            result.clear();
            for (size_t position = is_possible._Find_first(); position < 64;
                 position = is_possible._Find_next(position)) {
                if (is_possible[position]) {
                    result.push_back({static_cast<int32_t>(position >> 3),
                                      static_cast<int32_t>(position & 7)});
                }
            }
        }
    }// namespace

    inline void Board::PossibleMovesHorizontal(Bitset64& is_possible) const {
        auto first_set = Same(player_);
        auto second_set = Opposite(player_);
        auto first = first_set.to_ullong();
        auto second = second_set.to_ullong();
        for (int32_t row = 0; row < 8; ++row) {
            int shift = 8 * row;
            auto first_mask = (first >> shift) & ((1 << 8) - 1);
            auto second_mask = (second >> shift) & ((1 << 8) - 1);
            auto x = precalced_check_line[(first_mask << 8) + second_mask];
            is_possible |= Bitset64(x.to_ullong() << shift);
        }
    }

    inline void Board::PossibleMovesVertical(Bitset64& is_possible) const {
        auto first_set = Same(player_);
        auto second_set = Opposite(player_);
        auto value_first = first_set.to_ullong();
        auto value_second = second_set.to_ullong();
        for (int32_t col = 0; col < 8; ++col) {
            auto x = (value_first >> col);
            auto first_mask = (x & 1) + (((x >> 8) & 1) << 1) + (((x >> 16) & 1) << 2) +
                              ((((x >> 24) & 1) << 3)) + (((x >> 32) & 1) << 4) +
                              (((x >> 40) & 1) << 5) + (((x >> 48) & 1) << 6) +
                              (((x >> 56) & 1) << 7);
            x = (value_second >> col);
            auto second_mask = (x & 1) + (((x >> 8) & 1) << 1) + (((x >> 16) & 1) << 2) +
                               ((((x >> 24) & 1) << 3)) + (((x >> 32) & 1) << 4) +
                               (((x >> 40) & 1) << 5) + (((x >> 48) & 1) << 6) +
                               (((x >> 56) & 1) << 7);
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            for (int32_t i = 0; i < 8; ++i) {
                if (res[i]) {
                    is_possible[(i << 3) + col] = true;
                }
            }
        }
    }

    inline void Board::PossibleMovesDiagonal(Bitset64& is_possible) const {
        auto first_set = Same(player_);
        auto second_set = Opposite(player_);
        auto value_first = first_set.to_ullong();
        auto value_second = second_set.to_ullong();
        for (int32_t col = 0; col < 6; ++col) {

            auto x = (value_first >> col);
            uint64_t first_mask = 0;
            for (int32_t i = 0; i < 8 - col; ++i) {
                first_mask |= ((x >> (i * 9)) & 1) << i;
            }
            x = (value_second >> col);
            uint64_t second_mask = 0;
            for (int32_t i = 0; i < 8 - col; ++i) {
                second_mask |= ((x >> (i * 9)) & 1) << i;
            }
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            for (int32_t i = 0; i < 8 - col; ++i) {
                if (res[i]) {
                    is_possible[(i << 3) + col + i] = true;
                }
            }
        }
        for (int32_t row = 1; row < 6; ++row) {
            auto x = (value_first >> (row << 3));
            uint64_t first_mask = 0;
            for (int32_t i = 0; i < 8 - row; ++i) {
                first_mask |= ((x >> (i * 9)) & 1) << i;
            }
            x = (value_second >> (row << 3));
            uint64_t second_mask = 0;
            for (int32_t i = 0; i < 8 - row; ++i) {
                second_mask |= ((x >> (i * 9)) & 1) << i;
            }
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            for (int32_t i = 0; i < 8 - row; ++i) {
                if (res[i]) {
                    is_possible[((i + row) << 3) + i] = true;
                }
            }
        }
        for (int32_t col = 2; col < 8; ++col) {
            auto x = (value_first >> col);
            uint64_t first_mask = 0;
            for (int32_t i = 0; i < col + 1; ++i) {
                first_mask |= ((x >> (i * 7)) & 1) << i;
            }
            x = (value_second >> col);
            uint64_t second_mask = 0;
            for (int32_t i = 0; i < col + 1; ++i) {
                second_mask |= ((x >> (i * 7)) & 1) << i;
            }
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            for (int32_t i = 0; i < col + 1; ++i) {
                if (res[i]) {
                    is_possible[(i << 3) + col - i] = true;
                }
            }
        }
        for (int32_t row = 1; row < 6; ++row) {
            auto x = (value_first >> ((row << 3) + 7));
            uint64_t first_mask = 0;
            for (int32_t i = 0; i < 8 - row; ++i) {
                first_mask |= ((x >> (i * 7)) & 1) << i;
            }
            x = (value_second >> ((row << 3) + 7));
            uint64_t second_mask = 0;
            for (int32_t i = 0; i < 8 - row; ++i) {
                second_mask |= ((x >> (i * 7)) & 1) << i;
            }
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            for (int32_t i = 0; i < 8 - row; ++i) {
                if (res[i]) {
                    is_possible[((row + i) << 3) + 7 - i] = true;
                }
            }
        }
    }

    void Board::PossibleMoves(std::vector<Cell>& result) const {
        Bitset64 is_possible;
        auto first_set = Same(player_);
        auto second_set = Opposite(player_);
        auto value_first = first_set.to_ullong();
        auto value_second = second_set.to_ullong();
        auto first_set_vertical = SameVertical(player_);
        auto second_set_vertical = OppositeVertical(player_);
        auto value_first_vertical = first_set_vertical.to_ullong();
        auto value_second_vertical = second_set_vertical.to_ullong();
        for (int32_t row = 0; row < 8; ++row) {
            int shift = 8 * row;
            auto first_mask = (value_first >> shift) & ((1 << 8) - 1);
            auto second_mask = (value_second >> shift) & ((1 << 8) - 1);
            auto x = precalced_check_line[(first_mask << 8) + second_mask];
            is_possible |= Bitset64(x.to_ullong() << shift);
        }
        for (int32_t col = 0; col < 8; ++col) {
            int shift = 8 * col;
            auto first_mask = (value_first_vertical >> shift) & ((1 << 8) - 1);
            auto second_mask = (value_second_vertical >> shift) & ((1 << 8) - 1);
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            for (int32_t i = 0; i < 8; ++i) {
                if (res[i]) {
                    is_possible[(i << 3) + col] = true;
                }
            }
        }
        for (int32_t col = 0; col < 6; ++col) {

            auto x = (value_first >> col);
            uint64_t first_mask = 0;
            for (int32_t i = 0; i < 8 - col; ++i) {
                first_mask |= ((x >> (i * 9)) & 1) << i;
            }
            x = (value_second >> col);
            uint64_t second_mask = 0;
            for (int32_t i = 0; i < 8 - col; ++i) {
                second_mask |= ((x >> (i * 9)) & 1) << i;
            }
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            for (int32_t i = 0; i < 8 - col; ++i) {
                if (res[i]) {
                    is_possible[(i << 3) + col + i] = true;
                }
            }
        }
        for (int32_t row = 1; row < 6; ++row) {
            auto x = (value_first >> (row << 3));
            uint64_t first_mask = 0;
            for (int32_t i = 0; i < 8 - row; ++i) {
                first_mask |= ((x >> (i * 9)) & 1) << i;
            }
            x = (value_second >> (row << 3));
            uint64_t second_mask = 0;
            for (int32_t i = 0; i < 8 - row; ++i) {
                second_mask |= ((x >> (i * 9)) & 1) << i;
            }
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            for (int32_t i = 0; i < 8 - row; ++i) {
                if (res[i]) {
                    is_possible[((i + row) << 3) + i] = true;
                }
            }
        }
        for (int32_t col = 2; col < 8; ++col) {
            auto x = (value_first >> col);
            uint64_t first_mask = 0;
            for (int32_t i = 0; i < col + 1; ++i) {
                first_mask |= ((x >> (i * 7)) & 1) << i;
            }
            x = (value_second >> col);
            uint64_t second_mask = 0;
            for (int32_t i = 0; i < col + 1; ++i) {
                second_mask |= ((x >> (i * 7)) & 1) << i;
            }
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            for (int32_t i = 0; i < col + 1; ++i) {
                if (res[i]) {
                    is_possible[(i << 3) + col - i] = true;
                }
            }
        }
        for (int32_t row = 1; row < 6; ++row) {
            auto x = (value_first >> ((row << 3) + 7));
            uint64_t first_mask = 0;
            for (int32_t i = 0; i < 8 - row; ++i) {
                first_mask |= ((x >> (i * 7)) & 1) << i;
            }
            x = (value_second >> ((row << 3) + 7));
            uint64_t second_mask = 0;
            for (int32_t i = 0; i < 8 - row; ++i) {
                second_mask |= ((x >> (i * 7)) & 1) << i;
            }
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            for (int32_t i = 0; i < 8 - row; ++i) {
                if (res[i]) {
                    is_possible[((row + i) << 3) + 7 - i] = true;
                }
            }
        }

        BitsetToVector(is_possible, result);
    }

    std::vector<Cell> Board::OldPossibleMoves() const {
        std::vector<Cell> res;
        for (int32_t row = 0; row < 8; ++row) {
            for (int32_t col = 0; col < 8; ++col) {
                int32_t position = Cell{row, col}.ToInt();
                if (is_first_[position] || is_second_[position]) {
                    continue;
                }
                bool possible = false;
                for (int32_t drow = -1; drow <= 1 && !possible; ++drow) {
                    for (int32_t dcol = -1; dcol <= 1 && !possible; ++dcol) {
                        if (dcol != 0 || drow != 0) {
                            possible = IsThereCaptures(row, col, drow, dcol);
                        }
                    }
                }
                if (possible) {
                    res.push_back({row, col});
                }
            }
        }
        return res;
    }

    bool Board::IsThereCaptures(int32_t row, int32_t col, int32_t drow, int32_t dcol) const {
        for (int32_t k = 1; IsInBoundingBox(Cell{row + k * drow, col + k * dcol}); ++k) {
            int32_t position = Cell{row + k * drow, col + k * dcol}.ToInt();
            if (!is_first_[position] && !is_second_[position]) {
                return false;
            }
            if (Same(player_)[position]) {
                if (k != 1) {
                    return true;
                } else {
                    return false;
                }
            }
        }
        return false;
    }

    Bitset64 Board::GetCaptures(int32_t row, int32_t col, int32_t drow, int32_t dcol) const {
        Bitset64 res;
        for (int32_t k = 1; IsInBoundingBox(Cell{row + k * drow, col + k * dcol}); ++k) {
            int32_t position = Cell{row + k * drow, col + k * dcol}.ToInt();
            if (!is_first_[position] && !is_second_[position]) {
                return {};
            }
            if (Same(player_)[position]) {
                return res;
            }
            res[position] = true;
        }
        return {};
    }

    bool Board::GameEnded() const {
        return (PossibleMoves().empty() && (MakeMove(Cell{-1, -1})).PossibleMoves().empty());
    }

    Bitset64& Board::Same(Player player) {
        if (player == First) {
            return is_first_;
        } else {
            return is_second_;
        }
    }

    Bitset64& Board::Opposite(Player player) {
        if (player == First) {
            return is_second_;
        } else {
            return is_first_;
        }
    }

    const Bitset64& Board::Same(Player player) const {
        if (player == First) {
            return is_first_;
        } else {
            return is_second_;
        }
    }

    const Bitset64& Board::Opposite(Player player) const {
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

    std::vector<Cell> Board::PossibleMoves() const {
        std::vector<Cell> result;
        PossibleMoves(result);
        return result;
    }

    int32_t Board::FinalEvaluation() const {
        auto first = is_first_.to_ullong();
        auto second = is_second_.to_ullong();
        int32_t res = 0;
        for (int32_t row = 0; row < 8; ++row) {
            int shift = 8 * row;
            auto first_mask = (first >> shift) & ((1 << 8) - 1);
            auto second_mask = (second >> shift) & ((1 << 8) - 1);
            res += precalced_row_costs[(row == 0 || row == 7) ? 0 : 1][(first_mask << 8) + second_mask];
        }
        return (player_ == First ? res : -res);
    }

    Bitset64& Board::SameVertical(Player player) {
        if (player == First) {
            return is_first_vertical;
        } else {
            return is_second_vertical;
        }
    }

    Bitset64& Board::OppositeVertical(Player player) {
        if (player == First) {
            return is_second_vertical;
        } else {
            return is_first_vertical;
        }
    }

    const Bitset64& Board::SameVertical(Player player) const {
        if (player == First) {
            return is_first_vertical;
        } else {
            return is_second_vertical;
        }
    }

    const Bitset64& Board::OppositeVertical(Player player) const {
        if (player == First) {
            return is_second_vertical;
        } else {
            return is_first_vertical;
        }
    }

}// namespace ReversiEngine
