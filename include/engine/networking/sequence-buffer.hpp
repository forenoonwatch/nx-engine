#pragma once

#include <engine/core/bit-array.hpp>

#include <engine/core/common.hpp>
#include <engine/core/memory.hpp>

#include <cstdlib> // TODO: move this crap to engine memory

constexpr bool sequenceGreaterThan(uint16 a, uint16 b) {
	return ( (a > b) && (a - b <= 32768) || (a < b) && (b - a > 32768) );
}

constexpr bool sequenceLessThan(uint16 a, uint16 b) {
	return sequenceGreaterThan(b, a);
}

template <typename T>
class SequenceBuffer {
	public:
		SequenceBuffer(size_t size)
				: entries((T*)std::malloc(sizeof(T) * size))
				, entrySequences((uint16*)std::malloc(sizeof(uint16) * size))
				, entryFlags(size)
				, sequence(0)
				, size(size)
				, firstEntry(true) {
			reset();
		}

		void reset() {
			entryFlags.clear();
			Memory::memset(entrySequences, 0, sizeof(uint16) * size);

			sequence = 0;
			firstEntry = true;
		}

		T* insert(uint16 sequence) {
			if (firstEntry) {
				this->sequence = sequence + 1;
				firstEntry = false;
			}
			else if (sequenceGreaterThan(sequence + 1, this->sequence)) {
				this->sequence = sequence + 1;
			}
			else if (sequenceLessThan(sequence,
					this->sequence - (uint16)size)) {
				return nullptr;
			}

			DEBUG_LOG_TEMP("INSERTING SEQUENCE %d", sequence);

			const size_t index = sequence % size;

			entryFlags.setBit(index);
			entrySequences[index] = sequence;

			return entries + index;
		}

		inline void remove(uint16 sequence) {
			entryFlags.clearBit(sequence % size);
		}

		inline bool isAvailable(uint16 sequence) const {
			return !entryFlags.getBit(sequence % size);
		}

		inline size_t getIndex(uint16 sequence) const {
			return sequence % size;
		}

		inline const T* find(uint16 sequence) const {
			const size_t index = sequence % size;

			if (entryFlags.getBit(index)
					&& entrySequences[index] == sequence) {
				return entries + index;
			}
			
			return nullptr;
		}

		inline T* find(uint16 sequence) {
			const size_t index = sequence % size;

			if (entryFlags.getBit(index)
					&& entrySequences[index] == sequence) {
				return entries + index;
			}
			
			return nullptr;
		}

		inline T* getAtIndex(size_t index) {
			return entryFlags.getBit(index) ? (entries + index) : (T*)nullptr;
		}

		inline uint16 getSequence() const { return sequence; }
		inline size_t getSize() const { return size; }

		~SequenceBuffer() {
			std::free(entries);
			std::free(entrySequences);
		}
	private:
		NULL_COPY_AND_ASSIGN(SequenceBuffer);

		T* entries;
		uint16* entrySequences;
		BitArray entryFlags;

		uint16 sequence;

		size_t size;

		bool firstEntry;
};

