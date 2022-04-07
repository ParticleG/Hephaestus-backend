//
// Created by Parti on 2021/2/19.
//

#include <magic_enum.hpp>
#include <plugins/HandlerManager.h>
#include <strategies/LedConnect.h>
#include <strategies/LedEffect.h>
#include <types/Action.h>

using namespace drogon;
using namespace magic_enum;
using namespace std;
using namespace hephaestus::plugins;
using namespace hephaestus::strategies;
using namespace hephaestus::structures;
using namespace hephaestus::types;
using namespace hephaestus::utils;

void HandlerManager::initAndStart(const Json::Value &config) {
    _handlerFactory.registerHandler<LedConnect>(enum_integer(Action::ledConnect));
    _handlerFactory.registerHandler<LedEffect>(enum_integer(Action::ledEffect));
    LOG_INFO << "HandlerManager loaded.";
}

void HandlerManager::shutdown() {
    LOG_INFO << "HandlerManager shutdown.";
}
