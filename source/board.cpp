#include "board.h"

#include <array>

std::array<Bitset8, 1 << 16> precalced_check_line;
std::array<std::array<int32_t, 1 << 16>, 8> precalced_row_costs;
std::array<std::array<Bitset8, 1 << 16>, 8> precalced_captures;

namespace ReversiEngine {

    namespace {
        // clang-format off
        const std::array<uint8_t, 64> CONV_POSITION_ROW = {
            100,  30,  30,  30,  30,  30,  30, 100,
             30,   1,   1,   1,   1,   1,   1,  30,
             30,   1,   1,   1,   1,   1,   1,  30,
             30,   1,   1,   1,   1,   1,   1,  30,
             30,   1,   1,   1,   1,   1,   1,  30,
             30,   1,   1,   1,   1,   1,   1,  30,
             30,   1,   1,   1,   1,   1,   1,  30,
            100,  30,  30,  30,  30,  30,  30, 100
        };

        const std::array<uint8_t, 64> CONV_POSITION_COL = {
                0,  8, 16, 24, 32, 40, 48, 56,
                1,  9, 17, 25, 33, 41, 49, 57,
                2, 10, 18, 26, 34, 42, 50, 58,
                3, 11, 19, 27, 35, 43, 51, 59,
                4, 12, 20, 28, 36, 44, 52, 60,
                5, 13, 21, 29, 37, 45, 53, 61,
                6, 14, 22, 30, 38, 46, 54, 62,
                7, 15, 23, 31, 39, 47, 55, 63
        };

        const std::array<uint8_t, 64> CONV_POSITION_DIAG1_POS = {
                28, 21, 15, 10,  6,  3,  1,  0,
                36, 29, 22, 16, 11,  7,  4,  2,
                43, 37, 30, 23, 17, 12,  8,  5,
                49, 44, 38, 31, 24, 18, 13,  9,
                54, 50, 45, 39, 32, 25, 19, 14,
                58, 55, 51, 46, 40, 33, 26, 20,
                61, 59, 56, 52, 47, 41, 34, 27,
                63, 62, 60, 57, 53, 48, 42, 35
        };

        const std::array<uint8_t, 64> CONV_POSITION_DIAG2_POS = {
                 0,  1,  3,  6, 10, 15, 21, 28,
                 2,  4,  7, 11, 16, 22, 29, 36,
                 5,  8, 12, 17, 23, 30, 37, 43,
                 9, 13, 18, 24, 31, 38, 44, 49,
                14, 19, 25, 32, 39, 45, 50, 54,
                20, 26, 33, 40, 46, 51, 55, 58,
                27, 34, 41, 47, 52, 56, 59, 61,
                35, 42, 48, 53, 57, 60, 62, 63
        };
        // clang-format on

        constexpr std::array<uint8_t, 8> offsets_col_diag1 = {28, 21, 15, 10, 6, 3, 0, 0};

        constexpr std::array<uint64_t, 8> col_diag1 = {(1ull << 36) - (1ull << 28),
                                                       (1ull << 28) - (1ull << 21),
                                                       (1ull << 21) - (1ull << 15),
                                                       (1ull << 15) - (1ull << 10),
                                                       (1ull << 10) - (1ull << 6),
                                                       (1ull << 6) - (1ull << 3),
                                                       0,
                                                       0};

        constexpr std::array<uint8_t, 8> offsets_row_diag1 = {0, 36, 43, 49, 54, 58, 0, 0};

        constexpr std::array<uint64_t, 8> row_diag1 = {0,
                                                       (1ull << 43) - (1ull << 36),
                                                       (1ull << 49) - (1ull << 43),
                                                       (1ull << 54) - (1ull << 49),
                                                       (1ull << 58) - (1ull << 54),
                                                       (1ull << 61) - (1ull << 58),
                                                       0,
                                                       0};

        constexpr std::array<uint8_t, 8> offsets_col_diag2 = {0, 0, 3, 6, 10, 15, 21, 28};

        constexpr std::array<uint64_t, 8> col_diag2 = {0,
                                                       0,
                                                       (1ull << 6) - (1ull << 3),
                                                       (1ull << 10) - (1ull << 6),
                                                       (1ull << 15) - (1ull << 10),
                                                       (1ull << 21) - (1ull << 15),
                                                       (1ull << 28) - (1ull << 21),
                                                       (1ull << 36) - (1ull << 28)};

        constexpr std::array<uint8_t, 8> offsets_row_diag2 = {28, 36, 43, 49, 54, 58, 0, 0};

        constexpr std::array<uint64_t, 8> row_diag2 = {(1ull << 36) - (1ull << 28),
                                                       (1ull << 43) - (1ull << 36),
                                                       (1ull << 49) - (1ull << 43),
                                                       (1ull << 54) - (1ull << 49),
                                                       (1ull << 58) - (1ull << 54),
                                                       (1ull << 61) - (1ull << 58),
                                                       0,
                                                       0};
    }// namespace

