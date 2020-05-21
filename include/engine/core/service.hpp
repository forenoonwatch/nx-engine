#pragma once

#include <engine/core/memory.hpp>
#include <utility>

template <typename T>
class Service {
    public:
        template <typename... Args>
        static inline void init(Args&&... args) {
            instance = Memory::make_shared<T>(std::forward<Args>(args)...);
        }

        static inline Memory::WeakPointer<T> get() {
            return Memory::WeakPointer<T>(instance);
        }

        static inline T& ref() {
            return *instance;
        }
    private:
        static inline Memory::SharedPointer<T> instance;
};
