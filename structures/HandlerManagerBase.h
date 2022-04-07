//
// Created by particleg on 2021/10/10.
//

#pragma once

#include <drogon/plugins/Plugin.h>
#include <helpers/I18nHelper.h>
#include <structures/HandlerFactory.h>
#include <structures/MessageHandlerBase.h>
#include <types/MessageType.h>

namespace hephaestus::structures {
    template<class handlerManagerImpl>
    class HandlerManagerBase :
            public drogon::Plugin<handlerManagerImpl>,
            public helpers::I18nHelper<handlerManagerImpl> {
    public:
        void process(
                int action,
                const drogon::WebSocketConnectionPtr &wsConnPtr,
                helpers::RequestJson &request
        ) {
            using namespace helpers;
            using namespace types;
            try {
                auto &handler = _handlerFactory.getHandler(action);
                if (handler.filter(wsConnPtr, request)) {
                    handler.process(wsConnPtr, request);
                }
            } catch (const std::range_error &) {
                MessageJson message(action);
                message.setMessageType(MessageType::failed);
                message.setReason(I18nHelper<handlerManagerImpl>::i18n("actionNotFound"));
                wsConnPtr->send(message.stringify());
            }
        }

        virtual ~HandlerManagerBase() = default;

    protected:
        HandlerFactory<MessageHandlerBase> _handlerFactory;
    };
}
