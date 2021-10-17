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
    stream_ << static_cast<char>(cell.x + 'a') << static_cast<int>(cell.y) + 1 << std::endl;
}

Writer::Writer(std::ostream& output_stream) : stream_(output_stream) {
}