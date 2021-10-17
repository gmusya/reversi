#include <iostream>

#include "ArgumentParser.h"
#include "Board.h"
#include "MoveParser.h"

void StartGame(ArgumentParser& argument_parser) {
	Board board(argument_parser.GetDepth());
	Writer writer(std::cout);
	MoveParser move_parser(writer);
	if (argument_parser.GetOrder()==1) {
		std::string str;
		do {
			getline(std::cin, str);
			if (str == "/Leave") {
				return;
			}
		} while (!move_parser.ParseMove(str, board));
		board.PrintMoveBoard(writer);
	}
	while (!board.GameEnded()) {
		board = board.MakeMove(board.GetBestMove(board.GetDepth()));
		board.PrintMoveBoard(writer);
		std::string str;
		do {
			getline(std::cin, str);
			if (str == "/Leave") {
				return;
			}
		} while (!move_parser.ParseMove(str, board));
		board.PrintMoveBoard(writer);
	}
	int32_t result = board.FinalEvaluation();
	if (result == 0) {
		writer.Print("Draw");
	} else if ((result > 0) ^ (board.MySymbol() == 'o')) {
		writer.Print("First player (x) won");
	} else {
		writer.Print("Second player (o) won");
	}
}

int main(int argc, char* argv[]) {
	ArgumentParser argument_parser(argc, argv);
	if (argument_parser.GetAction() == ArgumentParser::Action::Unknown) {
		std::cout << "To get help use command \"reversi -h\"";
		return 0;
	} else if (argument_parser.GetAction() == ArgumentParser::Action::Help) {
		argument_parser.PrintHelp();
	} else {
		StartGame(argument_parser);
	}
    return 0;
}
