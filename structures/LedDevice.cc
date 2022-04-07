//
// Created by ParticleG on 2022/3/28.
//

#include <magic_enum.hpp>
#include <helpers/RequestJson.h>
#include <plugins/Perfmon.h>
#include <structures/LedDevice.h>
#include <types/JsonValue.h>
#include <utils/color.h>

using namespace drogon;
using namespace magic_enum;
using namespace hephaestus::helpers;
using namespace hephaestus::plugins;
using namespace hephaestus::structures;
using namespace hephaestus::types;
using namespace hephaestus::utils;
using namespace std;

LedDevice::LedDevice(const string &path, uint8_t ledCount) :
        hidDevice(app().getPlugin<HidManager>()->getDevice(path)),
        ledCount(ledCount) {
    initTime();
    animationLoopId = app().getLoop()->runEvery(
            chrono::milliseconds(ANIMATION_LOOP_MS),
            [this]() {
                switch (animationType) {
                    case AnimationType::breathing:
                        breathing();
                        break;
                    case AnimationType::colorCycle:
                        colorCycle();
                        break;
                    case AnimationType::comet:
                    case AnimationType::custom:
                    case AnimationType::dark:
                        dark();
                        break;
                    case AnimationType::game:
                        game();
                        break;
                    case AnimationType::music:
                    case AnimationType::rainbow:
                        rainbow();
                        break;
                    case AnimationType::simple:
                        simple();
                        break;
                    case AnimationType::smart:
                        smart();
                        break;
                    case AnimationType::star:
                    case AnimationType::starryNight:
                        starryNight();
                        break;
                    case AnimationType::strobing:
                        strobing();
                        break;
                    case AnimationType::tide:
                    case AnimationType::trigger:
                        dark();
                        break;
                }
            }
    );
}

LedDevice::~LedDevice() {
    app().getLoop()->invalidateTimer(animationLoopId);
}

void LedDevice::processConfig(Json::Value &&config) {
    {
        shared_lock<shared_mutex> lock(_sharedMutex);
        if (_config == config) {
            return;
        }
    }
    {
        unique_lock<shared_mutex> lock(_sharedMutex);
        _config = config;
    }
    RequestJson requestJson(move(config));
    LOG_INFO << "Config: " << requestJson.stringify("  ");
    if (requestJson.check("animationType", JsonValue::String)) {
        auto type = enum_cast<AnimationType>(requestJson["animationType"].asString());
        if (type) {
            auto castedType = type.value();

            if (castedType != animationType) {
                initTime();
            }

            animationType = castedType;
        }
    }
    if (requestJson.check("animationDirection", JsonValue::Bool)) {
        animationDirection = requestJson["animationDirection"].asBool();
    }
    if (requestJson.check("minPeriod", JsonValue::Double)) {
        minPeriod = requestJson["minPeriod"].asDouble();
    }
    if (requestJson.check("maxPeriod", JsonValue::Double)) {
        maxPeriod = requestJson["maxPeriod"].asDouble();
    }
    if (requestJson.check("startColor", JsonValue::String)) {
        unique_lock<shared_mutex> lock(_sharedMutex);
        startColor = rgb2hsv(stoul(requestJson["startColor"].asString().substr(1), nullptr, 16));
    }
    if (requestJson.check("endColor", JsonValue::UInt64)) {
        unique_lock<shared_mutex> lock(_sharedMutex);
        endColor = rgb2hsv(requestJson["endColor"].asUInt());
    }
    once = true;
}

void LedDevice::initTime() {
    mainLastMs = duration_cast<chrono::milliseconds>(
            chrono::high_resolution_clock::now().time_since_epoch()
    ).count();

    if (maxPeriod != 0) {
        mainPhase = fmod(static_cast<double>(mainLastMs), maxPeriod);
    } else {
        mainPhase = 0;
    }
}

void LedDevice::breathing() {
    const auto timeNorm = getNormalizedTime();

    shared_lock<shared_mutex> lock(_sharedMutex);
    auto value = startColor[2] * ((timeNorm < 0.5) ? timeNorm : 1 - timeNorm);
    setLeds({startColor[0], startColor[1], value});
}

void LedDevice::colorCycle() {
    const auto timeNorm = getNormalizedTime();

    shared_lock<shared_mutex> lock(_sharedMutex);
    setLeds({fmod(startColor[0] + timeNorm * 360.0, 360.0),
             startColor[1],
             startColor[2]});
}

void LedDevice::dark() {
    if (once) {
        once = false;
        setLeds(HSV{});
    }
}

void LedDevice::game() {
    vector<HSV> leds;
    leds.resize(ledCount, {});

    setLeds(leds);
}

void LedDevice::simple() {
    if (once) {
        once = false;
        shared_lock<shared_mutex> lock(_sharedMutex);
        setLeds(startColor);
    }
}

