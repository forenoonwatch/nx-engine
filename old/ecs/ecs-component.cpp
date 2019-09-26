#include "engine/ecs/ecs-component.hpp"

ArrayList<ECSComponentTypeData>* BaseECSComponent::componentTypes;

uint32 BaseECSComponent::registerComponentType(ECSComponentCreateFunction createfn,
				ECSComponentFreeFunction freefn, size_t size) {
	if (componentTypes == nullptr) {
		componentTypes = new ArrayList<ECSComponentTypeData>();
	}

	uint32 componentID = componentTypes->size();

	componentTypes->emplace_back(createfn, freefn, size);

	return componentID;
}
