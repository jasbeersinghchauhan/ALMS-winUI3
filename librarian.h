#ifndef LIBRARIAN_H
#define LIBRARIAN_H

#include <pch.h>
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include "account.h"
#include "book.h"
#include "borrowedBooks.h"
#include <mysql_connection.h>

class librarian {
public:
    // Account-related functions
    void viewAccountById(int accountId, std::function<void(std::string)> showMessage,
        std::function<void(Account)> displayAccount);
    void viewAccountByEmail(const std::string& email, std::function<void(std::string)> showMessage,
        std::function<void(Account)> displayAccount);
    void viewAllAccounts(std::function<void(Account)> displayAccount, std::function<void(std::string)> showMessage);

    // Book-related functions
    void addBook(const std::string& title, const std::string& author,
        const std::string& isbn, int year, int quantity,
        std::function<void(std::string)> showMessage);

    void updateBook(const std::string& isbn, const std::string& title, const std::string& author,
        int year, int quantity, std::function<void(std::string)> showMessage);
    void deleteBook(const std::string& isbn, std::function<void(std::string)> showMessage);
    void viewAllBooks(std::function<void(Book)> displayBook, std::function<void(std::string)> showMessage);
    static void viewAvailableBooks(std::function<void(Book)> displayBook, std::function<void(std::string)> showMessage);

    // Borrowing-related functions
    void viewAllBorrowedBooks(std::function<void(BorrowedBook)> displayBorrowedBook, std::function<void(std::string)> showMessage);
    void viewBorrowedBooksByAccountId(int accountId, std::function<void(BorrowedBook)> displayBorrowedBook, std::function<void(std::string)> showMessage);
    void allocateBook(int accountId, int bookId, std::function<void(std::string)> showMessage, std::function<void(bool)> onResult);
    void revertBookAllocation(int accountId, int bookId, std::function<void(std::string)> showMessage, std::function<void(bool)> onResult);
};

#endif
