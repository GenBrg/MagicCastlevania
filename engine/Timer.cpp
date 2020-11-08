#include "Timer.hpp"

void TimerManager::AddTimer(float duration, const std::function<void()> &callback)
{
	auto expire_time = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(static_cast<long long>(duration * 1000));
	timers_.emplace(callback, expire_time);
}

void TimerManager::Update()
{
	while (!timers_.empty())
	{
		auto &timer = const_cast<Timer &>(*(timers_.begin()));

		if (timer.IsExpired())
		{
			timer();
			timers_.erase(timers_.begin());
		}
		else
		{
			break;
		}
	}
}

bool operator<(const Timer &lhs, const Timer &rhs)
{
	return lhs.GetExpireTime() < rhs.GetExpireTime();
}
