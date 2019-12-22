#pragma once

#include <engine/core/common.hpp>

class BitArray {
	public:
		BitArray(size_t size);

		void clear();

		void setBit(size_t index);
		void clearBit(size_t index);

		uint64 getBit(size_t index) const;

		inline size_t getSize() const { return size; }

		~BitArray();
	private:
		NULL_COPY_AND_ASSIGN(BitArray);

		const size_t size;
		const size_t bytes;
		
		uint64* data;
};

