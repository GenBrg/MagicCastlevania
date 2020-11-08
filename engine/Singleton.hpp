#pragma once

// CRTP singleton base class.
template <typename T>
class Singleton {
public:
	static T* Instance() {
		static T singleton;
		return &singleton;
	}

	T(const T&) = delete;
	T(T&&) = delete;
	T& operator=(const T&) = delete;
	T& operator=(T&&) = delete;
};
