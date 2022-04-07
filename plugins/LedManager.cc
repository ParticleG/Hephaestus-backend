//
// Created by ParticleG on 2022/3/28.
//

#include <drogon/drogon.h>
#include <plugins/LedManager.h>

using namespace drogon;
using namespace hephaestus::plugins;
using namespace hephaestus::structures;
using namespace std;

void LedManager::initAndStart(const Json::Value &config) {
    LOG_INFO << "LedManager loaded.";
}

void LedManager::shutdown() {
    LOG_INFO << "LedManager shutdown.";
}

bool LedManager::subscribe(const string &path, uint8_t count) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    return _deviceMap.try_emplace(path, path, count).second;
}

void LedManager::unsubscribe(const string &path) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    _deviceMap.erase(path);
}

bool LedManager::ledEffect(string &&path, Json::Value &&effect) {
    shared_lock<shared_mutex> lock(_sharedMutex);
    if (_deviceMap.contains(path)) {
        _deviceMap.at(path).processConfig(move(effect));
        return true;
    } else {
        return false;
    }
}




