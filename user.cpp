#include "pch.h"
#include "user.h"
#include "dbConnection.h"

#include <sstream>
#include <ctime>
#include <cstdio>
#include <iomanip>

User::User(int id) : accountId(id) {}

void User::getUserDetails(std::function<void(std::string)> showMessage, std::function<void(Account)> displayAccount) {
    try {
        auto con = connectToDatabase(showMessage);
        if (!con) {
            showMessage("Database connection failed.");
            return;
        }

        Account account(accountId);
        std::string resultMessage = account.searchAccountByID(con.get());
        showMessage(resultMessage);

        if (account.getAccountId() != 0) {
            displayAccount(account);
        }
    }
    catch (const std::exception& e) {
        showMessage(std::string("Exception in getUserDetails: ") + e.what());
    }
}

void User::getBorrowedBooks(std::function<void(BorrowedBook)> displayBorrowedBook, std::function<void(std::string)> showMessage) {
    try {
        auto con = connectToDatabase(showMessage);
        if (!con) {
            showMessage("Database connection failed.");
            return;
        }

        BorrowedBooksManager manager(con.get());
        auto borrowed = manager.getBorrowedBooksByUserID(accountId, showMessage);
        for (const auto& book : borrowed) {
            displayBorrowedBook(book);
        }
    }
    catch (const std::exception& e) {
        showMessage(std::string("Exception in getBorrowedBooks: ") + e.what());
    }
}

void User::getOverdueBooks(std::function<void(BorrowedBook)> displayOverdueBook, std::function<void(std::string)> showMessage) {
    try {
        auto con = connectToDatabase(showMessage);
        if (!con) {
            showMessage("Database connection failed.");
            return;
        }

        BorrowedBooksManager manager(con.get());
        auto borrowed = manager.getBorrowedBooksByUserID(accountId, showMessage);

        std::time_t now = std::time(nullptr);
        constexpr int OVERDUE_DAYS = 14;

        for (const auto& book : borrowed) {
            if (!book.returned) {
                std::tm borrowTm{};
                std::istringstream iss(book.borrowDate);
                iss >> std::get_time(&borrowTm, "%Y-%m-%d");

                if (!iss.fail()) {
                    std::time_t borrowTime = std::mktime(&borrowTm);
                    double days = std::difftime(now, borrowTime) / (60 * 60 * 24);

                    if (days > OVERDUE_DAYS) {
                        displayOverdueBook(book);
                    }
                }
                else {
                    showMessage("Failed to parse borrow date: " + book.borrowDate);
                }
            }
        }
    }
    catch (const std::exception& e) {
        showMessage(std::string("Exception in getOverdueBooks: ") + e.what());
    }
}

void User::getTotalFine(std::function<void(double)> displayTotalFine, std::function<void(std::string)> showMessage) {
    try {
        auto con = connectToDatabase(showMessage);
        if (!con) {
            showMessage("Database connection failed.");
            return;
        }

        BorrowedBooksManager manager(con.get());
        auto borrowed = manager.getBorrowedBooksByUserID(accountId, showMessage);

        std::time_t now = std::time(nullptr);
        constexpr int ALLOWED_DAYS = 14;
        constexpr double FINE_PER_DAY = 2.0;
        double totalFine = 0.0;

        for (const auto& book : borrowed) {
            if (!book.returned) {
                std::tm borrowTm{};
                std::istringstream iss(book.borrowDate);
                iss >> std::get_time(&borrowTm, "%Y-%m-%d");

                if (!iss.fail()) {
                    std::time_t borrowTime = std::mktime(&borrowTm);
                    double days = std::difftime(now, borrowTime) / (60 * 60 * 24);

                    int overdueDays = static_cast<int>(days) - ALLOWED_DAYS;
                    if (overdueDays > 0) {
                        totalFine += overdueDays * FINE_PER_DAY;
                    }
                }
                else {
                    showMessage("Failed to parse borrow date: " + book.borrowDate);
                }
            }
        }

        displayTotalFine(totalFine);
    }
    catch (const std::exception& e) {
        showMessage(std::string("Exception in getTotalFine: ") + e.what());
    }
}

void User::getAvailableBooks(std::function<void(Book)> displayBook, std::function<void(std::string)> showMessage) {
    try {
        auto con = connectToDatabase(showMessage);
        if (!con) {
            showMessage("Database connection failed.");
            return;
        }

        Book book;
        std::string error;
        auto books = book.getAvailableBooks(con.get(), error);

        if (!error.empty()) {
            showMessage("Error fetching books: " + error);
            return;
        }
        if (books.empty()) {
            showMessage("No books found.");
            return;
        }

        for (const auto& b : books) {
            displayBook(b);
        }
    }
    catch (const std::exception& e) {
        showMessage(std::string("Exception in getAvailableBooks: ") + e.what());
    }
}
