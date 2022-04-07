//
// Created by particleg on 2021/10/2.
//

#pragma once

#include <drogon/drogon.h>
#include <helpers/MessageJson.h>
#include <types/HidAction.h>
#include <types/JsonValue.h>
#include <types/MessageType.h>
#include <types/ResultCode.h>

namespace hephaestus::internal {
    class BaseException : public std::exception {
    public:
        explicit BaseException(std::string message);

        ~BaseException() noexcept override = default;

        [[nodiscard]] const char *what() const noexcept override;

    protected:
        const std::string _message;
    };
}

namespace hephaestus::structures {
    class ResponseException : public internal::BaseException {
    public:
        explicit ResponseException(
                std::string message,
                const types::ResultCode &code = types::ResultCode::internalError,
                const drogon::HttpStatusCode &statusCode = drogon::HttpStatusCode::k500InternalServerError
        );

        explicit ResponseException(
                std::string message,
                const std::exception &e,
                const types::ResultCode &code = types::ResultCode::internalError,
                const drogon::HttpStatusCode &statusCode = drogon::HttpStatusCode::k500InternalServerError
        );

        [[nodiscard]] const types::ResultCode &code() const noexcept;

        [[nodiscard]] const drogon::HttpStatusCode &statusCode() const noexcept;

        [[nodiscard]] Json::Value toJson() const noexcept;

    private:
        const std::string _reason;
        const types::ResultCode _code;
        const drogon::HttpStatusCode _statusCode;
    };

    class MessageException : public internal::BaseException {
    public:
        explicit MessageException(std::string message, bool error = false);

        std::atomic<bool> error;
    };

    class HidException : public internal::BaseException {
    public:
        explicit HidException(std::string message, types::HidAction action);

        std::atomic<types::HidAction> action;
    };

    namespace json_exception {
        class InvalidFormat : public internal::BaseException {
        public:
            explicit InvalidFormat(std::string message);
        };

        class WrongType : public internal::BaseException {
        public:
            explicit WrongType(const types::JsonValue &valueType);
        };
    }

    namespace sql_exception {
        class EmptyValue : public internal::BaseException {
        public:
            explicit EmptyValue(std::string message);
        };

        class NotEqual : public internal::BaseException {
        public:
            explicit NotEqual(std::string message);
        };
    }

    namespace redis_exception {
        class KeyNotFound : public internal::BaseException {
        public:
            explicit KeyNotFound(std::string message);
        };

        class FieldNotFound : public internal::BaseException {
        public:
            explicit FieldNotFound(std::string message);
        };

        class NotEqual : public internal::BaseException {
        public:
            explicit NotEqual(std::string message);
        };
    }
}
