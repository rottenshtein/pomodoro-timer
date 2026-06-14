#include "timer.hpp"
#include "utils.hpp"
#include "globals.hpp"
#include <iostream>
#include <ostream>

extern Timer g_timer;
extern std::atomic<bool> g_running;

void input_thread() {
	while (true) {
		char input = getch();
		if (input == 'p') {
			g_timer.changeState();
			bool timer_state = g_timer.getState();
			std::cout << "\r\033[2K" << std::flush;
			std::cout << "\rTimer " << (timer_state ? "resumed" : "paused") << std::flush;
		} else if (input == 'q') {
			break;
		}
	}
}

