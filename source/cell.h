#pragma once

#include <cstdint>
#include <iostream>

namespace ReversiEngine {

    struct Cell {
        int32_t row;
        int32_t col;

        friend std::ostream& operator<<(std::ostream& out, const Cell& cell) {
            if (cell.row == -1) {
                return out << "Skip";
            }
            return out << static_cast<char>(cell.col + 'a') << static_cast<char>(cell.row + '1');
        }

        bool operator==(const Cell& other) const {
            return row == other.row && col == other.col;
        }

        [[nodiscard]] int32_t ToInt() const {
            return (row << 3) + col;
        }
    };

}// namespace ReversiEngine
