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
    void viewAccountById(sql::Connection* con, int accountId, std::function<void(std::string)> showMessage, std::function<void(Account)> displayAccount);
    void viewAccountByEmail(sql::Connection* con, const std::string& email, std::function<void(std::string)> showMessage, std::function<void(Account)> displayAccount);
    void viewAllAccounts(sql::Connection* con, std::function<void(Account)> displayAccount);

    // Book-related functions
    void addBook(sql::Connection* con, const Book& newBook);
    void updateBook(sql::Connection* con, const Book& updatedBook);
    void deleteBook(sql::Connection* con, const std::string& isbn);
    void viewAllBooks(sql::Connection* con, std::function<void(Book)> displayBook);
    void viewAvailableBooks(sql::Connection* con, std::vector<Book>& outBooks);

    // Borrowing-related functions
    void viewAllBorrowedBooks(sql::Connection* con, std::vector<BorrowedBook>& outBorrowedBooks);
    void viewBorrowedBooksByAccountId(sql::Connection* con, int accountId, std::vector<BorrowedBook>& outBorrowedBooks);
    bool allocateBook(sql::Connection* con, int accountId, int bookId);
    bool revertBookAllocation(sql::Connection* con, int accountId, int bookId);
};

#endif