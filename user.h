#ifndef USER_H
#define USER_H

#include <pch.h>
#include <string>
#include <vector>
#include <memory>
#include <cppconn/connection.h>
#include "account.h"
#include "borrowedBooks.h"
#include "book.h"

class User {
private:
    int accountId;

public:
    User(int id);

    std::string getUserDetails(sql::Connection* con);
    std::vector<BorrowedBook> getBorrowedBooks(sql::Connection* con);
    std::vector<BorrowedBook> getOverdueBooks(sql::Connection* con);
    std::vector<Book> getAvailableBooks(sql::Connection* con);
    double getTotalFine(sql::Connection* con);
    std::vector<Book> getAvailableBooks(sql::Connection* con);
};

#endif