#include "engine/core/bit-array.hpp"

#include <engine/core/memory.hpp>

BitArray::BitArray(size_t size)
		: size(size)
		, bytes( 8 * ((size / 64) + ((size % 64) ? 1 : 0)) )
		, data((uint64*)Memory::malloc(bytes)) {
	clear();		
}

void BitArray::clear() {
	Memory::memset(data, 0, bytes);
}

void BitArray::setBit(size_t index) {
	const size_t dataIndex = index >> 6;
	const size_t bitIndex = index & ((1 << 6) - 1);

	data[dataIndex] |= ((uint64)1) << bitIndex;
}

void BitArray::clearBit(size_t index) {
	const size_t dataIndex = index >> 6;
	const size_t bitIndex = index & ((1 << 6) - 1);

	data[dataIndex] &= ~( ((uint64)1) << bitIndex );
}

uint64 BitArray::getBit(size_t index) const {
	const size_t dataIndex = index >> 6;
	const size_t bitIndex = index & ((1 << 6) - 1);

	return (data[dataIndex] >> bitIndex) & 1;
}

BitArray::~BitArray() {
	Memory::free(data);
}

