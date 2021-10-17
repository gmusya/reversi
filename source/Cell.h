#pragma once

#include <cstdint>

struct Cell {
    Cell(const int32_t& x1, const int32_t& y1);

    Cell(const Cell& other);

    Cell(Cell&& other) = default;

    Cell& operator=(const Cell& other) = default;

    int32_t x;
    int32_t y;
};