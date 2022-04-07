//
// Created by ParticleG on 2022/3/28.
//

#pragma once

#include <shared_mutex>
#include <structures/HidInfo.h>
#include <structures/HidDevice.h>

namespace hephaestus::plugins {
    class HidManager :
            public drogon::Plugin<HidManager> {
    public:
        using TestFunc = std::function<bool(const structures::HidInfo &)>;

        HidManager() = default;

        void initAndStart(const Json::Value &config) override;

        void shutdown() override;

        void updateHidList();

        std::string getPath(
                uint16_t device,
                uint16_t product_id,
                int32_t collection_number = -1,
                int32_t interface_number = -1
        );

        std::shared_ptr<structures::HidDevice> getDevice(const std::string &path);

    private:
        mutable std::shared_mutex _sharedMutex;
        std::vector<structures::HidInfo> _hidList;
        std::unordered_map<std::string, structures::HidDevice> _deviceMap;

        void _subscribe(const std::string &path);

        void _unsubscribe(const std::string &path);

        bool _checkDevice(const TestFunc &func);
    };
}

