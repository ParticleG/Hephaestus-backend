//
// Created by ParticleG on 2022/3/28.
//

#pragma once

#include <codecvt>
#include <drogon/drogon.h>
#include <helpers/I18nHelper.h>
#include <hidapi/hidapi.h>
#include <locale>
#include <shared_mutex>

namespace hephaestus::structures {
    class HidDevice : public helpers::I18nHelper<HidDevice> {
    public:
        static constexpr char projectName[] = CMAKE_PROJECT_NAME;
    public:
        using MessageHandler = std::function<void(const std::vector<uint8_t> &)>;

        explicit HidDevice(
                const std::string &path,
                uint8_t reportLength = 65,
                bool enableRead = false
        );

        ~HidDevice() override;

        void write(const std::vector<uint8_t> &data);

        bool read(std::vector<uint8_t> &data);

        void setMessageCallback(const MessageHandler &callback);

        std::string error();

        const uint8_t reportLength;

    protected:
        mutable std::shared_mutex _sharedMutex;
        std::atomic<bool> isOpen;
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter{};
        hid_device *_deviceHandle;
        MessageHandler _messageCallback;
    };
}