#include "dbConnection.h"

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cppconn/driver.h>
#include <cppconn/exception.h>

std::unique_ptr<sql::Connection> connectToDatabase(std::function<void(std::string)> showMessage) {
    try {
        static sql::Driver* driver = get_driver_instance();
        const char* user = std::getenv("DB_USER");
        const char* pass = std::getenv("DB_PASSWORD");
        const char* host = std::getenv("DB_HOST");

        if (!user || !pass || !host) {
            showMessage("Environment variables for DB not set.");
            return nullptr;
        }

        std::unique_ptr<sql::Connection> con(driver->connect(host, user, pass));
        con->setSchema("library_db");

        showMessage("Connected to the database successfully.");
        return con;
    }
    catch (sql::SQLException& e) {
        showMessage("SQL Error: " + std::string(e.what()));
        return nullptr;
    }
    catch (const std::runtime_error& e) {
        showMessage("Runtime Error: " + std::string(e.what()));
        return nullptr;
    }
    catch (...) {
        showMessage("Unknown error: Unable to connect to database.");
        return nullptr;
    }
}