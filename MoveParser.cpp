#include "MoveParser.h"

namespace {
    string substring(const string& str, size_t l, size_t r) {
        string res;
        for (size_t i = l; i < r; ++i) {
            res += str[i];
        }
        return res;
    }
}

MoveParser::MoveParser(const Writer& writer) : writer_(writer) {
}

bool MoveParser::ParseMove(const string& str, Board& board) {
    if (str == "/Leave") {
        return true;
    }
    if (str.size() == 2) {
        int32_t col = str[0] - 'a';
        int32_t row = str[1] - '1';
        if (0 <= col && col <= 7 && 0 <= row && row <= 7) {
            board = board.MakeMove(Cell(col, row));
            return true;
        } else {
            RecommendHelp();
        }
    }
    if (str == "/Help") {
        Help();
    } else if (str == "/BestMove") {
        writer_.Print(board.GetBestMove(board.GetDepth()));
    } else if (str == "/PrintEvaluation") {
        writer_.Print(board.SmartEvaluation(board.GetDepth(), -64, 64));
    } else if (substring(str, 0, 5) == "/Skip" && substring(str, 5, 7) == " 0" && str.size() == 7) {
        if (board.PossibleMoves().empty()) {
            board = board.MakeMove(Cell(-1, -1));
        } else {
            writer_.Print("There are legal moves");
        }
    } else if (substring(str, 0, 5) == "/Skip" && substring(str, 5, 7) == " 1" && str.size() == 7) {
        board = board.MakeMove(Cell(-1, -1));
    } else if (str.size() == 12 && substring(str, 0, 10) == "/PrintMove") {
        if (substring(str, 10, 12) == " 0") {
            board.PrintMoveMode() = false;
        } else if (substring(str, 10, 12) == " 1") {
            board.PrintMoveMode() = true;
        } else {
            RecommendHelp();
        }
    } else if (str.size() == 13 && substring(str, 0, 11) == "/PrintBoard") {
        if (substring(str, 10, 12) == " 0") {
            board.PrintBoardMode() = false;
        } else if (substring(str, 10, 12) == " 1") {
            board.PrintBoardMode() = true;
        } else {
            RecommendHelp();
        }
    } else if (str.size() == 8 && substring(str, 0, 6) == "/Depth" && str[6] == ' ' && '1' <= str[7] && str[7] <= '9') {
        board.ChangeDepth(str[7] - '0');
    } else {
        RecommendHelp();
    }
    return false;
}

void MoveParser::RecommendHelp() const {
    writer_.Print("> To get help print /Help");
}

void MoveParser::Help() const {
    writer_.Print(R"(> To get help use command /Help)");                                            // DONE
    writer_.Print(R"(> To make move print [col][row], where col is a..h and row is 1..8)");         // DONE
    writer_.Print(R"(> To get best move used command /BestMove)");                                  // DONE
    writer_.Print(R"(> To disable printing table use command /PrintBoard 0)");                      // DONE
    writer_.Print(R"(> To enable printing table use command /PrintBoard 1)");                       // DONE
    writer_.Print(R"(> To disable printing last move use command /PrintMove 0)");                   // DONE
    writer_.Print(R"(> To enable printing last move use command /PrintMove1)");                     // DONE
    writer_.Print(R"(> To get evaluation use command /Evaluation)");                                // DONE
    writer_.Print(R"(> To skip move (if there are no legal moves) use command /Skip 0)");           // DONE
    writer_.Print(R"(> To skip move (even if there are legal moves) use command /Skip 1)");         // DONE
    writer_.Print(R"(> To change depth use command /Depth [depth], where depth is 1..9 (6 and 7 are recommended))");
    writer_.Print(R"(> To leave use command /Leave)");
}
