//
// Created by Parti on 2021/2/5.
//

#pragma once

#include <controllers/BaseWebsocket.h>
#include <helpers/I18nHelper.h>
#include <plugins/HandlerManager.h>

namespace hephaestus::ws::v2 {
    class Hid :
            public BaseWebsocket<Hid, plugins::HandlerManager>,
            public helpers::I18nHelper<Hid> {
    public:
        static constexpr char projectName[] = CMAKE_PROJECT_NAME;

    public:
        WS_PATH_LIST_BEGIN
            WS_PATH_ADD("/hephaestus/ws/v1/hid", "drogon::IntranetIpFilter")
        WS_PATH_LIST_END

        void handleNewConnection(
                const drogon::HttpRequestPtr &req,
                const drogon::WebSocketConnectionPtr &wsConnPtr
        ) override;

        void handleConnectionClosed(const drogon::WebSocketConnectionPtr &wsConnPtr) override;

        [[nodiscard]] std::string reason(const std::string &param) const override;
    };
}