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

    // Add a new account
    void addAccount(sql::Connection* con, const std::string& name, const std::string& email,
        const std::string& password, const std::string& phone, const std::string& role,
        std::function<void(std::string)> showMessage);

    // View account by ID
    void viewAccountById(sql::Connection* con, int accountId,
        std::function<void(std::string)> showMessage,
        std::function<void(Account)> displayAccount);

    // View account by email
    void viewAccountByEmail(sql::Connection* con, const std::string& email,
        std::function<void(std::string)> showMessage,
        std::function<void(Account)> displayAccount);

    // Update an existing account
    void updateAccount(sql::Connection* con, int accountId, const std::string& name,
        const std::string& email, const std::string& phone, const std::string& role,
        std::function<void(std::string)> showMessage);

    // Update account role
    void updateRole(sql::Connection* con, int currentAdminId, int targetAccountId,
        const std::string& newRole, std::function<void(std::string)> showMessage);

    // Delete an account
    void deleteAccount(sql::Connection* con, int accountId, std::function<void(std::string)> showMessage);

    // View all accounts
    void viewAllAccounts(sql::Connection* con, std::function<void(Account)> displayAccount);

    // Add a new book
    void addBook(sql::Connection* con, const std::string& title, const std::string& author,
        const std::string& isbn, int year, int quantity, std::function<void(std::string)> showMessage);

    // Update book details
    void updateBook(sql::Connection* con, const std::string& isbn, const std::string& title,
        const std::string& author, int year, int quantity, std::function<void(std::string)> showMessage);

    // Delete a book
    void deleteBook(sql::Connection* con, const std::string& isbn, std::function<void(std::string)> showMessage);

    // View all books
    void viewAllBooks(sql::Connection* con, std::function<void(Book)> displayBook);

    // View all borrowed books
    void viewAllBorrowedBooks(sql::Connection* con, std::function<void(BorrowedBook)> displayBorrowedBook);

    // View borrowed books by account ID
    void viewBorrowedBooksByAccountId(sql::Connection* con, int accountId,
        std::function<void(BorrowedBook)> displayBorrowedBook);
};