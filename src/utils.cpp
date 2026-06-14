#include "utils.hpp"
#include <algorithm>
#include <charconv>
#include <string>
#include <unistd.h>
#include <termios.h>

bool is_valid_num(std::string str) {
	if (str.empty()) return false;

	int value;

	auto [ptr,ec] = std::from_chars(str.data(), str.data() + str.size(), value);

	return ec == std::errc() && ptr == (str.data() + str.size());
}


char getch() {
	char buf = 0;
	struct termios old;

	if (tcgetattr(0, &old) < 0)
		perror("tcsetattr()");

	old.c_lflag &= ~ICANON;
	old.c_lflag &= ~ECHO;
	old.c_cc[VMIN] = 1;
	old.c_cc[VTIME] = 0;

	if (tcsetattr(0, TCSANOW, &old) < 0)
		perror("tcsetattr ICANON");

	if (read(0, &buf, 1) < 0)
		perror("read()");

	old.c_lflag |= ICANON;
	old.c_lflag |= ECHO;
	if (tcsetattr(0, TCSADRAIN, &old) < 0)
		perror("tcsetattr ~ICANON");

	return buf;
}

bool stringToBool(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
        return std::tolower(c);
    });

    return (str == "true" || str == "1" || str == "yes" || str == "on");
}
