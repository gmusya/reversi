#include "ArgumentParser.h"

#include <iostream>

ArgumentParser::ArgumentParser(int argc, char* argv[]) {
	if (argc <= 1 || argc >= 5) {
		action_ = Unknown;
	} else if (argc == 2) {
		if (string(argv[1]) == string("-h")) {
			action_ = Help;
		} else {
			action_ = Unknown;
		}
	} else if (string(argv[1]) != string("-p")) {
		action_ = Unknown;
	} else {
		if (string(argv[2]) != "-f" && string(argv[2]) != "-s") {
			action_ = Unknown;
		} else {
			if (string(argv[2]) == "-f") {
				params_["order"] = 1;
			} else {
				params_["order"] = 2;
			}
			if (argc == 3) {
				action_ = Play;
				params_["depth"] = 6;
			} else {
				if (argv[3][0] == '-' && argv[3][1] == 'd' && argv[3][2] == '=') {
					int32_t depth = (unsigned char)argv[3][3] - '0';
					if (1 <= depth && depth <= 9) {
						action_ = Play;
						params_["depth"] = depth;
					} else {
						action_ = Unknown;
					}
				}
			}
		}
	}
}

void ArgumentParser::PrintHelp() const {
	std::cout << std::endl;
	std::cout << R"(> To play use command "reversi -p ["-f" or "-s"] -d=[depth])" << std::endl;
	std::cout << ">>> -f - you play first" << std::endl;
	std::cout << ">>> -s - you play second" << std::endl;
	std::cout << ">>> depth - number from 1 to 9 (<= 5 too weak, >= 8 too slow)" << std::endl;
	std::cout << "> To decode use command \"archiver -d archive_name folder_name\"" << std::endl;
	std::cout << "> To get help use command \"archiver -h\"" << std::endl;
}

ArgumentParser::Action ArgumentParser::GetAction() const {
	return action_;
}

int32_t ArgumentParser::GetDepth() {
	return params_["depth"];
}

int32_t ArgumentParser::GetOrder() {
	return params_["order"];
}
