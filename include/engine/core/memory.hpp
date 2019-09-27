#pragma once

#include <memory>
#include <cstring>

namespace Memory {
	template <typename T>
	using unique_ptr = std::unique_ptr<T>;

	template <typename T>
	using shared_ptr = std::shared_ptr<T>;

	template <typename T>
	using weak_ptr = std::weak_ptr<T>;

	FORCEINLINE void* memcpy(void* dest, const void* src, uintptr amt) {
		return std::memcpy(dest, src, amt);
	}

	FORCEINLINE void* memmove(void* dest, const void* src, uintptr amt) {
		return std::memmove(dest, src, amt);
	}

	FORCEINLINE int32 memcmp(const void* dest, const void* src, uintptr amt) {
		return std::memcmp(dest, src, amt);
	}

	FORCEINLINE void* memset(void* dest, int value, uintptr amt) {
		return std::memset(dest, value, amt);
	}

	template <typename T, typename... Args>
	FORCEINLINE unique_ptr<T> make_unique(Args&&... args) {
		return std::make_unique<T>(args...);
	}

	template <typename T, typename... Args>
	FORCEINLINE shared_ptr<T> make_shared(Args&&... args) {
		return std::make_shared<T>(args...);
	}
};
