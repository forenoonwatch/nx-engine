#pragma once

#include "engine/core/common.hpp"
#include "engine/core/array-list.hpp"

class BaseECSComponent;

typedef void* EntityHandle;
typedef uint32 (*ECSComponentCreateFunction)(ArrayList<uint8>& memory, EntityHandle entity,
		BaseECSComponent* comp);
typedef void (*ECSComponentFreeFunction)(BaseECSComponent* comp);

#define NULL_ENTITY_HANDLE nullptr

struct ECSComponentTypeData {
	inline ECSComponentTypeData(ECSComponentCreateFunction createfn,
				ECSComponentFreeFunction freefn, size_t size)
			: createfn(createfn)
			, freefn(freefn)
			, size(size) {}

	ECSComponentCreateFunction createfn;
	ECSComponentFreeFunction freefn;
	size_t size;
};

class BaseECSComponent {
	public:
		static uint32 registerComponentType(ECSComponentCreateFunction createfn,
				ECSComponentFreeFunction freefn, size_t size);

		inline static ECSComponentCreateFunction getTypeCreateFunction(uint32 id) {
			return (*componentTypes)[id].createfn;
		}

		inline static ECSComponentFreeFunction getTypeFreeFunction(uint32 id) {
			return (*componentTypes)[id].freefn;
		}

		inline static size_t getTypeSize(uint32 id) {
			return (*componentTypes)[id].size;
		}

		inline static bool isTypeValid(uint32 id) {
			return id < componentTypes->size();
		}

		EntityHandle entity = NULL_ENTITY_HANDLE;
	private:
		static ArrayList<ECSComponentTypeData>* componentTypes;
};

template <typename T>
class ECSComponent: public BaseECSComponent {
	public:
		static const uint32 ID;
		static const size_t SIZE;

		static const ECSComponentCreateFunction CREATE_FUNCTION;
		static const ECSComponentFreeFunction FREE_FUNCTION;
};

template<typename Component>
uint32 ECSComponentCreate(ArrayList<uint8>& memory, EntityHandle entity, BaseECSComponent* comp) {
	uint32 index = memory.size();
	memory.resize(index + Component::SIZE);
	Component* component = new(&memory[index]) Component(*((Component*)comp));
	component->entity = entity;

	return index;
}

template <typename Component>
void ECSComponentFree(BaseECSComponent* comp) {
	Component* component = (Component*)comp;
	component->~Component();
}

template <typename T>
const uint32 ECSComponent<T>::ID(BaseECSComponent::registerComponentType(ECSComponentCreate<T>,
			ECSComponentFree<T>, sizeof(T)));

template <typename T>
const size_t ECSComponent<T>::SIZE(sizeof(T));

template <typename T>
const ECSComponentCreateFunction ECSComponent<T>::CREATE_FUNCTION(ECSComponentCreate<T>);

template <typename T>
const ECSComponentFreeFunction ECSComponent<T>::FREE_FUNCTION(ECSComponentFree<T>);

