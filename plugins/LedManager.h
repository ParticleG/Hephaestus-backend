//
// Created by ParticleG on 2022/3/28.
//

#pragma once

#include <structures/LedDevice.h>

namespace hephaestus::plugins {
    class LedManager : public drogon::Plugin<LedManager> {
    public:
        LedManager() = default;

        void initAndStart(const Json::Value &config) override;

        void shutdown() override;

        bool subscribe(const std::string &path, uint8_t count);

        void unsubscribe(const std::string &path);

        bool ledEffect(std::string &&path, Json::Value &&effect);

    private:
        mutable std::shared_mutex _sharedMutex;
        std::unordered_map<std::string, structures::LedDevice> _deviceMap;
    };
}

