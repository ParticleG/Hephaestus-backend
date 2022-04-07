//
// Created by Parti on 2021/2/19.
//

#pragma once

#include <memory>
#include <unordered_map>
#include <shared_mutex>
#include <structures/Exceptions.h>

namespace hephaestus::structures {
    template<class baseHandler>
    class HandlerFactory final {
    public:
        HandlerFactory() = default;

        ~HandlerFactory() = default;

        HandlerFactory(const HandlerFactory &) = delete;

        const HandlerFactory &operator=(const HandlerFactory &) = delete;

        template<class Handler>
        void registerHandler(const int &action) {
            std::unique_lock<std::shared_mutex> lock(_sharedMutex);
            _handlerRegistrarsMap[action] = std::make_unique<Handler>();
        }

        baseHandler &getHandler(const int &action) {
            std::shared_lock<std::shared_mutex> lock(_sharedMutex);
            return *_handlerRegistrarsMap.at(action);
        }

    private:
        mutable std::shared_mutex _sharedMutex;
        std::unordered_map<int, std::unique_ptr<baseHandler>> _handlerRegistrarsMap;
    };
}