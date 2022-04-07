//
// Created by Particle_G on 2021/3/04.
//

#include <helpers/MessageJson.h>
#include <magic_enum.hpp>
#include <plugins/LedManager.h>
#include <strategies/LedConnect.h>
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

LedConnect::LedConnect() : MessageHandlerBase(enum_integer(Action::ledConnect)) {}

bool LedConnect::filter(const WebSocketConnectionPtr &wsConnPtr, RequestJson &request) {
    if (!request.check("count", JsonValue::UInt) || !(
            request.check("path", JsonValue::String) ||
            (request.check("vid", JsonValue::UInt) && request.check("pid", JsonValue::UInt))
    )) {
        MessageJson message(_action);
        message.setMessageType(MessageType::failed);
        message.setReason(i18n("invalidArguments"));
        message.sendTo(wsConnPtr);
        return false;
    }
    return true;
}

void LedConnect::process(const WebSocketConnectionPtr &wsConnPtr, RequestJson &request) {
    handleExceptions([&]() {
        string path;
        if (request.check("path", JsonValue::String)) {
            path = request["path"].asString();
        } else {
            uint16_t vid = request["vid"].asUInt(), pid = request["pid"].asUInt();
            int32_t col = request.check("col", JsonValue::Int) ? request["col"].asInt() : -1,
                    inf = request.check("inf", JsonValue::Int) ? request["inf"].asInt() : -1;
            path = app().getPlugin<HidManager>()->getPath(vid, pid, col, inf);
        }
        if (app().getPlugin<LedManager>()->subscribe(path, request["count"].asUInt())) {
            MessageJson message(_action);
            message.setData(path);
            message.sendTo(wsConnPtr);
        } else {
            MessageJson message(_action);
            message.setMessageType(MessageType::failed);
            message.setData(path);
            message.setReason(i18n("alreadyConnected"));
            message.sendTo(wsConnPtr);
        }
    }, _action, wsConnPtr);
}
