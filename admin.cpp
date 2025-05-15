#include "pch.h"
#include "admin.h"
#include "account.h"
#include "book.h"
#include "borrowedBooks.h"
#include "DatabaseService.h"

void Admin::addAccount(const std::string& name, const std::string& email, const std::string& password,
    const std::string& phone, const std::string& role,
    std::function<void(std::string)> showMessage) {

    if (role != "admin" && role != "user" && role != "librarian") {
        showMessage("Invalid role. Please enter 'admin', 'user', or 'librarian'.");
        return;
    }

    DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
        Account newAccount(name, email, password, phone, role);
        std::string resultMessage = newAccount.insertAccount(con);
        showMessage(resultMessage);
        });
}


void Admin::viewAccountById(int accountId, std::function<void(std::string)> showMessage,
    std::function<void(Account)> displayAccount) {

    DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
        Account account(accountId);
        std::string resultMessage = account.searchAccountByID(con);
        showMessage(resultMessage);
        if (account.getAccountId() != 0) {
            displayAccount(account);
        }
        });
}


void Admin::viewAccountByEmail(const std::string& email, std::function<void(std::string)> showMessage,
    std::function<void(Account)> displayAccount) {
    DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
        Account account(email);
        std::string resultMessage = account.searchAccountByEmail(con);
        showMessage(resultMessage);
        if (!account.getEmail().empty()) {
            displayAccount(account);
        }
        });
}

void Admin::updateAccount(int accountId, const std::string& name, const std::string& email,
    const std::string& phone, const std::string& role,
    std::function<void(std::string)> showMessage) {

    DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
        Account acc(accountId);
        acc.searchAccountByID(con);

        if (acc.getAccountId() != 0) {
            if (!role.empty() && role != "admin" && role != "user" && role != "librarian") {
                showMessage("Invalid role. Please enter 'admin', 'user', or 'librarian'.");
                return;
            }

            if (!name.empty()) acc.setName(name);
            if (!email.empty()) acc.setEmail(email);
            if (!phone.empty()) acc.setPhone(phone);
            if (!role.empty()) acc.setRole(role);

            std::string resultMessage = acc.updateAccount(con);
            showMessage(resultMessage);
        }
        else {
            showMessage("Account not found with ID: " + std::to_string(accountId));
        }
        });
}

void Admin::updateRole(int currentAdminId, int targetAccountId, const std::string& newRole,
    std::function<void(std::string)> showMessage) {
    DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
        Account current(currentAdminId);
        current.searchAccountByID(con);
        if (current.getRole() != "admin") {
            showMessage("Access denied. Only admins can update roles.");
            return;
        }
        if (newRole != "admin" && newRole != "user" && newRole != "librarian") {
            showMessage("Invalid role. Please enter 'admin', 'user', or 'librarian'.");
            return;
        }
        Account target(targetAccountId);
        target.setRole(newRole);
        std::string resultMessage = target.updateRole(con);
        showMessage(resultMessage);
        });
}

void Admin::deleteAccount(int accountId, std::function<void(std::string)> showMessage) {
    DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
        Account acc(accountId);
        acc.searchAccountByID(con);

        if (acc.getAccountId() != 0) {
            std::string resultMessage = acc.deleteAccount(con);
            showMessage(resultMessage);
        }
        else {
            showMessage("Account not found with ID: " + std::to_string(accountId));
        }
        });
}

void Admin::viewAllAccounts(std::function<void(Account)> displayAccount, std::function<void(std::string)> showMessage) {
    DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
        Account acc;
        std::string error;
        auto accounts = acc.displayAllAccounts(con, error);
        if (!error.empty()) {
            showMessage("Error fetching accounts: " + error);
            return;
        }
        if (accounts.empty()) {
            showMessage("No accounts found.");
        }
        else {
            for (const auto& a : accounts) {
                displayAccount(a);
            }
        }
        });
}


void Admin::addBook(const std::string& title, const std::string& author,
    const std::string& isbn, int year, int quantity,
    std::function<void(std::string)> showMessage) {
    DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
        Book book(title, author, isbn, year, quantity);
        std::string resultMessage = book.insertBook(con); 
        showMessage(resultMessage);                        
        });
}


void Admin::updateBook(const std::string& isbn, const std::string& title, const std::string& author,
    int year, int quantity, std::function<void(std::string)> showMessage) {

    DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
        Book book(isbn);
        std::string searchResult = book.searchBookByISBN(con);
        if (searchResult.empty()) {
            if (!title.empty()) book.setTitle(title);
            if (!author.empty()) book.setAuthor(author);
            if (year > 0) book.setYear(year);
            if (quantity >= 0) book.setQuantity(quantity);

            std::string updateResult = book.updateBook(con);
            showMessage(updateResult);
        }
        else {
            showMessage(searchResult);
        }
        });
}


void Admin::deleteBook(const std::string& isbn, std::function<void(std::string)> showMessage) {
    DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
        Book book(isbn);
        std::string searchResult = book.searchBookByISBN(con);

        if (!book.getISBN().empty()) {
            std::string resultMessage = book.deleteBook(con);
            showMessage(resultMessage);
        }
        else {
            showMessage("Book not found with ISBN: " + isbn);
        }
        });
}


void Admin::viewAllBooks(std::function<void(Book)> displayBook, std::function<void(std::string)> showMessage) {
    DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
        Book book;
        std::string error;
        auto books = book.displayAllBooks(con, error);

        if (!error.empty()) {
            showMessage("Error fetching books: " + error);
            return;
        }
        if (books.empty()) {
            showMessage("No books found.");
        }
        else {
            for (const auto& b : books) {
                displayBook(b);
            }
        }
    });
}


void Admin::viewAllBorrowedBooks(std::function<void(BorrowedBook)> displayBorrowedBook,
    std::function<void(std::string)> showMessage) {
    DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
        BorrowedBooksManager manager(con);
        auto borrowed = manager.getAllBorrowedBooks(showMessage);
        for (const auto& book : borrowed) {
            displayBorrowedBook(book);
        }
        });
}


void Admin::viewBorrowedBooksByAccountId(int accountId, std::function<void(BorrowedBook)> displayBorrowedBook, 
    std::function<void(std::string)> showMessage) {
    DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
        BorrowedBooksManager manager(con);
        auto borrowed = manager.getBorrowedBooksByUserID(accountId, showMessage);
        for (const auto& book : borrowed) {
            displayBorrowedBook(book);
        }
        });
}
