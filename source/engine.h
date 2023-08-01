#pragma once

#include "board.h"

namespace ReversiEngine {

    class Engine {
    public:
        [[nodiscard]] Cell GetBestMove(Board board, int32_t depth) const;

        [[nodiscard]] int32_t SmartEvaluation(Board board, int32_t depth, int32_t alpha,
                                              int32_t beta) const;
    };
}// namespace ReversiEngine
