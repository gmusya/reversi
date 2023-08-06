#include "board.h"
#include "engine.h"
#include "time_wrapper.h"

#include <atomic>
#include <iostream>
#include <thread>

namespace ReversiEngine {

    namespace {
        void ReadAndDoMove(Board& board) {
            while (true) {
                std::cout << "Print your move: ";
                std::cout.flush();
                std::string str;
                getline(std::cin, str);
                if (str == "/Leave") {
                    exit(0);
                }
                if (str == "/Skip") {
                    if (!board.PossibleMoves().empty()) {
                        std::cout << "Incorrect move" << std::endl;
                    } else {
                        board = board.MakeMove(Cell{-1, -1});
                        break;
                    }
                }
                if (str.size() != 2) {
                    std::cout << "Incorrect move" << std::endl;
                } else {
                    int32_t col = str[0] - 'a';
                    int32_t row = str[1] - '1';
                    if (0 <= col && col <= 7 && 0 <= row && row <= 7) {
                        Cell cell{row, col};
                        auto moves = board.PossibleMoves();
                        if (std::find(moves.begin(), moves.end(), cell) == moves.end()) {
                            std::cout << "Incorrect move" << std::endl;
                        } else {
                            board = board.MakeMove(cell);
                            return;
                        }
                    } else {
                        std::cout << "Incorrect move" << std::endl;
                    }
                }
            }
        }
    }// namespace

    Cell BestMoveForSecond(Board board) {
        Time total_time(0);
        int32_t depth = 2;
        std::atomic<Cell> result{};
        Engine engine;
        auto foo = [&]() {
            while (depth < 32) {
                ++depth;
                auto [best_move_info, time] =
                        MeasureMethod(engine, &Engine::GetBestMove, board, depth);
                if (engine.stop) {
                    break;
                }
                result = best_move_info.first;
                int32_t evaluation = best_move_info.second;
                total_time += time;
                auto nodes_per_sec = static_cast<int64_t>(static_cast<double>(engine.nodes) /
                                                          total_time.seconds);
                std::cout << "[depth=" << depth << ", eval=" << evaluation << "]"
                          << ": " << result << " (" << total_time << ", " << engine.nodes
                          << " nodes, " << nodes_per_sec << " nodes/sec"
                          << ")" << std::endl;
            }
        };
        std::jthread th(foo);
        sleep(1);
        engine.stop = true;
        return result;
    }

    void StartGame(Player player) {
        Board board;
        board.InitPrecalc();
        if (player == First) {
            std::cout << board << std::endl;
            ReadAndDoMove(board);
        }
        while (!board.GameEnded()) {
            board = board.MakeMove(BestMoveForSecond(board));
            std::cout << board << std::endl;
            ReadAndDoMove(board);
            std::cout << board << std::endl;
        }
        int32_t result = board.FinalEvaluation();
        if (result == 0) {
            std::cout << ("Draw") << std::endl;
        } else if ((result > 0) ^ (board.MySymbol() == 'o')) {
            std::cout << ("First player (x) won") << std::endl;
        } else {
            std::cout << ("Second player (o) won") << std::endl;
        }
    }

}// namespace ReversiEngine

int main() {
    std::cout << "Are you playing first (yes/no)?" << std::endl;
    std::string str;
    while (str != "yes" && str != "no") {
        std::getline(std::cin, str);
        if (str != "yes" && str != "no") {
            std::cout << "Print (yes/no)" << std::endl;
        }
    }
    if (str == "yes") {
        ReversiEngine::StartGame(ReversiEngine::First);
    } else {
        ReversiEngine::StartGame(ReversiEngine::Second);
    }
    return 0;
}
