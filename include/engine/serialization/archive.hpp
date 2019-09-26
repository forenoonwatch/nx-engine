#pragma once

#include "engine/core/common.hpp"

#include <cereal/archives/binary.hpp>

#define InputArchive cereal::BinaryInputArchive
#define OutputArchive cereal::BinaryOutputArchive

namespace Serialization {
	template <typename T>
	FORCEINLINE auto encodeBinary(T&& data, size_t size) {
		return cereal::binary_data((void*)data, size);
	}
};
