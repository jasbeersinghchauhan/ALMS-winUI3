#pragma once
#include <pch.h>
#include "account.h"
#include "book.h"
#include "borrowedBooks.h"
#include <string>
#include <functional>
#include <vector>
#include <memory>

class Admin {
public:
    Admin() = default;

    void addAccount(const std::string& name, const std::string& email, const std::string& password, const std::string& phone,
        const std::string& role, std::function<void(std::string)> showMessage);

    void viewAccountById(int accountId, std::function<void(std::string)> showMessage, std::function<void(Account)> displayAccount);

    void viewAccountByEmail(const std::string& email, std::function<void(std::string)> showMessage, std::function<void(Account)> displayAccount);

    void updateAccount(int accountId, const std::string& name, const std::string& email, const std::string& phone, const std::string& role,
        std::function<void(std::string)> showMessage);

    void updateRole(int currentAdminId, int targetAccountId, const std::string& newRole, std::function<void(std::string)> showMessage);

    void deleteAccount(int accountId, std::function<void(std::string)> showMessage);

    void viewAllAccounts(std::function<void(Account)> displayAccount, std::function<void(std::string)> showMessage);

    void addBook(const std::string& title, const std::string& author,const std::string& isbn, int year, int quantity,
        std::function<void(std::string)> showMessage);

    void updateBook(const std::string& isbn, const std::string& title, const std::string& author, int year, int quantity, 
        std::function<void(std::string)> showMessage);

    void deleteBook(const std::string& isbn, std::function<void(std::string)> showMessage);

    void viewAllBooks(std::function<void(Book)> displayBook, std::function<void(std::string)> showMessage);

    void viewAllBorrowedBooks(std::function<void(BorrowedBook)> displayBorrowedBook, std::function<void(std::string)> showMessage);

    void viewBorrowedBooksByAccountId(int accountId, std::function<void(BorrowedBook)> displayBorrowedBook, 
        std::function<void(std::string)> showMessage);
};
