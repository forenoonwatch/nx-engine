#pragma once

template <typename T>
class Singleton {
	public:
		static inline T& ref() { return instance; }
	private:
		static inline T instance;
};
