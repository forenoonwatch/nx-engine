#pragma once

#include <engine/resource/resource-loader.hpp>

#include <engine/rendering/shader.hpp>

class ShaderLoader final : public ResourceLoader<ShaderLoader, Shader> {
	public:
		Memory::SharedPointer<Shader> load(const char* fileName) const {
			auto& context = RenderContext::ref();

			Memory::SharedPointer<Shader> sh = Memory::make_shared<Shader>(context);

			if (!sh->load(fileName)) {
				return nullptr;
			}

			return sh;
		}
	private:
};

