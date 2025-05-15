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

    void getUserDetails(std::function<void(std::string)> showMessage, std::function<void(Account)> displayAccount);
    void getBorrowedBooks(std::function<void(BorrowedBook)> displayBorrowedBook, std::function<void(std::string)> showMessage);
    void getOverdueBooks(std::function<void(BorrowedBook)> displayOverdueBook, std::function<void(std::string)> showMessage);
    void getAvailableBooks(std::function<void(Book)> displayBook, std::function<void(std::string)> showMessage);
    void getTotalFine(std::function<void(double)> displayTotalFine, std::function<void(std::string)> showMessage);
};

#endif
