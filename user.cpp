#include "user.h"
#include <sstream>
#include <ctime>
#include <cstdio>

User::User(int id) : accountId(id) {}

std::string User::getUserDetails(sql::Connection* con) {
    Account account(accountId);
    account.searchAccountByID(con);

    std::ostringstream details;
    details << "Account ID: " << account.getAccountId() << "\n";
    details << "Name: " << account.getName() << "\n";
    details << "Email: " << account.getEmail() << "\n";
    details << "Phone: " << account.getPhone() << "\n";
    return details.str();
}

std::vector<BorrowedBook> User::getBorrowedBooks(sql::Connection* con) {
    BorrowedBooksManager manager(con);
    return manager.getBorrowedBooksByUserID(accountId);
}

std::vector<BorrowedBook> User::getOverdueBooks(sql::Connection* con) {
    BorrowedBooksManager manager(con);
    std::vector<BorrowedBook> borrowed = manager.getBorrowedBooksByUserID(accountId);
    std::vector<BorrowedBook> overdue;

    std::time_t now = std::time(nullptr);

    for (const auto& book : borrowed) {
        if (!book.returned) {
            std::tm borrowTm = {};
            int y, m, d;
            if (std::sscanf(book.borrowDate.c_str(), "%d-%d-%d", &y, &m, &d) == 3) {
                borrowTm.tm_year = y - 1900;
                borrowTm.tm_mon = m - 1;
                borrowTm.tm_mday = d;

                std::time_t borrowTime = std::mktime(&borrowTm);
                double days = std::difftime(now, borrowTime) / (60 * 60 * 24);

                if (days > 14) {
                    overdue.push_back(book);
                }
            }
        }
    }

    return overdue;
}

double User::getTotalFine(sql::Connection* con) {
    BorrowedBooksManager manager(con);
    std::vector<BorrowedBook> borrowed = manager.getBorrowedBooksByUserID(accountId);
    double totalFine = 0.0;
    std::time_t now = std::time(nullptr);

    for (const auto& book : borrowed) {
        if (!book.returned) {
            std::tm borrowTm = {};
            int y, m, d;
            if (std::sscanf(book.borrowDate.c_str(), "%d-%d-%d", &y, &m, &d) == 3) {
                borrowTm.tm_year = y - 1900;
                borrowTm.tm_mon = m - 1;
                borrowTm.tm_mday = d;

                std::time_t borrowTime = std::mktime(&borrowTm);
                double days = std::difftime(now, borrowTime) / (60 * 60 * 24);

                int overdueDays = static_cast<int>(days) - 14;
                if (overdueDays > 0) {
                    totalFine += overdueDays * 2.0;
                }
            }
        }
    }

    return totalFine;
}

std::vector<Book> User::getAvailableBooks(sql::Connection* con) {
    Book book;
    return book.getAvailableBooks(con);
}