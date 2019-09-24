#include <initializer_list>

namespace {
	template <typename T>
	inline uint32 deduceID(T& obj) {
		return T::ID;
	}

	template <typename T>
	inline BaseECSComponent* deduceComponents(T& obj) {
		return (BaseECSComponent*)&obj;
	}
};

template <class Component>
inline void ECS::addComponent(EntityHandle handle, Component* component) {
	addComponentInternal(handle, handleToEntityComponents(handle), Component::ID, component);

	for (uint32 i = 0; i < listeners.size(); ++i) {
		const ArrayList<uint32>& componentIDs = listeners[i]->getComponentIDs();

		if (listeners[i]->notifyOnAllComponentOperations) {
			listeners[i]->onAddComponent(handle, Component::ID);
		}
		else {
			for (uint32 j = 0; j < componentIDs.size(); ++j) {
				if (componentIDs[j] == Component::ID) {
					listeners[i]->onAddComponent(handle, Component::ID);
					break;
				}
			}
		}
	}
}

template <class Component>
inline bool ECS::removeComponent(EntityHandle handle) {
	for (uint32 i = 0; i < listeners.size(); ++i) {
		const ArrayList<uint32>& componentIDs = listeners[i]->getComponentIDs();

		if (listeners[i]->notifyOnAllComponentOperations) {
			listeners[i]->onRemoveComponent(handle, Component::ID);
		}
		else {
			for (uint32 j = 0; j < componentIDs.size(); ++j) {
				if (componentIDs[j] == Component::ID) {
					listeners[i]->onRemoveComponent(handle, Component::ID);
					break;
				}
			}
		}
	}

	return removeComponentInternal(handle, Component::ID);
}

template <class Component>
inline Component* ECS::getComponent(EntityHandle handle) {
	return (Component*)getComponentInternal(handleToEntityComponents(handle), 
			components[Component::ID], Component::ID);
}

inline BaseECSComponent* ECS::getComponentByType(EntityHandle entity,
		uint32 componentID) {
	return getComponentInternal(handleToEntityComponents(entity),
			components[componentID], componentID);
}

template <class... ComponentTypes>
inline EntityHandle ECS::makeEntity(ComponentTypes... componentsIn) {
	const size_t numComponents = sizeof...(ComponentTypes);
	BaseECSComponent* components[numComponents];
	uint32 componentIDs[numComponents];
	
	std::initializer_list<uint32> ids {deduceID(componentsIn)...};
	std::initializer_list<BaseECSComponent*> comps {deduceComponents(componentsIn)...};
	int i = 0;

	for (auto it = std::begin(ids), end = std::end(ids); it != end; ++it) {
		componentIDs[i++] = *it;
	}

	i = 0;

	for (auto it = std::begin(comps), end = std::end(comps); it != end; ++it) {
		components[i++] = *it;
	}

	return makeEntityFromLists(components, componentIDs, numComponents);
}
