#include "engine/ecs/ecs.hpp"

#include "engine/core/memory.hpp"
#include "engine/math/math.hpp"

EntityHandle ECS::makeEntityFromLists(BaseECSComponent** entityComponents,
		const uint32* componentIDs, size_t numComponents) {
	RawEntity* newEntity = new RawEntity();
	EntityHandle handle = (EntityHandle)newEntity;

	ECSComponentCreateFunction createfn;

	for (uint32 i = 0; i < numComponents; ++i) {
		if (!BaseECSComponent::isTypeValid(componentIDs[i])) {
			DEBUG_LOG("ECS", LOG_ERROR, "'%u' is not a valid component type.", componentIDs[i]);
			delete newEntity;
			return NULL_ENTITY_HANDLE;
		}

		createfn = BaseECSComponent::getTypeCreateFunction(componentIDs[i]);
		newEntity->entityComponents.emplace_back(componentIDs[i],
				createfn(components[componentIDs[i]], handle, entityComponents[i]));
	}

	newEntity->index = entities.size();
	entities.push_back(newEntity);

	for (uint32 i = 0; i < listeners.size(); ++i) {
		if (listeners[i]->notifyOnAllEntityOperations) {
			listeners[i]->onMakeEntity(handle);
		}
		else {
			const ArrayList<uint32>& listenerCompIDs = listeners[i]->getComponentIDs();
			bool isValid = true;

			for (uint32 j = 0; j < listenerCompIDs.size(); ++j) {
				bool hasComponent = false;

				for (uint32 k = 0; k < numComponents; ++k) {
					if (listenerCompIDs[j] == componentIDs[k]) {
						hasComponent = true;
						break;
					}
				}

				if (!hasComponent) {
					isValid = false;
					break;
				}
			}

			if (isValid) {
				listeners[i]->onMakeEntity(handle);
			}
		}
	}

	return handle;
}

void ECS::removeEntity(EntityHandle handle) {
	EntityComponents& entityComponents = handleToEntityComponents(handle);

	for (uint32 i = 0; i < listeners.size(); ++i) {
		if (listeners[i]->notifyOnAllEntityOperations) {
			listeners[i]->onRemoveEntity(handle);
		}
		else {
			const ArrayList<uint32>& componentIDs = listeners[i]->getComponentIDs();
			bool isValid = true;

			for (uint32 j = 0; j < entityComponents.size(); ++j) { // O(n) entityComponents search
				bool hasComponent = false;

				for (uint32 k = 0; k < componentIDs.size(); ++k) {
					if (componentIDs[k] == entityComponents[j].first) {
						hasComponent = true;
						break;
					}
				}

				if (!hasComponent) {
					isValid = false;
					break;
				}
			}

			if (isValid) {
				listeners[i]->onRemoveEntity(handle);
			}
		}
	}

	for (uint32 i = 0; i < entityComponents.size(); ++i) {
		deleteComponent(entityComponents[i].first, entityComponents[i].second);
	}

	uint32 destIndex = handleToEntityIndex(handle);
	uint32 srcIndex = entities.size() - 1;

	delete entities[destIndex];

	entities[destIndex] = entities[srcIndex];
	entities[destIndex]->index = destIndex;

	entities.pop_back();
}

// O(1)
void ECS::addComponentInternal(EntityHandle handle, EntityComponents& entityComponents,
		uint32 componentID, BaseECSComponent* component) {
	ECSComponentCreateFunction createfn = BaseECSComponent::getTypeCreateFunction(componentID);
	entityComponents.emplace_back(componentID,
			createfn(components[componentID], handle, component));
}

BaseECSComponent* ECS::getComponentInternal(EntityComponents& entityComponents,
		ArrayList<uint8>& array, uint32 componentID) {
	for (uint32 i = 0; i < entityComponents.size(); ++i) { // O(n) search
		if (componentID == entityComponents[i].first) {
			return (BaseECSComponent*)&array[entityComponents[i].second];
		}
	}

	return nullptr;
}

bool ECS::removeComponentInternal(EntityHandle handle, uint32 componentID) {
	EntityComponents& entityComponents = handleToEntityComponents(handle);

	for (uint32 i = 0; i < entityComponents.size(); ++i) { // O(n) search
		if (componentID == entityComponents[i].first) {
			deleteComponent(entityComponents[i].first, entityComponents[i].second);
			uint32 srcIndex = entityComponents.size() - 1;
			uint32 destIndex = i;

			entityComponents[destIndex] = entityComponents[srcIndex];
			entityComponents.pop_back();

			return true;
		}
	}

	return false;
}

