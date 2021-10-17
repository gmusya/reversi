#pragma once

#include <fstream>

#include "Board.h"
#include "Writer.h"

using std::pair;
using std::string;
using std::vector;

class MoveParser {
public:
    explicit MoveParser(const Writer& writer);

    bool ParseMove(const string& str, Board& board);

private:
    void Help() const;

    void RecommendHelp() const;

    Writer writer_;
};
