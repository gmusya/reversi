#include "engine.h"

#include <algorithm>

namespace ReversiEngine {

    const int INF = 10000;

    std::pair<ReversiEngine::Cell, int32_t>
    ReversiEngine::Engine::GetBestMove(ReversiEngine::Board board, int32_t depth) const {
        ++nodes;
        int32_t value = -INF;
        int32_t alpha = -INF;
        int32_t beta = INF;
        Cell best_move{-1, -1};
        std::vector<Cell>& possible_moves = buffers[depth];
        board.PossibleMoves(possible_moves);
        if (possible_moves.empty()) {
            value = -SmartEvaluation(board.MakeMove({-1, -1}), depth - 1, -beta, -alpha);
            return {best_move, value};
        }
        auto& buffer = buffers2[depth];
        buffer.resize(possible_moves.size());
        for (size_t i = 0; i < possible_moves.size(); ++i) {
            buffer[i] = {i, board.MakeMove(possible_moves[i]).FinalEvaluation()};
        }
        if (depth >= 4) {
            std::sort(buffer.begin(), buffer.end(), [](auto& lhs, auto& rhs) {
                return lhs.second < rhs.second;
            });
        }
        for (size_t i = 0; i < possible_moves.size(); ++i) {
            const Cell& cell = possible_moves[buffer[i].first];
            Board new_board = board.MakeMove(cell);
            int32_t candidate_value = -SmartEvaluation(new_board, depth - 1, -beta, -alpha);
            if (value < candidate_value) {
                value = candidate_value;
                best_move = cell;
            }
            if (alpha < value) {
                alpha = value;
            }
        }
        return {best_move, value};
    }

    int32_t ReversiEngine::Engine::SmartEvaluation(ReversiEngine::Board board, int32_t depth,
                                                   int32_t alpha, int32_t beta) const {
        ++nodes;
        static int counter = 0;
        ++counter;
        if ((counter & 64) == 0 && stop) {
            return -INF;
        }
        if (depth == 0) {
            return board.FinalEvaluation();
        }
        int32_t value = -INF;

        std::vector<Cell>& possible_moves = buffers[depth];
        board.PossibleMoves(possible_moves);
        //        {
        //            auto old_moves = board.OldPossibleMoves();
        //            assert(possible_moves == old_moves);
        //        }
        if (possible_moves.empty()) {
            Board new_board = board.MakeMove(Cell{-1, -1});
            return -SmartEvaluation(new_board, depth - 1, -beta, -alpha);
        }
        if (depth >= 3) {
            auto& buffer = buffers2[depth];
            buffer.resize(possible_moves.size());
            for (size_t i = 0; i < possible_moves.size(); ++i) {
                buffer[i] = {i, board.MakeMove(possible_moves[i]).FinalEvaluation()};
            }
            if (buffer.size() >= 4) {
                std::nth_element(buffer.begin(), buffer.begin() + 4, buffer.end(),
                                 [](auto& lhs, auto& rhs) {
                                     return lhs.second < rhs.second;
                                 });
                std::sort(buffer.begin(), buffer.begin() + 4, [](auto& lhs, auto& rhs) {
                    return lhs.second < rhs.second;
                });
            } else {
                std::sort(buffer.begin(), buffer.end(), [](auto& lhs, auto& rhs) {
                    return lhs.second < rhs.second;
                });
            }
            for (size_t i = 0; i < possible_moves.size(); ++i) {
                const Cell& cell = possible_moves[buffer[i].first];
                Board new_board = board.MakeMove(cell);
                int32_t candidate_value = -SmartEvaluation(new_board, depth - 1, -beta, -alpha);
                if (value < candidate_value) {
                    value = candidate_value;
                }
                if (alpha < value) {
                    alpha = value;
                }
                if (value >= beta) {
                    //                    std::cout << i + 1 << "/" << possible_moves.size() << std::endl;
                    return value;
                }
            }
            return value;
        }
        for (size_t i = 0; i < possible_moves.size(); ++i) {
            const Cell& cell = possible_moves[i];
            Board new_board = board.MakeMove(cell);
            int32_t candidate_value = -SmartEvaluation(new_board, depth - 1, -beta, -alpha);
            if (value < candidate_value) {
                value = candidate_value;
            }
            if (alpha < value) {
                alpha = value;
            }
            if (value >= beta) {
                return value;
            }
        }
        return value;
    }

}// namespace ReversiEngine
