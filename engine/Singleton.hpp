#pragma once

// CRTP singleton base class.
template <typename T>
class Singleton {
public:
	static T* Instance() {
		static T singleton;
		return &singleton;
	}

	Singleton(const Singleton&) = delete;
	Singleton(Singleton&&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	Singleton& operator=(Singleton&&) = delete;

protected:
	Singleton() {}
};
