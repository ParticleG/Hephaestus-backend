//
// Created by Parti on 2021/3/27.
//

#include <controllers/Hid.h>
#include <magic_enum.hpp>

using namespace drogon;
using namespace magic_enum;
using namespace std;
using namespace hephaestus::helpers;
using namespace hephaestus::plugins;
using namespace hephaestus::structures;
using namespace hephaestus::ws::v2;
using namespace hephaestus::types;

void Hid::handleNewConnection(
        const HttpRequestPtr &req,
        const WebSocketConnectionPtr &wsConnPtr
) {
    wsConnPtr->setPingMessage("", chrono::seconds(10));
}

void Hid::handleConnectionClosed(const WebSocketConnectionPtr &wsConnPtr) {}

std::string Hid::reason(const string &param) const { return i18n(param); }
