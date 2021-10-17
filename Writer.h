#pragma once

#include <fstream>
#include <vector>
#include <string>

#include "Cell.h"

using std::vector;

class Writer {
public:
    Writer(std::ostream& input_stream);

    void Print(const vector<vector<char>>& board) const;

    void Print(const Cell& cell) const;

    void Print(const std::string& string) const;

    void Print(int32_t value) const;

private:
    std::ostream& stream_;
};


