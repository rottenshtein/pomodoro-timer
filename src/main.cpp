#include <atomic>
#include <csignal>
#include <cstdlib>
#include <ini.h>
#include <filesystem>
#include <string>
#include <string_view>
#include <iostream>
#include <system_error>
#include <thread>
#include <libnotify/notify.h>

#include "config.hpp"
#include "timer.hpp"
#include "eventQueue.hpp"
#include "ui.hpp"

#define TIMER_CONFIG_NAME "pomodoro_config.ini"

std::filesystem::path find_config();

bool verbose = false;
std::atomic<bool> g_running = true;
Timer g_timer;

void signal_handler(int signal_num) {
	if (signal_num == SIGINT || signal_num == SIGTERM) {
		g_running = false;
		EventQueue::get_instance().push([]{});
	} else if (signal_num == SIGUSR1) {
		g_timer.changeState();
			bool timer_state = g_timer.getState();
			std::cout << "\r\033[2K" << std::flush;
			std::cout << "\rTimer " << (timer_state ? "resumed" : "paused") << std::flush;
	}
}

int main(int argc, char* argv[]) {
	std::signal(SIGINT, signal_handler);
	std::signal(SIGTERM, signal_handler);
	std::signal(SIGUSR1, signal_handler);

	bool nonotify = false;

	std::filesystem::path config_file_path;
	// arg parsing
	if (argc>1) {
		for (int i = 1; i < argc; i++) {
			std::string_view arg = argv[i];
			if (arg=="-h"||arg=="--help") {
				std::cout << argv[0] << " - simple pomodoro timer.\n" ;
				std::cout << "Usage\n" <<
					         argv[0] << " <flags>\n\n" <<
							 "Flags:\n" <<
							 "--config (-c) <path/to/config.ini>\n" <<
							 "--verbose (-v)\n" <<
							 "--nonotify (-N)\n";
				return 0;
			} else if (arg=="-c"||arg=="--config") {
				std::error_code ec;
				if (i+1==argc||argv[i+1][0]=='-') {
					std::cout << argv[0] << ": flag \"" << arg << "\" takes one positonal argument.\n" ;
					return 1;
				}
				if (std::filesystem::exists(argv[i+1], ec)) {
					if (std::filesystem::is_directory(argv[i+1])) {
						std::cout << argv[0] << ": " << argv[i+1] << ": " 
							<< std::make_error_code(std::errc::is_a_directory).message() << "\n";
						return std::make_error_code(std::errc::is_a_directory).value();
					}
					config_file_path = argv[i+1];
					i++;
					continue;
				} else if (ec) {
					std::cout << argv[0] << ": \"" << arg << "\": " << argv[i+1] << ": " << ec.message() << "\n" ;
					return ec.value();
				} else {
					std::error_code not_found(std::make_error_code(std::errc::no_such_file_or_directory));
					std::cout << argv[0] << ": \"" << arg << "\": "  << argv[i+1] << ": " << 
						not_found.message() << "\n";
					return not_found.value();
				}
			} else if (arg=="-V"||arg=="--verbose") {
				std::cout << argv[0] << ": verbose mode enabled.\n";
				verbose = true;
				continue;
			} else if (arg=="-N"||arg=="--nonotify") {
				nonotify = true;
			} else {
				std::cout << argv[0] << ": \"" << arg << "\" - unrecognized " 
					<< (argv[i][0]=='-' ? "flag" : "positional argument") <<".\nUse -h or --help for help.\n";
				return 1;
			}
		}
	}

	std::filesystem::path default_config_file_path = find_config();
	PomodoroConfig timerConfig(true);
	auto defaultTimerConfig = read_pomodoro_config(default_config_file_path);
	auto userTimerConfig = read_pomodoro_config(config_file_path);
	timerConfig = PomodoroConfig(false)^defaultTimerConfig^userTimerConfig;
	if (!nonotify) timerConfig.notification = 1;

	if (verbose) std::cout << "[VERBOSE] User config:\n" << timerConfig << "\n";
	if (verbose) std::cout << "[VERBOSE] Config:\n" << timerConfig << "\n";
	
	if (timerConfig.notification) {
		if (!notify_init("Pomodoro")) {
			std::cerr << "Failed to initialize libnotify.";
			return 1;
		}
	}

	std::cout << "Controls: [p]ause/resume, [q]uit.\n";

	std::thread input([] {
			input_thread();
			g_running = false;
			EventQueue::get_instance().push([]{}); // Push dummy event to bypass wait for events in main loop
		});

	PomodoroChain chain;
	ChainState state = ChainState::STATE_FIRSTRUN;
	
	chain.chain(state, timerConfig);

	while (g_running) {
		EventQueue::get_instance().process_events();
	}
	if (input.joinable()) input.join();
	g_timer.stop();

    if (timerConfig.notification) notify_uninit();

	return 0;
}

std::filesystem::path find_config() {
	std::filesystem::path workdir = std::filesystem::current_path();
	std::filesystem::path config = workdir / TIMER_CONFIG_NAME;
	
	if (std::filesystem::exists(config)) {
		return config;
	}
	std::filesystem::path home = std::string(std::getenv("HOME"));
	config = home / ".config" / "pomodoro" / TIMER_CONFIG_NAME;
	if (std::filesystem::exists(config)) {
		return config;
	}

	config = std::filesystem::path("/etc") / "pomodoro" / TIMER_CONFIG_NAME;
	if (std::filesystem::exists(config)) {
		return config;
	}
	
	if (verbose) std::cout << "[VERBOSE] default config not found\n";
	
	return std::filesystem::path();
}


