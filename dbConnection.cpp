#include "pch.h"
#include "dbConnection.h"

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cppconn/driver.h>
#include <cppconn/exception.h>

std::unique_ptr<sql::Connection> connectToDatabase(std::function<void(std::string)> showMessage) {
    char* user = nullptr;
    char* pass = nullptr;
    char* host = nullptr;
    try {
        static sql::Driver* driver = get_driver_instance();
        size_t len = 0;

        if (_dupenv_s(&user, &len, "DB_USER") != 0 || user == nullptr ||
            _dupenv_s(&pass, &len, "DB_PASSWORD") != 0 || pass == nullptr ||
            _dupenv_s(&host, &len, "DB_HOST") != 0 || host == nullptr) {
            throw std::runtime_error("Missing or failed to read one or more required environment variables.");
        }

        if (!user || !pass || !host) {
            showMessage("Environment variables for DB not set.");
        }

        std::unique_ptr<sql::Connection> con(driver->connect(host, user, pass));
        con->setSchema("library_db");
        free(user);
        free(pass);
        free(host);

        showMessage("Connected to the database successfully.");
        return con;
    }
    catch (sql::SQLException& e) {
        showMessage("SQL Error: " + std::string(e.what()));
    }
    catch (const std::runtime_error& e) {
        showMessage("Runtime Error: " + std::string(e.what()));
    }
    catch (...) {
        showMessage("Unknown error: Unable to connect to database.");
    }
    if (user) free(user);
    if (pass) free(pass);
    if (host) free(host);
    return nullptr;
}
