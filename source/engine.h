#pragma once

#include "board.h"

namespace ReversiEngine {

    struct EngineSettings {
        int32_t depth = 7;
    };

    class Engine {
    public:
        [[nodiscard]] Cell GetBestMove(Board board, int32_t depth) const;

        [[nodiscard]] int32_t SmartEvaluation(Board board, int32_t depth, int32_t alpha,
                                              int32_t beta) const;

        EngineSettings& Settings() {
            return settings_;
        }

    private:
        EngineSettings settings_;
    };
}// namespace ReversiEngine
