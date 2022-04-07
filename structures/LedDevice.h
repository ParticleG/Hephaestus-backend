//
// Created by ParticleG on 2022/3/28.
//

#pragma once

#include <plugins/HidManager.h>
#include <types/AnimationType.h>

namespace hephaestus::structures {
    class LedDevice {
    public:
        typedef std::array<double, 3> HSV;

        const uint8_t REPORT_LENGTH = 65,
                REPORT_ID = 0x06,
                ENABLE_FLOW = 0x11,
                DISABLE_FLOW = 0x10,
                INDEX_OFFSET = 4,
                SEPARATE_LIMIT = 15,
                ANIMATION_LOOP_MS = 33,
                ledCount;

        LedDevice(const std::string &path, uint8_t ledCount);

        ~LedDevice();

        void processConfig(Json::Value &&config);

        const std::shared_ptr<HidDevice> hidDevice;
        std::atomic<uint64_t> animationLoopId;

        //// Animation configs
        std::atomic<types::AnimationType> animationType;
        std::atomic<double> minPeriod, maxPeriod;
        std::atomic<bool> animationDirection{true};

        //// Mutable variables
        mutable std::atomic<bool> once{true};
        mutable std::atomic<double> mainPhase{};
        mutable std::atomic<int64_t> mainLastMs{};

    private:
        mutable std::shared_mutex _sharedMutex;
        Json::Value _config;
        HSV startColor{}, endColor{};

        void initTime();

        void breathing();

        void colorCycle();

        void dark();

        void game();

        void simple();

        void smart();

        void starryNight();

        void strobing();

        void rainbow();

        double getNormalizedTime();

        double getNormalizedTime(
                double period,
                const int64_t &nowMS,
                std::atomic<int64_t> &lastMS,
                std::atomic<double> &phase
        ) const;

        void setLeds(const std::vector<HSV> &colors);

        void setLeds(const HSV &color);
    };
}