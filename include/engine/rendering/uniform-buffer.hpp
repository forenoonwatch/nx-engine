#pragma once

#include <engine/core/common.hpp>
#include <engine/core/hash-map.hpp>
#include <engine/core/string.hpp>
#include <engine/core/string-view.hpp>
#include <engine/core/memory.hpp>

#include <engine/math/matrix.hpp>

#include <engine/rendering/render-context.hpp>

#include <initializer_list>

class RenderContext;

class UniformBuffer {
	public:
		UniformBuffer(RenderContext& context, uintptr dataSize,
				uint32 usage, uint32 blockBinding, const void* data = nullptr);

		void update(const void* data, uintptr dataSize);
		void update(const void* data, uintptr offset, uintptr dataSize);
		inline void update(const void* data) { update(data, size); }

		void* map();
		void* map(uintptr offset, uintptr size);

		void unmap();

		void addVariableOffset(const String& name, uintptr offset);

		void set(const String& name, float value);

		void set(const String& name, const Vector2f& value);
		void set(const String& name, const Vector3f& value);
		void set(const String& name, const Vector4f& value);

		void set(const String& name, const Matrix4f& value);

		template <typename... Args>
		void set(std::initializer_list<StringView> keys,
				const Args&... args);

		inline uint32 getID() { return bufferID; }
		inline uint32 getBlockBinding() { return blockBinding; }

		inline uintptr getOffset(const String& name) { return variableOffsets[name]; }

		~UniformBuffer();
	private:
		NULL_COPY_AND_ASSIGN(UniformBuffer);

		RenderContext* context;

		uint32 bufferID;
		uint32 blockBinding;
		uintptr size;

		HashMap<String, uintptr> variableOffsets;
};

template <typename... Args>
void UniformBuffer::set(std::initializer_list<StringView> keys,
		const Args&... args) {
    std::initializer_list<std::pair<const void*, size_t>> memoryInfo {getObjectMemoryData(args)...};

	auto mi = memoryInfo.begin();

	glBindBuffer(GL_UNIFORM_BUFFER, bufferID);
	
	void* bufferBase = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);

	for (auto key : keys) {
		void* dest = reinterpret_cast<void*>(reinterpret_cast<uintptr>(bufferBase)
				+ variableOffsets[String(key.data(), key.size())]);
		Memory::memcpy(dest, mi->first, mi->second);

		++mi;
	}

	glUnmapBuffer(GL_UNIFORM_BUFFER);
}
