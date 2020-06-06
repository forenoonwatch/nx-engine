#pragma once

#include <engine/resource/resource-loader.hpp>

#include <engine/rendering/indexed-model.hpp>

class ModelLoader final : public ResourceLoader<ModelLoader, IndexedModel> {
	public:
		Memory::SharedPointer<IndexedModel> load(const IndexedModel& model) const {
			return Memory::make_shared<IndexedModel>(model);
		}
	private:
};
