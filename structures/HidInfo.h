//
// Created by ParticleG on 2022/3/28.
//

#pragma once

#include <drogon/drogon.h>
#include <hidapi/hidapi.h>

namespace hephaestus::structures {
    class HidInfo {
    public:
        explicit HidInfo(struct hid_device_info *info);

        const std::string path;
        const std::wstring manufacturer_string, product_string, serial_number;
        const uint16_t product_id, release_number, usage_page, usage, vendor_id;
        const int32_t collection_number{-1}, interface_number{-1};

    private:
        static constexpr int32_t getCollectionNumber(const std::string &str) {
            auto pos = str.find("&col");
            return pos == std::string::npos ? -1 : std::stoi(str.substr(pos + 4, 2));
        }
    };
}