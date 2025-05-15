#include "pch.h"
#include "user.h"
#include "dataBaseService.h"

#include <sstream>
#include <ctime>
#include <cstdio>
#include <iomanip>

User::User(int id) : accountId(id) {}

void User::getUserDetails(std::function<void(std::string)> showMessage, std::function<void(Account)> displayAccount) {
    DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
        Account account(accountId);
        std::string resultMessage = account.searchAccountByID(con);
        showMessage(resultMessage);
        if (account.getAccountId() != 0) {
            displayAccount(account);
        }
    });
}

void User::getBorrowedBooks(std::function<void(BorrowedBook)> displayBorrowedBook, std::function<void(std::string)> showMessage) {
    DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
        BorrowedBooksManager manager(con);
        auto borrowed = manager.getBorrowedBooksByUserID(accountId, showMessage);
        for (const auto& book : borrowed) {
            displayBorrowedBook(book);
        }
        });
}

void User::getOverdueBooks(std::function<void(BorrowedBook)> displayOverdueBook, std::function<void(std::string)> showMessage) {
    DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
        BorrowedBooksManager manager(con);
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
                    if (showMessage) {
                        showMessage("Failed to parse borrow date: " + book.borrowDate);
                    }
                }
            }
        }
        });
}


void User::getTotalFine(std::function<void(double)> displayTotalFine, std::function<void(std::string)> showMessage) {
    DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
        BorrowedBooksManager manager(con);
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
                    if (showMessage) {
                        showMessage("Failed to parse borrow date: " + book.borrowDate);
                    }
                }
            }
        }

        displayTotalFine(totalFine);
        });
}


void User::getAvailableBooks(std::function<void(Book)> displayBook, std::function<void(std::string)> showMessage) {
    DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
        Book book;
        std::string error;
        auto books = book.getAvailableBooks(con, error);

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
