#include "TimerGuard.hpp"

bool TimerGuard::operator()(float cooldown, const std::function<void()>& fn)
{
	auto now = std::chrono::high_resolution_clock::now();

	if (now > next_call_time_) {
		next_call_time_ = now + std::chrono::milliseconds(static_cast<long long>(cooldown * 1000));
		fn();
		return true;
	}

	return false;
}

float TimerGuard::SecsUntilNextCall() const {
    auto interval = next_call_time_ - std::chrono::high_resolution_clock::now();
    float res = (float)std::chrono::duration_cast<std::chrono::milliseconds>(interval).count();
    return res > 0.0f ? (float)(res * 1e-3f) : 0.0f;
}
