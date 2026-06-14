#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <libnotify/notification.h>
#include <mutex>
#include <thread>
#include "timer.hpp"
#include "eventQueue.hpp"
#include "globals.hpp"

void Timer::start(int seconds, std::function<void()> callback) {
	if (worker.joinable()) {
		stop();
	}

	paused = false;
	running = true;
	stopThread = false;

	worker = std::thread([this, seconds, callback] {
		std::unique_lock<std::mutex> lock(mtx);
		std::chrono::duration remaining = std::chrono::seconds(seconds);
		std::chrono::time_point endTime = std::chrono::steady_clock().now()+remaining;
		while (!stopThread) {
			if (running) {
				if(cv.wait_for(lock, remaining, [this] {
					return paused || stopThread;
				})) {
					if (paused) {
						remaining = std::chrono::duration_cast<std::chrono::seconds>
							(endTime-std::chrono::steady_clock().now());
						cv.wait(lock, [this] { return !paused; });
					}
				} else { 
					stopThread = true;
					EventQueue::get_instance().push(callback);
				}
			}
		}
	});
}
void Timer::pause() {
	{
		std::unique_lock lock(mtx);
		running = false;
		paused = true;
	}
	cv.notify_one();
}
void Timer::resume() {
	{
		std::unique_lock lock(mtx);
		paused = false;
		running = true;
	}
	cv.notify_one();
}

void Timer::stop() {
	{
		std::unique_lock lock(mtx);
		running = false;
		paused = false;
		stopThread = true;
	}
	cv.notify_one();
	if (worker.joinable()) worker.join();
}

void Timer::changeState() {
	bool local_state;
	bool local_stop;
	{
		std::unique_lock lock(mtx);
		local_state = running;
		local_stop = stopThread;
	}
	if (local_stop) return;
	if (local_state) pause();
	else resume();
}	

bool Timer::getState() {
	bool local_state;
	{
		std::unique_lock lock(mtx);
		local_state = !paused;
	}
	return local_state;
}

int Timer::getRemainingSeconds() const {
	std::unique_lock lock(mtx);
	if (stopThread) {
		return 0;
	}
	if (paused) {
		if (remainingTime.count()<0) return 0;
		return static_cast<int>(remainingTime.count());
	}
	return -1;
}

extern Timer g_timer;

void PomodoroChain::chain(ChainState state, PomodoroConfig config) {
	int timerSeconds = 0;
	std::string msg;
	ChainState nextState = state;

	switch (state) {
		case ChainState::STATE_FIRSTRUN:
			msg = config.startText;
			timerSeconds = config.workMins * 60 + config.workSeconds;
			nextState = ChainState::STATE_REST;
			break;
		case ChainState::STATE_WORK:
			msg = config.workNotification;
			timerSeconds = config.workMins * 60 + config.workSeconds;
			nextState = ChainState::STATE_REST;
			break;
		case ChainState::STATE_REST:
			msg = config.restNotification;
			timerSeconds = config.restMins * 60 + config.restSeconds;
			nextState = ChainState::STATE_WORK;
			break;
	}

	if (config.notification) {
		NotifyNotification* notification = notify_notification_new("Pomodoro", msg.data(), "dialog-information");
		notify_notification_set_urgency(notification, NOTIFY_URGENCY_NORMAL);
		notify_notification_set_timeout(notification, 5000);
		GError* error = nullptr;
		if (!notify_notification_show(notification, &error)) {
			std::cerr << "Failed to send notification: "
					  << (error ? error->message : "Unknown error") << std::endl;
			if (error) g_error_free(error);
		}
		g_object_unref(G_OBJECT(notification));
	}
	
	std::cout << "\r\033[2K" << std::flush;
	std::cout << "\r" << msg << std::flush;
	g_timer.start(timerSeconds, [this, nextState, msg, config] {
		chain(nextState, config);
	});
}

