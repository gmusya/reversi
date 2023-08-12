#include "engine.h"

#include <algorithm>

namespace ReversiEngine {

    const int INF = 10000;

    std::pair<ReversiEngine::Cell, int32_t>
    ReversiEngine::Engine::GetBestMove(const ReversiEngine::Board& board, int32_t depth) const {
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
        std::sort(buffer.begin(), buffer.end(), [](auto& lhs, auto& rhs) {
            return lhs.second < rhs.second;
        });
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

    int32_t ReversiEngine::Engine::SmartEvaluation(const Board& board, int32_t depth, int32_t alpha,
                                                   int32_t beta) const {
        ++nodes;
        if (stop) {
            return -INF;
        }
        if (depth == 0) {
            return board.FinalEvaluation();
        }
        int32_t value = -INF;

        std::vector<Cell>& possible_moves = buffers[depth];
        board.PossibleMoves(possible_moves);
        if (possible_moves.empty()) {
            Board new_board = board.MakeMove(Cell{-1, -1});
            return -SmartEvaluation(new_board, depth - 1, -beta, -alpha);
        }

        if (depth >= 3) {
            auto& boards = buffers3[depth];
            boards.resize(possible_moves.size());
            for (size_t i = 0; i < possible_moves.size(); ++i) {
                boards[i] = board.MakeMove(possible_moves[i]);
            }
            auto& buffer = buffers2[depth];
            buffer.resize(possible_moves.size());
            for (size_t i = 0; i < possible_moves.size(); ++i) {
                buffer[i] = {i, boards[i].FinalEvaluation()};
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
                int32_t candidate_value =
                        -SmartEvaluation(boards[buffer[i].first], depth - 1, -beta, -alpha);
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
        for (auto cell : possible_moves) {
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
