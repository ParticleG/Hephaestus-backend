//
// Created by Particle_G on 2021/3/04.
//

#include <helpers/MessageJson.h>
#include <magic_enum.hpp>
#include <plugins/LedManager.h>
#include <strategies/LedEffect.h>
#include <types/Action.h>
#include <types/JsonValue.h>

using namespace drogon;
using namespace magic_enum;
using namespace std;
using namespace hephaestus::helpers;
using namespace hephaestus::plugins;
using namespace hephaestus::strategies;
using namespace hephaestus::structures;
using namespace hephaestus::types;

LedEffect::LedEffect() : MessageHandlerBase(enum_integer(Action::ledEffect)) {}

bool LedEffect::filter(const WebSocketConnectionPtr &wsConnPtr, RequestJson &request) {
    if (!request.check("path", JsonValue::String) ||
        !request.check("config", JsonValue::Object)) {
        MessageJson message(_action);
        message.setMessageType(MessageType::failed);
        message.setReason(i18n("invalidArguments"));
        message.sendTo(wsConnPtr);
        return false;
    }
    return true;
}

void LedEffect::process(const WebSocketConnectionPtr &wsConnPtr, RequestJson &request) {
    handleExceptions([&]() {
        if (app().getPlugin<LedManager>()->ledEffect(
                move(request["path"].asString()),
                move(request.ref()["config"])
        )) {
            MessageJson message(_action);
            message.sendTo(wsConnPtr);
        } else {
            MessageJson message(_action);
            message.setMessageType(MessageType::failed);
            message.setReason(i18n("deviceNotFound"));
            message.sendTo(wsConnPtr);
        }
    }, _action, wsConnPtr);
}
