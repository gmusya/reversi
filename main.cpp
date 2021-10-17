#include <iostream>

#include "Board.h"
#include "MoveParser.h"

void StartGame() {
    Board board(1);
    Writer writer(std::cout);
    MoveParser move_parser(writer);

    bool game_ended = false;
    while (!game_ended) {
        board = board.MakeMove(board.GetBestMove(board.GetDepth()));
        board.PrintMoveBoard(writer);
        std::string str;
        do {
            getline(std::cin, str);
            if (str == "/Leave") {
                game_ended = true;
            }
        } while (!move_parser.ParseMove(str, board));
        board.PrintMoveBoard(writer);
    }
}

int main() {
    StartGame();
    return 0;
}