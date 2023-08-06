#include "board.h"

#include <array>

std::vector<std::bitset<8>> precalced_check_line(1 << 16);

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

    void Board::InitPrecalc() const {
        for (int32_t mask_first = 0; mask_first < (1 << 8); ++mask_first) {
            for (int32_t mask_second = 0; mask_second < (1 << 8); ++mask_second) {
                if (mask_first & mask_second) {
                    continue;
                }
                std::bitset<8> is_first(mask_first);
                std::bitset<8> is_second(mask_second);
                std::bitset<8> is_possible;
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
                std::bitset<8> is_first(mask_first);
                std::bitset<8> is_second(mask_second);
                std::bitset<8> is_possible;
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
        if (is_first_[position]) {
            eval -= CELL_COST[position >> 3][position & 7];
        }
        if (is_second_[position]) {
            eval += CELL_COST[position >> 3][position & 7];
        }
        Same(player)[position] = true;
        Opposite(player)[position] = false;
        if (is_first_[position]) {
            eval += CELL_COST[position >> 3][position & 7];
        }
        if (is_second_[position]) {
            eval -= CELL_COST[position >> 3][position & 7];
        }
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
        std::bitset<64> captured;
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

    inline void Board::CheckLine(Cell first, int drow, int dcol, std::bitset<64>& is_possible,
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
        void BitsetToVector(const std::bitset<64>& is_possible, std::vector<Cell>& result) {
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

    void Board::PossibleMovesHorizontal(std::bitset<64>& is_possible) const {
        auto first_set = Same(player_);
        auto second_set = Opposite(player_);
        auto first = first_set.to_ullong();
        auto second = second_set.to_ullong();
        for (int32_t row = 0; row < 8; ++row) {
            int shift = 8 * row;
            auto first_mask = (first >> shift) & ((1 << 8) - 1);
            auto second_mask = (second >> shift) & ((1 << 8) - 1);
            auto x = precalced_check_line[(first_mask << 8) + second_mask];
            is_possible |= std::bitset<64>(x.to_ullong() << shift);
        }
    }

    void Board::PossibleMovesVertical(std::bitset<64>& is_possible) const {
        auto first_set = Same(player_);
        auto second_set = Opposite(player_);
        for (int32_t col = 0; col < 8; ++col) {
            std::bitset<8> bs_first;
            std::bitset<8> bs_second;
            for (int32_t i = 0; i < 8; ++i) {
                bs_first[i] = first_set[(i << 3) + col];
                bs_second[i] = second_set[(i << 3) + col];
            }
            auto first_mask = bs_first.to_ullong();
            auto second_mask = bs_second.to_ullong();
            auto x = precalced_check_line[(first_mask << 8) + second_mask];
            for (int32_t i = 0; i < 8; ++i) {
                if (x[i]) {
                    is_possible[(i << 3) + col] = true;
                }
            }
        }
    }

    void Board::PossibleMovesDiagonal(std::bitset<64>& is_possible) const {
        auto first_set = Same(player_);
        auto second_set = Opposite(player_);
        std::bitset<8> bs_first;
        std::bitset<8> bs_second;
        for (int32_t col = 0; col < 6; ++col) {
            bs_first = 0;
            bs_second = 0;
            for (int32_t i = 0; i < 8 - col; ++i) {
                bs_first[i] = first_set[(i << 3) + col + i];
                bs_second[i] = second_set[(i << 3) + col + i];
            }
            auto first_mask = bs_first.to_ullong();
            auto second_mask = bs_second.to_ullong();
            auto x = precalced_check_line[(first_mask << 8) + second_mask];
            for (int32_t i = 0; i < 8 - col; ++i) {
                if (x[i]) {
                    is_possible[(i << 3) + col + i] = true;
                }
            }
        }

        for (int32_t row = 1; row < 6; ++row) {
            bs_first = 0;
            bs_second = 0;
            for (int32_t i = 0; i < 8 - row; ++i) {
                bs_first[i] = first_set[((i + row) << 3) + i];
                bs_second[i] = second_set[((i + row) << 3) + i];
            }
            auto first_mask = bs_first.to_ullong();
            auto second_mask = bs_second.to_ullong();
            auto x = precalced_check_line[(first_mask << 8) + second_mask];
            for (int32_t i = 0; i < 8 - row; ++i) {
                if (x[i]) {
                    is_possible[((i + row) << 3) + i] = true;
                }
            }
        }
        for (int32_t col = 2; col < 8; ++col) {
            bs_first = 0;
            bs_second = 0;
            for (int32_t i = 0; i < col + 1; ++i) {
                bs_first[i] = first_set[(i << 3) + col - i];
                bs_second[i] = second_set[(i << 3) + col - i];
            }
            auto first_mask = bs_first.to_ullong();
            auto second_mask = bs_second.to_ullong();
            auto x = precalced_check_line[(first_mask << 8) + second_mask];
            for (int32_t i = 0; i < col + 1; ++i) {
                if (x[i]) {
                    is_possible[(i << 3) + col - i] = true;
                }
            }
        }
        for (int32_t row = 1; row < 6; ++row) {
            bs_first = 0;
            bs_second = 0;
            for (int32_t i = 0; i < 8 - row; ++i) {
                bs_first[i] = first_set[((row + i) << 3) + 7 - i];
                bs_second[i] = second_set[((row + i) << 3) + 7 - i];
            }
            auto first_mask = bs_first.to_ullong();
            auto second_mask = bs_second.to_ullong();
            auto x = precalced_check_line[(first_mask << 8) + second_mask];
            for (int32_t i = 0; i < 8 - row; ++i) {
                if (x[i]) {
                    is_possible[((row + i) << 3) + 7 - i] = true;
                }
            }
        }
    }

    void Board::PossibleMoves(std::vector<Cell>& result) const {
        std::bitset<64> is_possible;
        PossibleMovesHorizontal(is_possible);
        PossibleMovesVertical(is_possible);
        PossibleMovesDiagonal(is_possible);

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

    std::bitset<64> Board::GetCaptures(int32_t row, int32_t col, int32_t drow, int32_t dcol) const {
        std::bitset<64> res;
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

    std::vector<Cell> Board::PossibleMoves() const {
        std::vector<Cell> result;
        PossibleMoves(result);
        return result;
    }

    int32_t Board::FinalEvaluation() const {
        return (player_ == First ? eval : -eval);
    }

}// namespace ReversiEngine
