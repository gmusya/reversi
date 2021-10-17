#include "Cell.h"

Cell::Cell(const int32_t& x1, const int32_t& y1) {
    x = x1;
    y = y1;
}

Cell::Cell(const Cell& other) {
    x = other.x;
    y = other.y;
}
