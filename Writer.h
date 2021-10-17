#pragma once

#include <fstream>
#include <vector>

#include "Cell.h"

using std::vector;

class Writer {
public:
    Writer(std::ostream& input_stream);
    void Print(const vector<vector<char>>& board) const;
    void Print(const Cell& cell) const;

private:
    std::ostream& stream_;
};


