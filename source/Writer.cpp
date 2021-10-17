#include "Writer.h"

void Writer::Print(const vector<vector<char>>& board) const {
    for (size_t i = 0; i < board.size(); ++i) {
        stream_ << board.size() - i << ' ';
        for (const auto& symbol : board[board.size() - i - 1]) {
            stream_ << symbol << ' ';
        }
        stream_ << std::endl;
    }
    stream_ << "  ";
    for (char ch = 'a'; ch <= 'h'; ++ch) {
        stream_ << ch << ' ';
    }
    stream_ << std::endl;
}

void Writer::Print(const Cell& cell) const {
    if (cell.x == -1 && cell.y == -1) {
        stream_ << "Skip" << std::endl;
    } else if (cell.x == -2 && cell.y == -2) {
        stream_ << "There is no last move" << std::endl;
    } else {
        stream_ << static_cast<char>(cell.x + 'a') << static_cast<int>(cell.y) + 1 << std::endl;
    }
}

void Writer::Print(const std::string& string) const {
    stream_ << string << std::endl;
}

Writer::Writer(std::ostream& output_stream) : stream_(output_stream) {
}

void Writer::Print(int32_t value) const {
    stream_ << value;
}
