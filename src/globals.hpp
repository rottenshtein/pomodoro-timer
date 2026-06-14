#pragma once
#include "timer.hpp"
#include <atomic>

extern std::atomic<bool> g_running;
extern Timer g_timer;
