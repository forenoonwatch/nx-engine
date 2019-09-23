#pragma once

#include <cstring>

namespace Memory {
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
};
