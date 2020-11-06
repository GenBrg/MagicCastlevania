#pragma once

#include <set>
#include <chrono>
#include <functional>

class Timer
{
private:
	std::function<void()> callback_;
	std::chrono::high_resolution_clock::time_point expire_time_;

public:
	Timer(const std::function<void()> &callback, std::chrono::high_resolution_clock::time_point expire_time) : callback_(callback),
																											   expire_time_(expire_time) {}

	void operator()()
	{
		callback_();
	}

	bool IsExpired() const
	{
		return std::chrono::high_resolution_clock::now() >= expire_time_;
	}

	std::chrono::high_resolution_clock::time_point GetExpireTime() const { return expire_time_; }
};

bool operator<(const Timer &lhs, const Timer &rhs);

class TimerManager
{
private:
	std::set<Timer> timers_;
	TimerManager() = default;

public:
	static TimerManager &Instance()
	{
		static TimerManager timer_manager;
		return timer_manager;
	}

	void AddTimer(long long duration, const std::function<void()> &callback);
	void Update();
};
