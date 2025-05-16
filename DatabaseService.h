#pragma once

#include <memory>
#include <functional>
#include <cppconn/connection.h>

class DatabaseService {
public:
    using MessageHandler = std::function<void(std::string)>;

    static bool withConnection(const MessageHandler& showMessage, const std::function<void(sql::Connection*)>& task);
};
