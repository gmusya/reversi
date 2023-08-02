#include "engine.h"

#include <algorithm>
#include <cassert>

namespace ReversiEngine {

    const int INF = 10000;

    std::pair<ReversiEngine::Cell, int32_t>
    ReversiEngine::Engine::GetBestMove(ReversiEngine::Board board, int32_t depth) const {
        int32_t value = -INF;
        int32_t alpha = -INF;
        int32_t beta = INF;
        Cell best_move{-1, -1};
        std::vector<Cell> possible_moves = board.PossibleMoves();
        if (possible_moves.empty()) {
            value = -SmartEvaluation(board.MakeMove({-1, -1}), depth - 1, -beta, -alpha);
            return {best_move, value};
        }
        if (depth >= 4) {
            std::sort(possible_moves.begin(), possible_moves.end(), [board](auto& lhs, auto& rhs) {
                return board.MakeMove(lhs).FinalEvaluation() <
                       board.MakeMove(rhs).FinalEvaluation();
            });
        }
        for (const Cell& cell : possible_moves) {
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
        static int counter = 0;
        ++counter;
        if ((counter & 64) == 0 && stop) {
            return -INF;
        }
        if (depth == 0) {
            return board.FinalEvaluation();
        }
        int32_t value = -INF;
        std::vector<Cell> possible_moves = board.PossibleMoves();
//        {
//            auto old_moves = board.OldPossibleMoves();
//            assert(possible_moves == old_moves);
//        }
        if (possible_moves.empty()) {
            Board new_board = board.MakeMove(Cell{-1, -1});
            return -SmartEvaluation(new_board, depth - 1, -beta, -alpha);
        } else {
            if (depth >= 4) {
                std::sort(possible_moves.begin(), possible_moves.end(),
                          [board](auto& lhs, auto& rhs) {
                              return board.MakeMove(lhs).FinalEvaluation() <
                                     board.MakeMove(rhs).FinalEvaluation();
                          });
            }
            for (const Cell& cell : possible_moves) {
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
        }
        return value;
    }

}// namespace ReversiEngine
