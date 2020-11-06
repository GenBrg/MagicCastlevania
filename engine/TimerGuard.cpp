#include "TimerGuard.hpp"

void TimerGuard::operator()(float cooldown, const std::function<void()>& fn)
{
	auto now = std::chrono::high_resolution_clock::now();

	if (now > next_call_time_) {
		next_call_time_ = now + std::chrono::milliseconds(static_cast<long long>(cooldown * 1000));
		fn();
	}
}
