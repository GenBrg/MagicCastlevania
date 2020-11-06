#pragma once

#include <chrono>
#include <functional>

/**
 * Class for limiting function call rate.
 */
class TimerGuard {
public:
	/** Try to call the function.
	 *  @param fn Function to call.
	 * 	@param cooldown Seconds to wait until next call if this call succeeds.
	 */
	void operator()(float cooldown, const std::function<void()>& fn);

private:
	std::chrono::high_resolution_clock::time_point next_call_time_;
};
