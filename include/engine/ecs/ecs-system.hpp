#pragma once

#include "engine/ecs/ecs-component.hpp"

class ECSSystem {
	public:
		enum {
			FLAG_OPTIONAL = 0x1,
		};

		inline ECSSystem() {}

		virtual void updateComponents(float delta, BaseECSComponent** components) = 0;

		bool isValid() const;

		inline const ArrayList<uint32>& getComponentTypes() const { return componentTypes; }
		inline const ArrayList<uint32>& getComponentFlags() const { return componentFlags; }
	protected:
		inline void addComponentType(uint32 componentType, uint32 componentFlag = 0) {
			componentTypes.push_back(componentType);
			componentFlags.push_back(componentFlag);
		}
	private:
		ArrayList<uint32> componentTypes;
		ArrayList<uint32> componentFlags;
};

class ECSSystemList {
	public:
		inline bool addSystem(ECSSystem& system) {
			if (!system.isValid()) {
				return false;
			}

			systems.push_back(&system);
			return true;
		}

		bool removeSystem(ECSSystem& system);

		inline ECSSystem* operator[](uint32 index) { return systems[index]; }

		inline size_t size() { return systems.size(); }
	private:
		ArrayList<ECSSystem*> systems;
};
