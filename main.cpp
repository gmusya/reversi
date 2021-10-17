#include <iostream>

#include "Board.h"
#include "Writer.h"

void StartGame() {
    Board board;
    Writer writer(std::cout);
    while (true) {
        Cell cell = board.GetBestMove(7);
        writer.Print(cell);
        board = board.MakeMove(cell);
        char x;
        char y;
        std::cin >> x >> y;
        if (x == '-' && y == '-') {
            continue;
        }
        if (x == 's' && y == 's') {
            break;
        }
        board = board.MakeMove(Cell(x - 'a', y - '1'));
    }
}

int main(int argc, char* argv[]) {
    StartGame();
    return 0;
}