#pragma once

#include <engine/game/resource-loader.hpp>

#include <engine/rendering/shader.hpp>

#include <engine/core/application.hpp>

class ShaderLoader final : public ResourceLoader<ShaderLoader, Shader> {
	public:
		Memory::SharedPointer<Shader> load(const char* fileName) const {
			Memory::SharedPointer<Shader> sh = Memory::make_shared<Shader>(
					*static_cast<RenderContext*>(&Application::getInstance()));

			if (!sh->load(fileName)) {
				return nullptr;
			}

			return sh;
		}
	private:
};

