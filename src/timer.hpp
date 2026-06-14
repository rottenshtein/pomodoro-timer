#pragma once

#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

#include "config.hpp"

class Timer {
	private:
		bool paused;
		bool running;
		bool stopThread;
		bool state;
		mutable std::mutex mtx;
		std::condition_variable cv;
		std::thread worker;
		std::chrono::seconds remainingTime{0};
		std::chrono::steady_clock::time_point lastCheckpoint;

	public:
		explicit Timer() : paused(true), running(false), stopThread(true) {}; 
		~Timer() {stop();};
		void start(int seconds, std::function<void()> callback);
		void pause();
		void resume();
		void stop();
		void changeState();
		bool getState();
		int getRemainingSeconds() const;
};

enum class ChainState {
	STATE_WORK,
	STATE_REST,
	STATE_FIRSTRUN
};

class PomodoroChain {
	public:
		void chain(ChainState state, PomodoroConfig config);
	private:
		ChainState state = ChainState::STATE_FIRSTRUN;
};


