#include "pch.h"
#include "DatabaseService.h"
#include "dbConnection.h"

bool DatabaseService::withConnection(const MessageHandler& showMessage, const std::function<void(sql::Connection*)>& task) {
    std::unique_ptr<sql::Connection> con = connectToDatabase(showMessage);
    if (!con) {
        showMessage("Failed to connect to the database.");
        return false;
    }

    try {
        task(con.get());
        return true;
    }
    catch (const std::exception& e) {
        showMessage("Database operation failed: " + std::string(e.what()));
        return false;
    }
}
