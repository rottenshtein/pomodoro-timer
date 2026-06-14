#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>



class EventQueue {
	public:
		static EventQueue& get_instance() {
			static EventQueue instance;
			return instance;
		};
		using Event = std::function<void()>;
		void push(std::function<void()> func);
		void process_events();
	private:
		std::mutex mtx;
		std::condition_variable cv;
		std::queue<Event> queue;
		bool ready;
};
