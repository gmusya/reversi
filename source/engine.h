#pragma once

#include "board.h"
#include <atomic>

namespace ReversiEngine {

    class Engine {
    public:
        Engine() {
            buffers.resize(100);
            for (auto& now : buffers) {
                now.reserve(64);
            }
            buffers2.resize(100);
            for (auto& now : buffers2) {
                now.reserve(100);
            }
            buffers3.resize(100);
            for (auto& now : buffers3) {
                now.reserve(100);
            }
        }

        [[nodiscard]] std::pair<ReversiEngine::Cell, int32_t> GetBestMove(const Board& board,
                                                                          int32_t depth) const;

        [[nodiscard]] int32_t SmartEvaluation(const Board& board, int32_t depth, int32_t alpha,
                                              int32_t beta) const;

        mutable std::vector<std::vector<Cell>> buffers;
        mutable std::vector<std::vector<std::pair<std::int32_t, std::int32_t>>> buffers2;
        mutable std::vector<std::vector<Board>> buffers3;
        mutable int64_t nodes = 0;
        std::atomic<bool> stop;
    };
}// namespace ReversiEngine