    void Board::InitPrecalc() const {
        for (int32_t mask_first = 0; mask_first < (1 << 8); ++mask_first) {
            for (int32_t mask_second = 0; mask_second < (1 << 8); ++mask_second) {
                if (mask_first & mask_second) {
                    continue;
                }
                Bitset64 is_first(mask_first);
                Bitset64 is_second(mask_second);
                Bitset8 is_possible;
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
                Bitset8 is_possible;
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
                            cost += CONV_POSITION_ROW[(row << 3) + col];
                        } else if (is_second[col]) {
                            cost -= CONV_POSITION_ROW[(row << 3) + col];
                        }
                    }
                    precalced_row_costs[row][(mask_first << 8) + mask_second] = cost;
                }
            }
        }
        for (int32_t mask_first = 0; mask_first < (1 << 8); ++mask_first) {
            for (int32_t mask_second = 0; mask_second < (1 << 8); ++mask_second) {
                if (mask_first & mask_second) {
                    continue;
                }
                Bitset8 is_first(mask_first);
                Bitset8 is_second(mask_second);
                for (int32_t position = 0; position < 8; ++position) {
                    Bitset8 res;
                    {
                        Bitset8 cur;
                        bool good = false;
                        for (int32_t k = 1; position - k >= 0; ++k) {
                            if (!is_first[position - k] && !is_second[position - k]) {
                                break;
                            }
                            if (is_first[position - k]) {
                                good = true;
                                break;
                            }
                            cur[position - k] = true;
                        }
                        if (good) {
                            res |= cur;
                        }
                    }
                    {
                        Bitset8 cur;
                        bool good = false;
                        for (int32_t k = 1; position + k < 8; ++k) {
                            if (!is_first[position + k] && !is_second[position + k]) {
                                break;
                            }
                            if (is_first[position + k]) {
                                good = true;
                                break;
                            }
                            cur[position + k] = true;
                        }
                        if (good) {
                            res |= cur;
                        }
                    }
                    precalced_captures[position][(mask_first << 8) + mask_second] = res;
                }
            }
        }
    }

    Board::Board() {
        PlacePiece(Cell{4, 3}, First);
        PlacePiece(Cell{3, 4}, First);
        PlacePiece(Cell{3, 3}, Second);
        PlacePiece(Cell{4, 4}, Second);
        player_ = First;
    }

    uint64_t counter = 0;
    void Board::PlacePiece(size_t position, Player player) {
        ++counter;
        if (player == First) {
            is_first_[position] = true;
            is_first_vertical[CONV_POSITION_COL[position]] = true;
            is_first_diagonal1[CONV_POSITION_DIAG1_POS[position]] = true;
            is_first_diagonal2[CONV_POSITION_DIAG2_POS[position]] = true;
        } else {
            is_second_[position] = true;
            is_second_vertical[CONV_POSITION_COL[position]] = true;
            is_second_diagonal1[CONV_POSITION_DIAG1_POS[position]] = true;
            is_second_diagonal2[CONV_POSITION_DIAG2_POS[position]] = true;
        }
    }

    inline void Board::PlacePiece(size_t position) {
        ++counter;
        is_first_[position] = true;
        is_first_vertical[CONV_POSITION_COL[position]] = true;
        is_first_diagonal1[CONV_POSITION_DIAG1_POS[position]] = true;
        is_first_diagonal2[CONV_POSITION_DIAG2_POS[position]] = true;
    }

    inline void Board::DeletePieces() {
        is_second_ &= ~(is_first_);
        is_second_vertical &= ~(is_first_vertical);
        is_second_diagonal1 &= ~(is_first_diagonal1);
        is_second_diagonal2 &= ~(is_first_diagonal2);
    }

    void Board::DeletePiecesLast() {
        is_second_ &= ~(is_first_);
    }

    void Board::PlacePieceLast(size_t position) {
        is_first_[position] = true;
    }

    inline void Board::PlacePiece(const Cell& cell, Player player) {
        int32_t position = cell.ToInt();
        PlacePiece(position, player);
    }



    Board Board::MakeMove(const Cell& cell) const {
        int32_t col = cell.col;
        int32_t row = cell.row;
        Board board = *this;
        if (col == -1 && row == -1) {
            std::swap(board.is_first_, board.is_second_);
            std::swap(board.is_first_vertical, board.is_second_vertical);
            std::swap(board.is_first_diagonal1, board.is_second_diagonal1);
            std::swap(board.is_first_diagonal2, board.is_second_diagonal2);
            board.player_ = (player_ == Player::First ? Player::Second : Player::First);
            return board;
        }
        Bitset64 other_captured;
        {
            {
                int shift = 8 * row;
                auto first_mask = (is_first_.to_ullong() >> shift) & ((1 << 8) - 1);
                auto second_mask = (is_second_.to_ullong() >> shift) & ((1 << 8) - 1);
                auto x = precalced_captures[col][(first_mask << 8) + second_mask];
                other_captured |= Bitset64(x.to_ullong() << shift);
            }
            {
                int shift = 8 * col;
                auto first_mask = (is_first_vertical.to_ullong() >> shift) & ((1 << 8) - 1);
                auto second_mask = (is_second_vertical.to_ullong() >> shift) & ((1 << 8) - 1);
                auto res = precalced_captures[row][(first_mask << 8) + second_mask];
                for (uint64_t i = res._Find_first(); i < res._Find_last(); ++i) {
                    other_captured[(i << 3) + col] = true;
                }
            }
            {
                if (col >= row) {
                    int32_t id = col - row;
                    uint64_t val = col_diag1[id];
                    uint64_t first_mask =
                            (is_first_diagonal1.to_ullong() & val) >> offsets_col_diag1[id];
                    uint64_t second_mask =
                            (is_second_diagonal1.to_ullong() & val) >> offsets_col_diag1[id];
                    auto res = precalced_captures[row][(first_mask << 8) + second_mask];
                    for (uint64_t i = res._Find_first(); i < res._Find_last(); ++i) {
                        other_captured[(i << 3) + id + i] = true;
                    }
                } else {
                    int32_t id = row - col;
                    uint64_t val = row_diag1[id];
                    uint64_t first_mask =
                            (is_first_diagonal1.to_ullong() & val) >> offsets_row_diag1[id];
                    uint64_t second_mask =
                            (is_second_diagonal1.to_ullong() & val) >> offsets_row_diag1[id];
                    auto res = precalced_captures[col][(first_mask << 8) + second_mask];
                    for (uint64_t i = res._Find_first(); i < res._Find_last(); ++i) {
                        other_captured[((i + id) << 3) + i] = true;
                    }
                }
            }
            {
                if (col + row <= 7) {
                    int32_t id = col + row;
                    uint64_t val = col_diag2[id];
                    uint64_t first_mask =
                            (is_first_diagonal2.to_ullong() & val) >> offsets_col_diag2[id];
                    uint64_t second_mask =
                            (is_second_diagonal2.to_ullong() & val) >> offsets_col_diag2[id];

                    auto res = precalced_captures[row][(first_mask << 8) + second_mask];
                    for (uint64_t i = res._Find_first(); i < res._Find_last(); ++i) {
                        other_captured[(i << 3) + id - i] = true;
                    }
                } else {
                    int32_t id = col + row - 7;
                    uint64_t val = row_diag2[id];
                    uint64_t first_mask =
                            (is_first_diagonal2.to_ullong() & val) >> offsets_row_diag2[id];
                    uint64_t second_mask =
                            (is_second_diagonal2.to_ullong() & val) >> offsets_row_diag2[id];
                    auto res = precalced_captures[row - id][(first_mask << 8) + second_mask];
                    for (uint64_t i = res._Find_first(); i < res._Find_last(); ++i) {
                        other_captured[((id + i) << 3) + 7 - i] = true;
                    }
                }
            }
        }
        for (size_t position = other_captured._Find_first(); position < 64;
             position = other_captured._Find_next(position)) {
            board.PlacePiece(position);
        }
        board.PlacePiece(cell.ToInt());
        board.DeletePieces();
        std::swap(board.is_first_, board.is_second_);
        std::swap(board.is_first_vertical, board.is_second_vertical);
        std::swap(board.is_first_diagonal1, board.is_second_diagonal1);
        std::swap(board.is_first_diagonal2, board.is_second_diagonal2);
        board.player_ = (player_ == Player::First ? Player::Second : Player::First);
        return board;
    }

    char Board::MySymbol() const {
        return (player_ == Player::First ? 'x' : 'o');
    }

    namespace {
        void BitsetToVector(const Bitset64& is_possible, std::vector<Cell>& result) {
            result.clear();
            for (size_t position = is_possible._Find_first(); position < 64;
                 position = is_possible._Find_next(position)) {
                result.push_back(
                        {static_cast<int32_t>(position >> 3), static_cast<int32_t>(position & 7)});
            }
        }
    }// namespace

    void Board::PossibleMoves(std::vector<Cell>& result) const {
        Bitset64 is_possible;
        auto value_first = is_first_.to_ullong();
        auto value_second = is_second_.to_ullong();
        for (int32_t row = 0; row < 8; ++row) {
            int shift = 8 * row;
            auto first_mask = (value_first >> shift) & ((1 << 8) - 1);
            auto second_mask = (value_second >> shift) & ((1 << 8) - 1);
            auto x = precalced_check_line[(first_mask << 8) + second_mask];
            is_possible |= Bitset64(x.to_ullong() << shift);
        }
        value_first = is_first_vertical.to_ullong();
        value_second = is_second_vertical.to_ullong();
        {
            auto first_mask = (value_first >> 0) & ((1 << 8) - 1);
            auto second_mask = (value_second >> 0) & ((1 << 8) - 1);
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            is_possible.value |= ((res.to_ullong() >> 0) & 1) << ((0 << 3) + 0);
            is_possible.value |= ((res.to_ullong() >> 1) & 1) << ((1 << 3) + 0);
            is_possible.value |= ((res.to_ullong() >> 2) & 1) << ((2 << 3) + 0);
            is_possible.value |= ((res.to_ullong() >> 3) & 1) << ((3 << 3) + 0);
            is_possible.value |= ((res.to_ullong() >> 4) & 1) << ((4 << 3) + 0);
            is_possible.value |= ((res.to_ullong() >> 5) & 1) << ((5 << 3) + 0);
            is_possible.value |= ((res.to_ullong() >> 6) & 1) << ((6 << 3) + 0);
            is_possible.value |= ((res.to_ullong() >> 7) & 1) << ((7 << 3) + 0);
        }
        {
            auto first_mask = (value_first >> 8) & ((1 << 8) - 1);
            auto second_mask = (value_second >> 8) & ((1 << 8) - 1);
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            is_possible.value |= ((res.to_ullong() >> 0) & 1) << ((0 << 3) + 1);
            is_possible.value |= ((res.to_ullong() >> 1) & 1) << ((1 << 3) + 1);
            is_possible.value |= ((res.to_ullong() >> 2) & 1) << ((2 << 3) + 1);
            is_possible.value |= ((res.to_ullong() >> 3) & 1) << ((3 << 3) + 1);
            is_possible.value |= ((res.to_ullong() >> 4) & 1) << ((4 << 3) + 1);
            is_possible.value |= ((res.to_ullong() >> 5) & 1) << ((5 << 3) + 1);
            is_possible.value |= ((res.to_ullong() >> 6) & 1) << ((6 << 3) + 1);
            is_possible.value |= ((res.to_ullong() >> 7) & 1) << ((7 << 3) + 1);
        }
        {
            auto first_mask = (value_first >> 16) & ((1 << 8) - 1);
            auto second_mask = (value_second >> 16) & ((1 << 8) - 1);
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            is_possible.value |= ((res.to_ullong() >> 0) & 1) << ((0 << 3) + 2);
            is_possible.value |= ((res.to_ullong() >> 1) & 1) << ((1 << 3) + 2);
            is_possible.value |= ((res.to_ullong() >> 2) & 1) << ((2 << 3) + 2);
            is_possible.value |= ((res.to_ullong() >> 3) & 1) << ((3 << 3) + 2);
            is_possible.value |= ((res.to_ullong() >> 4) & 1) << ((4 << 3) + 2);
            is_possible.value |= ((res.to_ullong() >> 5) & 1) << ((5 << 3) + 2);
            is_possible.value |= ((res.to_ullong() >> 6) & 1) << ((6 << 3) + 2);
            is_possible.value |= ((res.to_ullong() >> 7) & 1) << ((7 << 3) + 2);
        }
        {
            auto first_mask = (value_first >> 24) & ((1 << 8) - 1);
            auto second_mask = (value_second >> 24) & ((1 << 8) - 1);
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            is_possible.value |= ((res.to_ullong() >> 0) & 1) << ((0 << 3) + 3);
            is_possible.value |= ((res.to_ullong() >> 1) & 1) << ((1 << 3) + 3);
            is_possible.value |= ((res.to_ullong() >> 2) & 1) << ((2 << 3) + 3);
            is_possible.value |= ((res.to_ullong() >> 3) & 1) << ((3 << 3) + 3);
            is_possible.value |= ((res.to_ullong() >> 4) & 1) << ((4 << 3) + 3);
            is_possible.value |= ((res.to_ullong() >> 5) & 1) << ((5 << 3) + 3);
            is_possible.value |= ((res.to_ullong() >> 6) & 1) << ((6 << 3) + 3);
            is_possible.value |= ((res.to_ullong() >> 7) & 1) << ((7 << 3) + 3);
        }
        {
            auto first_mask = (value_first >> 32) & ((1 << 8) - 1);
            auto second_mask = (value_second >> 32) & ((1 << 8) - 1);
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            is_possible.value |= ((res.to_ullong() >> 0) & 1) << ((0 << 3) + 4);
            is_possible.value |= ((res.to_ullong() >> 1) & 1) << ((1 << 3) + 4);
            is_possible.value |= ((res.to_ullong() >> 2) & 1) << ((2 << 3) + 4);
            is_possible.value |= ((res.to_ullong() >> 3) & 1) << ((3 << 3) + 4);
            is_possible.value |= ((res.to_ullong() >> 4) & 1) << ((4 << 3) + 4);
            is_possible.value |= ((res.to_ullong() >> 5) & 1) << ((5 << 3) + 4);
            is_possible.value |= ((res.to_ullong() >> 6) & 1) << ((6 << 3) + 4);
            is_possible.value |= ((res.to_ullong() >> 7) & 1) << ((7 << 3) + 4);
        }
        {
            auto first_mask = (value_first >> 40) & ((1 << 8) - 1);
            auto second_mask = (value_second >> 40) & ((1 << 8) - 1);
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            is_possible.value |= ((res.to_ullong() >> 0) & 1) << ((0 << 3) + 5);
            is_possible.value |= ((res.to_ullong() >> 1) & 1) << ((1 << 3) + 5);
            is_possible.value |= ((res.to_ullong() >> 2) & 1) << ((2 << 3) + 5);
            is_possible.value |= ((res.to_ullong() >> 3) & 1) << ((3 << 3) + 5);
            is_possible.value |= ((res.to_ullong() >> 4) & 1) << ((4 << 3) + 5);
            is_possible.value |= ((res.to_ullong() >> 5) & 1) << ((5 << 3) + 5);
            is_possible.value |= ((res.to_ullong() >> 6) & 1) << ((6 << 3) + 5);
            is_possible.value |= ((res.to_ullong() >> 7) & 1) << ((7 << 3) + 5);
        }
        {
            auto first_mask = (value_first >> 48) & ((1 << 8) - 1);
            auto second_mask = (value_second >> 48) & ((1 << 8) - 1);
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            is_possible.value |= ((res.to_ullong() >> 0) & 1) << ((0 << 3) + 6);
            is_possible.value |= ((res.to_ullong() >> 1) & 1) << ((1 << 3) + 6);
            is_possible.value |= ((res.to_ullong() >> 2) & 1) << ((2 << 3) + 6);
            is_possible.value |= ((res.to_ullong() >> 3) & 1) << ((3 << 3) + 6);
            is_possible.value |= ((res.to_ullong() >> 4) & 1) << ((4 << 3) + 6);
            is_possible.value |= ((res.to_ullong() >> 5) & 1) << ((5 << 3) + 6);
            is_possible.value |= ((res.to_ullong() >> 6) & 1) << ((6 << 3) + 6);
            is_possible.value |= ((res.to_ullong() >> 7) & 1) << ((7 << 3) + 6);
        }
        {
            auto first_mask = (value_first >> 56) & ((1 << 8) - 1);
            auto second_mask = (value_second >> 56) & ((1 << 8) - 1);
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            is_possible.value |= ((res.to_ullong() >> 0) & 1) << ((0 << 3) + 7);
            is_possible.value |= ((res.to_ullong() >> 1) & 1) << ((1 << 3) + 7);
            is_possible.value |= ((res.to_ullong() >> 2) & 1) << ((2 << 3) + 7);
            is_possible.value |= ((res.to_ullong() >> 3) & 1) << ((3 << 3) + 7);
            is_possible.value |= ((res.to_ullong() >> 4) & 1) << ((4 << 3) + 7);
            is_possible.value |= ((res.to_ullong() >> 5) & 1) << ((5 << 3) + 7);
            is_possible.value |= ((res.to_ullong() >> 6) & 1) << ((6 << 3) + 7);
            is_possible.value |= ((res.to_ullong() >> 7) & 1) << ((7 << 3) + 7);
        }
        value_first = is_first_diagonal1.to_ullong();
        value_second = is_second_diagonal1.to_ullong();

        {
            uint64_t first_mask = (value_first & 68451041280) >> 28;
            uint64_t second_mask = (value_second & 68451041280) >> 28;
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            is_possible.value |= ((res.to_ullong() >> 0) & 1) << ((0 << 3) + 0);
            is_possible.value |= ((res.to_ullong() >> 1) & 1) << ((1 << 3) + 1);
            is_possible.value |= ((res.to_ullong() >> 2) & 1) << ((2 << 3) + 2);
            is_possible.value |= ((res.to_ullong() >> 3) & 1) << ((3 << 3) + 3);
            is_possible.value |= ((res.to_ullong() >> 4) & 1) << ((4 << 3) + 4);
            is_possible.value |= ((res.to_ullong() >> 5) & 1) << ((5 << 3) + 5);
            is_possible.value |= ((res.to_ullong() >> 6) & 1) << ((6 << 3) + 6);
            is_possible.value |= ((res.to_ullong() >> 7) & 1) << ((7 << 3) + 7);
        }
        {
            uint64_t first_mask = (value_first & 266338304) >> 21;
            uint64_t second_mask = (value_second & 266338304) >> 21;
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            is_possible.value |= ((res.to_ullong() >> 0) & 1) << ((0 << 3) + 1);
            is_possible.value |= ((res.to_ullong() >> 1) & 1) << ((1 << 3) + 2);
            is_possible.value |= ((res.to_ullong() >> 2) & 1) << ((2 << 3) + 3);
            is_possible.value |= ((res.to_ullong() >> 3) & 1) << ((3 << 3) + 4);
            is_possible.value |= ((res.to_ullong() >> 4) & 1) << ((4 << 3) + 5);
            is_possible.value |= ((res.to_ullong() >> 5) & 1) << ((5 << 3) + 6);
            is_possible.value |= ((res.to_ullong() >> 6) & 1) << ((6 << 3) + 7);
        }
        {
            uint64_t first_mask = (value_first & 2064384) >> 15;
            uint64_t second_mask = (value_second & 2064384) >> 15;
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            is_possible.value |= ((res.to_ullong() >> 0) & 1) << ((0 << 3) + 2);
            is_possible.value |= ((res.to_ullong() >> 1) & 1) << ((1 << 3) + 3);
            is_possible.value |= ((res.to_ullong() >> 2) & 1) << ((2 << 3) + 4);
            is_possible.value |= ((res.to_ullong() >> 3) & 1) << ((3 << 3) + 5);
            is_possible.value |= ((res.to_ullong() >> 4) & 1) << ((4 << 3) + 6);
            is_possible.value |= ((res.to_ullong() >> 5) & 1) << ((5 << 3) + 7);
        }
        {
            uint64_t first_mask = (value_first & 31744) >> 10;
            uint64_t second_mask = (value_second & 31744) >> 10;
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            is_possible.value |= ((res.to_ullong() >> 0) & 1) << ((0 << 3) + 3);
            is_possible.value |= ((res.to_ullong() >> 1) & 1) << ((1 << 3) + 4);
            is_possible.value |= ((res.to_ullong() >> 2) & 1) << ((2 << 3) + 5);
            is_possible.value |= ((res.to_ullong() >> 3) & 1) << ((3 << 3) + 6);
            is_possible.value |= ((res.to_ullong() >> 4) & 1) << ((4 << 3) + 7);
        }
        {
            uint64_t first_mask = (value_first & 960) >> 6;
            uint64_t second_mask = (value_second & 960) >> 6;
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            is_possible.value |= ((res.to_ullong() >> 0) & 1) << ((0 << 3) + 4);
            is_possible.value |= ((res.to_ullong() >> 1) & 1) << ((1 << 3) + 5);
            is_possible.value |= ((res.to_ullong() >> 2) & 1) << ((2 << 3) + 6);
            is_possible.value |= ((res.to_ullong() >> 3) & 1) << ((3 << 3) + 7);
        }
        {
            uint64_t first_mask = (value_first & 56) >> 3;
            uint64_t second_mask = (value_second & 56) >> 3;
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            is_possible.value |= ((res.to_ullong() >> 0) & 1) << ((0 << 3) + 5);
            is_possible.value |= ((res.to_ullong() >> 1) & 1) << ((1 << 3) + 6);
            is_possible.value |= ((res.to_ullong() >> 2) & 1) << ((2 << 3) + 7);
        }

        {
            uint64_t first_mask = (value_first & 8727373545472) >> 36;
            uint64_t second_mask = (value_second & 8727373545472) >> 36;
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            is_possible.value |= ((res.to_ullong() >> 0) & 1) << (((1) << 3) + 0);
            is_possible.value |= ((res.to_ullong() >> 1) & 1) << (((2) << 3) + 1);
            is_possible.value |= ((res.to_ullong() >> 2) & 1) << (((3) << 3) + 2);
            is_possible.value |= ((res.to_ullong() >> 3) & 1) << (((4) << 3) + 3);
            is_possible.value |= ((res.to_ullong() >> 4) & 1) << (((5) << 3) + 4);
            is_possible.value |= ((res.to_ullong() >> 5) & 1) << (((6) << 3) + 5);
            is_possible.value |= ((res.to_ullong() >> 6) & 1) << (((7) << 3) + 6);
        }
        {
            uint64_t first_mask = (value_first & 554153860399104) >> 43;
            uint64_t second_mask = (value_second & 554153860399104) >> 43;
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            is_possible.value |= ((res.to_ullong() >> 0) & 1) << (((2) << 3) + 0);
            is_possible.value |= ((res.to_ullong() >> 1) & 1) << (((3) << 3) + 1);
            is_possible.value |= ((res.to_ullong() >> 2) & 1) << (((4) << 3) + 2);
            is_possible.value |= ((res.to_ullong() >> 3) & 1) << (((5) << 3) + 3);
            is_possible.value |= ((res.to_ullong() >> 4) & 1) << (((6) << 3) + 4);
            is_possible.value |= ((res.to_ullong() >> 5) & 1) << (((7) << 3) + 5);
        }
        {
            uint64_t first_mask = (value_first & 17451448556060672) >> 49;
            uint64_t second_mask = (value_second & 17451448556060672) >> 49;
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            is_possible.value |= ((res.to_ullong() >> 0) & 1) << (((3) << 3) + 0);
            is_possible.value |= ((res.to_ullong() >> 1) & 1) << (((4) << 3) + 1);
            is_possible.value |= ((res.to_ullong() >> 2) & 1) << (((5) << 3) + 2);
            is_possible.value |= ((res.to_ullong() >> 3) & 1) << (((6) << 3) + 3);
            is_possible.value |= ((res.to_ullong() >> 4) & 1) << (((7) << 3) + 4);
        }
        {
            uint64_t first_mask = (value_first & 270215977642229760) >> 54;
            uint64_t second_mask = (value_second & 270215977642229760) >> 54;
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            is_possible.value |= ((res.to_ullong() >> 0) & 1) << (((4) << 3) + 0);
            is_possible.value |= ((res.to_ullong() >> 1) & 1) << (((5) << 3) + 1);
            is_possible.value |= ((res.to_ullong() >> 2) & 1) << (((6) << 3) + 2);
            is_possible.value |= ((res.to_ullong() >> 3) & 1) << (((7) << 3) + 3);
        }
        {
            uint64_t first_mask = (value_first & 2017612633061982208) >> 58;
            uint64_t second_mask = (value_second & 2017612633061982208) >> 58;
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            is_possible.value |= ((res.to_ullong() >> 0) & 1) << (((5) << 3) + 0);
            is_possible.value |= ((res.to_ullong() >> 1) & 1) << (((6) << 3) + 1);
            is_possible.value |= ((res.to_ullong() >> 2) & 1) << (((7) << 3) + 2);
        }
        value_first = is_first_diagonal2.to_ullong();
        value_second = is_second_diagonal2.to_ullong();
        {
            uint64_t first_mask = (value_first & 56) >> 3;
            uint64_t second_mask = (value_second & 56) >> 3;
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            is_possible.value |= ((res.to_ullong() >> 0) & 1) << ((0 << 3) + 2);
            is_possible.value |= ((res.to_ullong() >> 1) & 1) << ((1 << 3) + 1);
            is_possible.value |= ((res.to_ullong() >> 2) & 1) << ((2 << 3) + 0);
        }
        {
            uint64_t first_mask = (value_first & 960) >> 6;
            uint64_t second_mask = (value_second & 960) >> 6;
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            is_possible.value |= ((res.to_ullong() >> 0) & 1) << ((0 << 3) + 3);
            is_possible.value |= ((res.to_ullong() >> 1) & 1) << ((1 << 3) + 2);
            is_possible.value |= ((res.to_ullong() >> 2) & 1) << ((2 << 3) + 1);
            is_possible.value |= ((res.to_ullong() >> 3) & 1) << ((3 << 3) + 0);
        }
        {
            uint64_t first_mask = (value_first & 31744) >> 10;
            uint64_t second_mask = (value_second & 31744) >> 10;
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            is_possible.value |= ((res.to_ullong() >> 0) & 1) << ((0 << 3) + 4);
            is_possible.value |= ((res.to_ullong() >> 1) & 1) << ((1 << 3) + 3);
            is_possible.value |= ((res.to_ullong() >> 2) & 1) << ((2 << 3) + 2);
            is_possible.value |= ((res.to_ullong() >> 3) & 1) << ((3 << 3) + 1);
            is_possible.value |= ((res.to_ullong() >> 4) & 1) << ((4 << 3) + 0);
        }
        {
            uint64_t first_mask = (value_first & 2064384) >> 15;
            uint64_t second_mask = (value_second & 2064384) >> 15;
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            is_possible.value |= ((res.to_ullong() >> 0) & 1) << ((0 << 3) + 5);
            is_possible.value |= ((res.to_ullong() >> 1) & 1) << ((1 << 3) + 4);
            is_possible.value |= ((res.to_ullong() >> 2) & 1) << ((2 << 3) + 3);
            is_possible.value |= ((res.to_ullong() >> 3) & 1) << ((3 << 3) + 2);
            is_possible.value |= ((res.to_ullong() >> 4) & 1) << ((4 << 3) + 1);
            is_possible.value |= ((res.to_ullong() >> 5) & 1) << ((5 << 3) + 0);
        }
        {
            uint64_t first_mask = (value_first & 266338304) >> 21;
            uint64_t second_mask = (value_second & 266338304) >> 21;
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            is_possible.value |= ((res.to_ullong() >> 0) & 1) << ((0 << 3) + 6);
            is_possible.value |= ((res.to_ullong() >> 1) & 1) << ((1 << 3) + 5);
            is_possible.value |= ((res.to_ullong() >> 2) & 1) << ((2 << 3) + 4);
            is_possible.value |= ((res.to_ullong() >> 3) & 1) << ((3 << 3) + 3);
            is_possible.value |= ((res.to_ullong() >> 4) & 1) << ((4 << 3) + 2);
            is_possible.value |= ((res.to_ullong() >> 5) & 1) << ((5 << 3) + 1);
            is_possible.value |= ((res.to_ullong() >> 6) & 1) << ((6 << 3) + 0);
        }
        {
            uint64_t first_mask = (value_first & 68451041280) >> 28;
            uint64_t second_mask = (value_second & 68451041280) >> 28;
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            is_possible.value |= ((res.to_ullong() >> 0) & 1) << ((0 << 3) + 7);
            is_possible.value |= ((res.to_ullong() >> 1) & 1) << ((1 << 3) + 6);
            is_possible.value |= ((res.to_ullong() >> 2) & 1) << ((2 << 3) + 5);
            is_possible.value |= ((res.to_ullong() >> 3) & 1) << ((3 << 3) + 4);
            is_possible.value |= ((res.to_ullong() >> 4) & 1) << ((4 << 3) + 3);
            is_possible.value |= ((res.to_ullong() >> 5) & 1) << ((5 << 3) + 2);
            is_possible.value |= ((res.to_ullong() >> 6) & 1) << ((6 << 3) + 1);
            is_possible.value |= ((res.to_ullong() >> 7) & 1) << ((7 << 3) + 0);
        }
        for (int32_t row = 1; row < 6; ++row) {
            uint64_t val = row_diag2[row];
            uint64_t first_mask = (value_first & val) >> offsets_row_diag2[row];
            uint64_t second_mask = (value_second & val) >> offsets_row_diag2[row];
            auto res = precalced_check_line[(first_mask << 8) + second_mask];
            for (int32_t i = 0; i < 8 - row; ++i) {
                is_possible.value |= ((res.to_ullong() >> i) & 1) << (((row + i) << 3) + 7 - i);
            }
        }

        BitsetToVector(is_possible, result);
    }

    bool Board::GameEnded() const {
        return (PossibleMoves().empty() && (MakeMove(Cell{-1, -1})).PossibleMoves().empty());
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
            res += precalced_row_costs[(row == 0 || row == 7) ? 0 : 1]
                                      [(first_mask << 8) + second_mask];
        }
        return res;
    }

    Board Board::MakeMoveLast(const Cell& cell) const {
        int32_t col = cell.col;
        int32_t row = cell.row;
        Board board = *this;
        if (col == -1 && row == -1) {
            std::swap(board.is_first_, board.is_second_);
            std::swap(board.is_first_vertical, board.is_second_vertical);
            std::swap(board.is_first_diagonal1, board.is_second_diagonal1);
            std::swap(board.is_first_diagonal2, board.is_second_diagonal2);
            board.player_ = (player_ == Player::First ? Player::Second : Player::First);
            return board;
        }
        {
            {
                int shift = 8 * row;
                auto first_mask = (is_first_.to_ullong() >> shift) & ((1 << 8) - 1);
                auto second_mask = (is_second_.to_ullong() >> shift) & ((1 << 8) - 1);
                auto x = precalced_captures[col][(first_mask << 8) + second_mask];
                board.is_first_ |= Bitset64(x.to_ullong() << shift);
            }
            {
                int shift = 8 * col;
                auto first_mask = (is_first_vertical.to_ullong() >> shift) & ((1 << 8) - 1);
                auto second_mask = (is_second_vertical.to_ullong() >> shift) & ((1 << 8) - 1);
                auto res = precalced_captures[row][(first_mask << 8) + second_mask];
                for (uint64_t i = res._Find_first(); i < res._Find_last(); ++i) {
                    board.is_first_[(i << 3) + col] = true;
                }
            }
            {
                if (col >= row) {
                    int32_t id = col - row;
                    uint64_t val = col_diag1[id];
                    uint64_t first_mask =
                            (is_first_diagonal1.to_ullong() & val) >> offsets_col_diag1[id];
                    uint64_t second_mask =
                            (is_second_diagonal1.to_ullong() & val) >> offsets_col_diag1[id];
                    auto res = precalced_captures[row][(first_mask << 8) + second_mask];
                    for (uint64_t i = res._Find_first(); i < res._Find_last(); ++i) {
                        board.is_first_[(i << 3) + id + i] = true;
                    }
                } else {
                    int32_t id = row - col;
                    uint64_t val = row_diag1[id];
                    uint64_t first_mask =
                            (is_first_diagonal1.to_ullong() & val) >> offsets_row_diag1[id];
                    uint64_t second_mask =
                            (is_second_diagonal1.to_ullong() & val) >> offsets_row_diag1[id];
                    auto res = precalced_captures[col][(first_mask << 8) + second_mask];
                    for (uint64_t i = res._Find_first(); i < res._Find_last(); ++i) {
                        board.is_first_[((i + id) << 3) + i] = true;
                    }
                }
            }
            {
                if (col + row <= 7) {
                    int32_t id = col + row;
                    uint64_t val = col_diag2[id];
                    uint64_t first_mask =
                            (is_first_diagonal2.to_ullong() & val) >> offsets_col_diag2[id];
                    uint64_t second_mask =
                            (is_second_diagonal2.to_ullong() & val) >> offsets_col_diag2[id];

                    auto res = precalced_captures[row][(first_mask << 8) + second_mask];
                    for (uint64_t i = res._Find_first(); i < res._Find_last(); ++i) {
                        board.is_first_[(i << 3) + id - i] = true;
                    }
                } else {
                    int32_t id = col + row - 7;
                    uint64_t val = row_diag2[id];
                    uint64_t first_mask =
                            (is_first_diagonal2.to_ullong() & val) >> offsets_row_diag2[id];
                    uint64_t second_mask =
                            (is_second_diagonal2.to_ullong() & val) >> offsets_row_diag2[id];
                    auto res = precalced_captures[row - id][(first_mask << 8) + second_mask];
                    for (uint64_t i = res._Find_first(); i < res._Find_last(); ++i) {
                        board.is_first_[((id + i) << 3) + 7 - i] = true;
                    }
                }
            }
        }
        board.PlacePieceLast(cell.ToInt());
        board.DeletePiecesLast();
        std::swap(board.is_first_, board.is_second_);
        board.player_ = (player_ == Player::First ? Player::Second : Player::First);
        return board;
    }

}// namespace ReversiEngine
