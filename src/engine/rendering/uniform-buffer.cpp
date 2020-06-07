#include "engine/rendering/uniform-buffer.hpp"

#include <engine/core/memory.hpp>

#include <engine/rendering/render-context.hpp>

UniformBuffer::UniformBuffer(RenderContext& context, uintptr dataSize,
			uint32 usage, uint32 blockBinding, const void* data)
		: context(&context)
		, bufferID(0)
		, blockBinding(blockBinding)
		, size(dataSize) {
	glGenBuffers(1, &bufferID);

	glBindBuffer(GL_UNIFORM_BUFFER, bufferID);
	glBufferData(GL_UNIFORM_BUFFER, dataSize, data, usage);

	glBindBufferBase(GL_UNIFORM_BUFFER, blockBinding, bufferID);
}

void UniformBuffer::update(const void* data, uintptr dataSize) {
	glBindBuffer(GL_UNIFORM_BUFFER, bufferID);

	void* dest = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	Memory::memcpy(dest, data, dataSize);
	glUnmapBuffer(GL_UNIFORM_BUFFER);
}

void UniformBuffer::update(const void* data, uintptr offset, uintptr dataSize) {
	glBindBuffer(GL_UNIFORM_BUFFER, bufferID);

	void* dest = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	Memory::memcpy((void*)((uintptr)dest + offset), data, dataSize);
	glUnmapBuffer(GL_UNIFORM_BUFFER);
}

void* UniformBuffer::map() {
	glBindBuffer(GL_UNIFORM_BUFFER, bufferID);

	return glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
}

void* UniformBuffer::map(uintptr offset, uintptr size) {
	glBindBuffer(GL_UNIFORM_BUFFER, bufferID);

	return glMapBufferRange(GL_UNIFORM_BUFFER, offset, size, GL_WRITE_ONLY);
}

void UniformBuffer::unmap() {
	glUnmapBuffer(GL_UNIFORM_BUFFER);
}

void UniformBuffer::addVariableOffset(const String& name, uintptr offset) {
	variableOffsets[name] = offset;
}

void UniformBuffer::set(const String& name, float value) {
	update(&value, variableOffsets[name], sizeof(float));
}

void UniformBuffer::set(const String& name, const Vector2f& value) {
	update(&value, variableOffsets[name], sizeof(Vector2f));
}

void UniformBuffer::set(const String& name, const Vector3f& value) {
	update(&value, variableOffsets[name], sizeof(Vector3f));
}

void UniformBuffer::set(const String& name, const Vector4f& value) {
	update(&value, variableOffsets[name], sizeof(Vector4f));
}

void UniformBuffer::set(const String& name, const Matrix4f& value) {
	update(&value, variableOffsets[name], sizeof(Matrix4f));
}

UniformBuffer::~UniformBuffer() {
	glDeleteBuffers(1, &bufferID);

	context->uniformBufferBindings[blockBinding] = false;
}