void LedDevice::rainbow() {
    const auto timeNorm = getNormalizedTime();
    vector<HSV> leds;

    shared_lock<shared_mutex> lock(_sharedMutex);
    leds.resize(ledCount, startColor);
    for (auto i = 0; i < ledCount; i++) {
        leds[i][0] = fmod(
                startColor[0] + (360.0 / 16.0) * i + 360.0 * timeNorm,
                360.0
        );
    }
    setLeds(leds);
}

void LedDevice::smart() {
    const auto info = app().getPlugin<Perfmon>()->parseInfo();
    const auto cpuUsage = info["cpu"].asDouble();

    shared_lock<shared_mutex> lock(_sharedMutex);
    setLeds({fmod(startColor[0] + cpuUsage * 360.0, 360.0),
             startColor[1],
             startColor[2]});
}

void LedDevice::starryNight() {
    const auto timeNorm = getNormalizedTime();

    shared_lock<shared_mutex> lock(_sharedMutex);
    auto value = startColor[2] * ((timeNorm < 0.5) ? timeNorm : 1 - timeNorm);
    setLeds({startColor[0], startColor[1], value});
}

void LedDevice::strobing() {
    const auto timeNorm = getNormalizedTime();

    shared_lock<shared_mutex> lock(_sharedMutex);
    setLeds({startColor[0],
             startColor[1],
             timeNorm < 0.5 ? startColor[2] : 0});
}

double LedDevice::getNormalizedTime() {
    using namespace std::chrono;
    return getNormalizedTime(
            maxPeriod,
            duration_cast<milliseconds>(
                    high_resolution_clock::now().time_since_epoch()
            ).count(),
            mainLastMs,
            mainPhase
    );
}

double LedDevice::getNormalizedTime(
        double period,
        const int64_t &nowMs,
        atomic<int64_t> &lastMs,
        atomic<double> &phase
) const {
    using namespace std::chrono;
    if (period != 0) {
        const auto delta = static_cast<double>(nowMs - lastMs);
        lastMs = nowMs;

        phase = animationDirection ? phase + delta : phase - delta;
        if ((phase = fmod(phase, period)) < 0) { phase += period; }

        return phase / period;
    } else {
        return 0;
    }
}

void LedDevice::setLeds(const vector<HSV> &colors) {
    vector<uint8_t> data{REPORT_ID, ENABLE_FLOW};
    data.resize(REPORT_LENGTH, 0);

    auto totalRounds = static_cast<int>(ceil(ledCount / static_cast<double>(SEPARATE_LIMIT)));
    for (int round = 0; round < totalRounds; round++) {
        uint8_t tempLedCount;
        if (round == totalRounds - 1) {
            tempLedCount = ledCount % SEPARATE_LIMIT;
            data[1] = DISABLE_FLOW;
        } else {
            tempLedCount = SEPARATE_LIMIT;
        }
        data[2] = tempLedCount;
        for (int ledIndex = 0; ledIndex < tempLedCount; ledIndex++) {
            const auto tempIndex = round * SEPARATE_LIMIT + ledIndex;
            const auto[red, green, blue] = hsv2rgb(colors[tempIndex]);
            data[3 + ledIndex * 4] = INDEX_OFFSET + tempIndex;
            data[4 + ledIndex * 4] = red;
            data[5 + ledIndex * 4] = green;
            data[6 + ledIndex * 4] = blue;
        }
        this_thread::sleep_for(chrono::milliseconds(1));
        hidDevice->write(data);
    }
}

void LedDevice::setLeds(const LedDevice::HSV &color) {
    const auto[red, green, blue] = hsv2rgb(color);
    vector<uint8_t> data{REPORT_ID, ENABLE_FLOW};
    data.resize(REPORT_LENGTH, 0);

    auto totalRounds = static_cast<int>(ceil(ledCount / static_cast<double>(SEPARATE_LIMIT)));
    for (int round = 0; round < totalRounds; round++) {
        uint8_t tempLedCount;
        if (round == totalRounds - 1) {
            tempLedCount = ledCount % SEPARATE_LIMIT;
            data[1] = DISABLE_FLOW;
        } else {
            tempLedCount = SEPARATE_LIMIT;
        }
        data[2] = tempLedCount;
        for (int ledIndex = 0; ledIndex < tempLedCount; ledIndex++) {
            data[3 + ledIndex * 4] = INDEX_OFFSET + round * SEPARATE_LIMIT + ledIndex;
            data[4 + ledIndex * 4] = red;
            data[5 + ledIndex * 4] = green;
            data[6 + ledIndex * 4] = blue;
        }
        this_thread::sleep_for(chrono::milliseconds(1));
        hidDevice->write(data);
    }
}
