#pragma once

#include "engine/core/common.hpp"

#include "engine/core/string.hpp"
#include "engine/core/array-list.hpp"

#include <algorithm>

namespace Util {
	inline String toLower(const String& str) {
		String res = str;
		std::transform(res.begin(), res.end(), res.begin(), ::tolower);

		return res;
	}

	inline bool startsWith(const String& s0, const String& s1) {
		return s0.compare(0, s1.length(), s1);
	}

	void split(ArrayList<String>& elems, const String& s, char delim);
	ArrayList<String> split(const String& s, char delim);

	String getFilePath(const String& fileName);
	String getFileExtension(const String& fileName);

	bool resolveFileLinking(StringStream& out, const String& fileName,
			const String& linkKeyword);

	template <typename T>
	inline T reverseBits(T v) {
		T r = v;
		uint32 s = sizeof(T) * 8 - 1;

		for (v >>= 1; v; v >>= 1) {
			r <<= 1;
			r |= v & 1;
			--s;
		}

		r <<= s;

		return r;
	}

	template <typename T>
	inline T rotateLeft(T v, int32 shift) {
		return (v << shift) | (v >> (sizeof(T) * 8 - shift));
	}

	template <typename T>
	inline T rotateRight(T v, int32 shift) {
		return (v >> shift) | (v << (sizeof(T) * 8 - shift));
	}
};
