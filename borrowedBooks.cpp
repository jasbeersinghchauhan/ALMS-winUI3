#include "borrowedBooks.h"
#include <stdexcept>
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>

// Constructor
BorrowedBooksManager::BorrowedBooksManager(sql::Connection* connection)
    : conn(connection) {
}

// Check if the book is available for borrowing
bool BorrowedBooksManager::isBookAvailable(int bookId) {
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            conn->prepareStatement("SELECT COUNT(*) FROM books WHERE book_id = ? AND available = TRUE")
        );
        pstmt->setInt(1, bookId);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        return res->next() && res->getInt(1) > 0;
    }
    catch (...) {
        return false;
    }
}

// Add a borrowed book record (user borrows a book)
std::string BorrowedBooksManager::addBorrowedBook(int accountId, int bookId) {
    if (!isBookAvailable(bookId)) {
        return "Book is not available for borrowing.";
    }

    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            conn->prepareStatement("INSERT INTO borrowed_books (account_id, book_id) VALUES (?, ?)")
        );
        pstmt->setInt(1, accountId);
        pstmt->setInt(2, bookId);
        pstmt->executeUpdate();
        return "Book borrowed successfully.";
    }
    catch (sql::SQLException& e) {
        revertBorrowedBook(accountId, bookId);
        return "Borrowing failed: " + std::string(e.what());
    }
}

// Revert borrowing process
void BorrowedBooksManager::revertBorrowedBook(int accountId, int bookId) {
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement("DELETE FROM borrowed_books WHERE account_id = ? AND book_id = ?"));
        pstmt->setInt(1, accountId);
        pstmt->setInt(2, bookId);
        pstmt->executeUpdate();
    }
    catch (sql::SQLException& e) {
        throw std::runtime_error("Error reverting borrowed book: " + std::string(e.what()));
    }
}

// Get all borrowed books (Admin view)
std::vector<BorrowedBook> BorrowedBooksManager::getAllBorrowedBooks() {
    std::vector<BorrowedBook> books;
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            conn->prepareStatement("SELECT * FROM borrowed_books")
        );
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        while (res->next()) {
            BorrowedBook b;
            b.borrowId = res->getInt("borrow_id");
            b.accountId = res->getInt("account_id");
            b.bookId = res->getInt("book_id");
            b.borrowDate = res->getString("borrow_date");
            b.returnDate = res->isNull("return_date") ? "" : res->getString("return_date");
            b.returned = res->getBoolean("returned");
            books.push_back(b);
        }
    }
    catch (...) {
        // Optionally handle/log
    }
    return books;
}

// Get borrowed books for a specific user
std::vector<BorrowedBook> BorrowedBooksManager::getBorrowedBooksByUserID(int userId) {
    std::vector<BorrowedBook> books;
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            conn->prepareStatement("SELECT * FROM borrowed_books WHERE account_id = ?")
        );
        pstmt->setInt(1, userId);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        while (res->next()) {
            BorrowedBook b;
            b.borrowId = res->getInt("borrow_id");
            b.accountId = res->getInt("account_id");
            b.bookId = res->getInt("book_id");
            b.borrowDate = res->getString("borrow_date");
            b.returnDate = res->isNull("return_date") ? "" : res->getString("return_date");
            b.returned = res->getBoolean("returned");
            books.push_back(b);
        }
    }
    catch (...) {
        // Optionally handle/log
    }

    return books;
}

// Mark a book as returned
std::string BorrowedBooksManager::markAsReturned(int borrowId) {
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            conn->prepareStatement(
                "UPDATE borrowed_books SET returned = TRUE, return_date = CURRENT_DATE WHERE borrow_id = ?"
            )
        );
        pstmt->setInt(1, borrowId);
        int affected = pstmt->executeUpdate();
        return affected == 0 ? "No borrow record found." : "Book marked as returned.";
    }
    catch (sql::SQLException& e) {
        return std::string("Failed to return book: ") + std::string(e.what());
    }
}

// Delete a borrow record
std::string BorrowedBooksManager::deleteBorrowRecord(int borrowId) {
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            conn->prepareStatement("DELETE FROM borrowed_books WHERE borrow_id = ?")
        );
        pstmt->setInt(1, borrowId);
        int affected = pstmt->executeUpdate();
        return affected == 0 ? "No record found to delete." : "Borrow record deleted.";
    }
    catch (sql::SQLException& e) {
        return std::string("Failed to delete record: ") + std::string(e.what());
    }
}