void ECS::deleteComponent(uint32 componentID, uint32 index) {
	ArrayList<uint8>& data = components[componentID];
	ECSComponentFreeFunction freefn = BaseECSComponent::getTypeFreeFunction(componentID);
	size_t typeSize = BaseECSComponent::getTypeSize(componentID);

	uint32 srcIndex = data.size() - typeSize;
	BaseECSComponent* srcComponent = (BaseECSComponent*)&data[srcIndex];
	BaseECSComponent* destComponent = (BaseECSComponent*)&data[index];

	freefn(destComponent);

	if (index == srcIndex) {
		data.resize(srcIndex);
		return;
	}

	Memory::memcpy(destComponent, srcComponent, typeSize);

	EntityComponents& srcComponents = handleToEntityComponents(srcComponent->entity);

	for (uint32 i = 0; i < srcComponents.size(); ++i) { // O(n) iteration for a search/match
		if (componentID == srcComponents[i].first && srcIndex == srcComponents[i].second) {
			srcComponents[i].second = index;
			break;
		}
	}

	data.resize(srcIndex);
}

void ECS::updateSystems(ECSSystemList& systems, float delta) {
	ArrayList<BaseECSComponent*> componentParam;
	ArrayList<ArrayList<uint8>*> componentArrayLists;
	BaseECSComponent* component;

	for (uint32 i = 0; i < systems.size(); ++i) {
		const ArrayList<uint32>& componentTypes = systems[i]->getComponentTypes();

		if (componentTypes.size() == 1) {
			size_t typeSize = BaseECSComponent::getTypeSize(componentTypes[0]);
			ArrayList<uint8>& array = components[componentTypes[0]];

			for (uint32 j = 0; j < array.size(); j += typeSize) {
				component = (BaseECSComponent*)&array[j];
				systems[i]->updateComponents(delta, &component);
			}
		}
		else {
			componentParam.resize(Math::max(componentParam.size(), componentTypes.size()));
			componentArrayLists.resize(Math::max(componentArrayLists.size(), componentTypes.size()));

			updateSystemWithMultipleComponents(i, systems, delta,
					componentTypes, componentParam, componentArrayLists);
		}
	}
}

void ECS::updateSystemWithMultipleComponents(uint32 index, ECSSystemList& systems, float delta,
		const ArrayList<uint32>& componentTypes, ArrayList<BaseECSComponent*>& componentParam,
		ArrayList<ArrayList<uint8>*>& componentArrayLists) {

	const ArrayList<uint32>& componentFlags = systems[index]->getComponentFlags();

	for (uint32 i = 0; i < componentTypes.size(); ++i) {
		componentArrayLists[i] = &components[componentTypes[i]];
	}

	uint32 minSizeIndex = findLeastCommonComponent(componentTypes, componentFlags); // O(n)
	size_t typeSize = BaseECSComponent::getTypeSize(componentTypes[minSizeIndex]);
	ArrayList<uint8>& array = *componentArrayLists[minSizeIndex];

	for (uint32 i = 0; i < array.size(); i += typeSize) { // O(numComponents * componentTypes.size)
		componentParam[minSizeIndex] = (BaseECSComponent*)&array[i];
		EntityComponents& entityComponents = handleToEntityComponents(componentParam[minSizeIndex]->entity);
		bool isValid = true;

		for (uint32 j = 0; j < componentTypes.size(); ++j) { // O(componentTypes.size) but its system's types
			if (j == minSizeIndex) {
				continue;
			}

			componentParam[j] = getComponentInternal(entityComponents, *componentArrayLists[j],
					componentTypes[j]);

			if (componentParam[j] == nullptr
					&& (componentFlags[j] & ECSSystem::FLAG_OPTIONAL) == 0) {
				isValid = false;
				break;
			}
		}

		if (isValid) {
			systems[index]->updateComponents(delta, &componentParam[0]);
		}
	}
}

// O(n) iteration over componentTypes, purpose is to find smallest RawEntity->index
uint32 ECS::findLeastCommonComponent(const ArrayList<uint32>& componentTypes,
		const ArrayList<uint32>& componentFlags) {
	uint32 minSize = (uint32)(-1);
	uint32 minIndex = (uint32)(-1);
	size_t typeSize;
	uint32 size;

	for (uint32 i = 0; i < componentTypes.size(); ++i) {
		if ((componentFlags[i] & ECSSystem::FLAG_OPTIONAL) != 0) {
			continue;
		}

		typeSize = BaseECSComponent::getTypeSize(componentTypes[i]);
		size = components[componentTypes[i]].size() / typeSize;

		if (size <= minSize) {
			minSize = size;
			minIndex = i;
		}
	}

	return minIndex;
}

ECS::~ECS() {
	size_t typeSize;
	ECSComponentFreeFunction freefn;

	for (auto it = std::begin(components), end = std::end(components); it != end; ++it) {
		typeSize = BaseECSComponent::getTypeSize(it->first);
		freefn = BaseECSComponent::getTypeFreeFunction(it->first);

		for (uint32 i = 0; i < it->second.size(); i += typeSize) {
			freefn((BaseECSComponent*)(&it->second[i]));
		}
	}

	for (uint32 i = 0; i < entities.size(); ++i) {
		delete entities[i];
	}
}
