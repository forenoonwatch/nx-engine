#pragma once

#include <utility>

template <typename T>
class Service {
    public:
        template <typename... Args>
        static inline void init(Args&&... args) {
            instance = new T(std::forward<Args>(args)...);
        }

        static inline T* get() {
            return instance;
        }

        static inline T& ref() {
            return *instance;
        }

        static inline void destroy() {
            delete instance;
            instance = nullptr;
        }
    private:
        static inline T* instance = nullptr;
};
