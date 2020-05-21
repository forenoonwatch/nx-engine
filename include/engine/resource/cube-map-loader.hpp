#pragma once

#include <engine/core/string.hpp>

#include <engine/resource/resource-loader.hpp>

#include <engine/rendering/cube-map.hpp>

class CubeMapLoader final : public ResourceLoader<CubeMapLoader, CubeMap> {
	public:
		Memory::SharedPointer<CubeMap> load(const String* fileNames, uint32 numFiles) const {
			auto& context = RenderContext::ref();

			Memory::SharedPointer<CubeMap> cm = Memory::make_shared<CubeMap>(context);

			if (numFiles == 1) {
				if (!cm->load(*fileNames)) {
					return nullptr;
				}

				return cm;
			}
			else {
				if (!cm->load(fileNames)) {
					return nullptr;
				}

				return cm;
			}
		}
	private:
};
