#pragma once

#include <map>
#include <string>

using std::map;
using std::string;

class ArgumentParser {
public:
	enum Action {
		Play,
		Help,
		Unknown
	};

	ArgumentParser(int argc, char* argv[]);

	Action GetAction() const;

	int32_t GetDepth();

	int32_t GetOrder();

	void PrintHelp() const;

private:
	Action action_;
	map<string, int32_t> params_;
};