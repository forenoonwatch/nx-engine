#pragma once

#include <engine/core/common.hpp>
#include <engine/core/memory.hpp>

#include <utility>

template <typename Service>
class ServiceLocator {
	public:
		ServiceLocator() = delete;

		static bool empty() noexcept { return !static_cast<bool>(service); }

		static Memory::WeakPointer<Service> get() noexcept { return service; }

		static Service& ref() noexcept { return *service; }

		template <typename Impl = Service, typename... Args>
		static void set(Args&&... args) {
			service = Memory::make_shared<Impl>(std::forward<Args>(args)...);
		}
			
		static void set(Memory::SharedPointer<Service> ptr) {
			service = std::move(ptr);
		}

		static void reset() {
			service.reset();
		}

		~ServiceLocator() = delete;
	private:
		inline static Memory::SharedPointer<Service> service = nullptr;
};

