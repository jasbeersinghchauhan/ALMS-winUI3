#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <memory>
#include <functional>
#include <cppconn/connection.h>

std::unique_ptr<sql::Connection> connectToDatabase(std::function<void(std::string)> showMessage);

#endif