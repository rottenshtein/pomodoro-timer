#include "eventQueue.hpp"
#include <functional>
#include <mutex>
#include <queue>

void EventQueue::push(Event func) {
	{
		std::lock_guard lock(mtx);
		queue.push(func);
	}
	cv.notify_one();
}

void EventQueue::process_events() {
	std::unique_lock lock(mtx);
	
	cv.wait(lock, [this] { return !queue.empty(); });

	while (!queue.empty()) {
		auto callback = queue.front();
		queue.pop();

		lock.unlock();
		callback();
		lock.lock();
	}
}
