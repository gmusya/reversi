#include "engine.h"

const int INF = 10000;

ReversiEngine::Cell ReversiEngine::Engine::GetBestMove(ReversiEngine::Board board,
                                                       int32_t depth) const {
    int32_t value = -INF;
    int32_t alpha = -INF;
    int32_t beta = INF;
    Cell best_move{-1, -1};
    std::vector<Cell> possible_moves = board.PossibleMoves();
    if (possible_moves.empty()) {
        return best_move;
    } else {
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
    }
    return best_move;
}

int32_t ReversiEngine::Engine::SmartEvaluation(ReversiEngine::Board board, int32_t depth,
                                               int32_t alpha, int32_t beta) const {
    if (depth == 0) {
        return board.FinalEvaluation();
    }
    int32_t value = -INF;
    std::vector<Cell> possible_moves = board.PossibleMoves();
    if (possible_moves.empty()) {
        Board new_board = board.MakeMove(Cell{-1, -1});
        return -SmartEvaluation(new_board, depth - 1, -beta, -alpha);
    } else {
        for (const Cell& cell : possible_moves) {
            Board new_board = board.MakeMove(cell);
            int32_t candidate_value = -SmartEvaluation(new_board, depth - 1, -beta, -alpha);
            if (value < candidate_value) {
                value = candidate_value;
            }
            if (alpha < value) {
                alpha = value;
            }
            if (value > beta) {
                return value;
            }
        }
    }
    return value;
}
