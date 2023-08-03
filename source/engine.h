#pragma once

#include "board.h"
#include <atomic>

namespace ReversiEngine {

    class Engine {
    public:
        [[nodiscard]] std::pair<ReversiEngine::Cell, int32_t> GetBestMove(Board board,
                                                                          int32_t depth) const;

        [[nodiscard]] int32_t SmartEvaluation(Board board, int32_t depth, int32_t alpha,
                                              int32_t beta) const;

        mutable int64_t nodes = 0;
        std::atomic<bool> stop;
    };
}// namespace ReversiEngine
