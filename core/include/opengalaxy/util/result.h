// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <QString>
#include <optional>
#include <variant>

namespace opengalaxy::util {

/**
 * @brief Result type for error handling without exceptions
 * @tparam T Success value type
 */
template <typename T> class Result {
  public:
    // Success constructor
    static Result success(T value) {
        Result r;
        r.data_ = std::move(value);
        return r;
    }

    // Error constructor
    static Result error(QString message, int code = -1) {
        Result r;
        r.data_ = Error{std::move(message), code};
        return r;
    }

    // Check if result is successful
    bool isOk() const { return std::holds_alternative<T>(data_); }
    bool isError() const { return !isOk(); }

    // Get value (only call if isOk())
    const T &value() const { return std::get<T>(data_); }
    T &value() { return std::get<T>(data_); }
    T valueOr(T defaultValue) const { return isOk() ? value() : std::move(defaultValue); }

    // Get error (only call if isError())
    const QString &errorMessage() const { return std::get<Error>(data_).message; }
    int errorCode() const { return std::get<Error>(data_).code; }

    // Convenience operators
    explicit operator bool() const { return isOk(); }

  private:
    struct Error {
        QString message;
        int code;
    };

    std::variant<T, Error> data_;
};

// Specialization for void (no value)
template <> class Result<void> {
  public:
    static Result success() {
        Result r;
        r.ok_ = true;
        return r;
    }

    static Result error(QString message, int code = -1) {
        Result r;
        r.ok_ = false;
        r.errorMessage_ = std::move(message);
        r.errorCode_ = code;
        return r;
    }

    bool isOk() const { return ok_; }
    bool isError() const { return !ok_; }

    const QString &errorMessage() const { return errorMessage_; }
    int errorCode() const { return errorCode_; }

    explicit operator bool() const { return isOk(); }

  private:
    bool ok_ = false;
    QString errorMessage_;
    int errorCode_ = -1;
};

} // namespace opengalaxy::util
