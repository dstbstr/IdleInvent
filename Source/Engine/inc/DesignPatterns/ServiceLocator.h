#pragma once

#include "Instrumentation/Logging.h"
#include "Utilities/TypeUtils.h"

#include <memory>
#include <map>
#include <ranges>

struct ServiceLocator {
    static ServiceLocator& Get() {
        static ServiceLocator instance{};
		return instance;
	}

    ~ServiceLocator() {
        services.clear();
    }

    ServiceLocator(const ServiceLocator&) = delete;
    ServiceLocator& operator=(const ServiceLocator&) = delete;

    template<typename T, typename... Args>
    void Set(Args&&... args) {
        SetThisAsThat<T, T, Args...>(std::forward<Args>(args)...);
	}

    template<typename TThis, typename TThat, typename... Args>
    void SetThisAsThat(Args&&... args) {
        auto id = Type::Id<TThat>();
        DR_ASSERT_MSG(!services.contains(id), "Service already set");

        services[id] = std::make_shared<TThis>(std::forward<Args>(args)...);
    }

    template<typename T>
    void Reset() {
        services.erase(Type::Id<T>());
	}

    void ResetAll() {
        for(auto& [id, service] : std::views::reverse(services)) {
            service.reset();
        }
        services.clear();
    }

    template<typename T>
    bool IsSet() const {
        return services.contains(Type::Id<T>());
    }

    template<typename T>
    T* Get() const {
        auto id = Type::Id<T>();
        if (services.contains(id)) {
            return static_cast<T*>(services.at(id).get());
        }

        return nullptr;
    }

    template<typename T>
    T& GetRequired() const {
        auto id = Type::Id<T>();
        DR_ASSERT_MSG_LAZY(services.contains(id), []{
            return std::format("Service {} not set", Type::Name<T>());
        });
        auto* ptr = static_cast<T*>(services.at(id).get());
        DR_ASSERT_MSG_LAZY(ptr, []{
            return std::format("Service {} is null", Type::Name<T>());
        });
        return *ptr;
    }

    template<typename T, typename... Args>
    T& GetOrCreate(Args&&... args) {
        auto id = Type::Id<T>();
        if (!services.contains(id)) {
            Set<T>(std::forward<Args>(args)...);
        }

		return *static_cast<T*>(services.at(id).get());
    }

    template<typename T, typename... Args>
    void CreateIfMissing(Args&&... args) {
        if (!services.contains(Type::Id<T>())) {
            Set<T>(std::forward<Args>(args)...);
        }
    }

private:
    ServiceLocator() = default;

    // must be shared_ptr because unique_ptr can't delete void*
    std::map<size_t, std::shared_ptr<void>> services;
};