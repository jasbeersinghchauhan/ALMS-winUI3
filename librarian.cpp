#include "pch.h"
#include "librarian.h"
#include "book.h"
#include "account.h"
#include "borrowedBooks.h"
#include "DatabaseService.h"

void librarian::viewAccountById(int accountId, std::function<void(std::string)> showMessage,
    std::function<void(Account)> displayAccount) {
    try {
        DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
            Account account(accountId);
            std::string resultMessage = account.searchAccountByID(con);
            showMessage(resultMessage);
            if (account.getAccountId() != 0) {
                displayAccount(account);
            }
            });
    }
    catch (const std::exception& e) {
        showMessage("Error: " + std::string(e.what()));
    }
}

void librarian::viewAccountByEmail(const std::string& email, std::function<void(std::string)> showMessage,
    std::function<void(Account)> displayAccount) {
    try {
        DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
            Account account(email);
            std::string resultMessage = account.searchAccountByEmail(con);
            showMessage(resultMessage);
            if (!account.getEmail().empty()) {
                displayAccount(account);
            }
        });
    }
    catch (const std::exception& e) {
        showMessage("Error: " + std::string(e.what()));
    }
}

void librarian::addBook(const std::string& title, const std::string& author, const std::string& isbn, int year, int quantity,
    std::function<void(std::string)> showMessage) {
    try {
        DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
            Book book(title, author, isbn, year, quantity);
            std::string resultMessage = book.insertBook(con);
            showMessage(resultMessage);
            });
    }
    catch (const std::exception& e) {
        showMessage("Error: " + std::string(e.what()));
    }
}

void librarian::updateBook(const std::string& isbn, const std::string& title, const std::string& author,
    int year, int quantity, std::function<void(std::string)> showMessage) {
    try {
        DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
            Book book = Book::fromISBN(isbn);
            std::vector<Book> foundBooks = book.searchBooksByISBN(con);

            if (foundBooks.empty()) {
                showMessage("Book not found with ISBN: " + isbn);
                return;
            }

            Book& bookToUpdate = foundBooks[0];
            if (!title.empty()) bookToUpdate.setTitle(title);
            if (!author.empty()) bookToUpdate.setAuthor(author);
            if (year > 0) bookToUpdate.setYear(year);
            if (quantity >= 0) bookToUpdate.setQuantity(quantity);

            std::string updateResult = bookToUpdate.updateBook(con);
            showMessage(updateResult);
        });
    }
    catch (const std::exception& e) {
        showMessage("Error: " + std::string(e.what()));
    }
}

void librarian::deleteBook(const std::string& isbn, std::function<void(std::string)> showMessage) {
    try {
        DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
            Book book = Book::fromISBN(isbn);
            std::vector<Book> foundBooks = book.searchBooksByISBN(con);

            if (!foundBooks.empty()) {
                Book& bookToDelete = foundBooks[0];
                std::string resultMessage = bookToDelete.deleteBook(con);
                showMessage(resultMessage);
            }
            else
                showMessage("Book not found with ISBN: " + isbn);
            });
    }
    catch (const std::exception& e) {
        showMessage("Error: " + std::string(e.what()));
    }
}

void librarian::viewAvailableBooks(std::function<void(Book)> displayBook, std::function<void(std::string)> showMessage) {
    try {
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
    catch (const std::exception& e) {
        showMessage("Error: " + std::string(e.what()));
    }
}

void librarian::viewAllBorrowedBooks(std::function<void(BorrowedBook)> displayBorrowedBook,
    std::function<void(std::string)> showMessage) {
    try {
        DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
            BorrowedBooksManager manager(con);
            auto borrowed = manager.getAllBorrowedBooks(showMessage);
            for (const auto& book : borrowed) {
                displayBorrowedBook(book);
            }
		});
    }
    catch (const std::exception& e) {
        showMessage("Error: " + std::string(e.what()));
    }
}

void librarian::viewBorrowedBooksByAccountId(int accountId, std::function<void(BorrowedBook)> displayBorrowedBook,
    std::function<void(std::string)> showMessage) {
    try {
        DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
            BorrowedBooksManager manager(con);
            auto borrowed = manager.getBorrowedBooksByUserID(accountId, showMessage);
            for (const auto& book : borrowed) {
                displayBorrowedBook(book);
            }
        });
    }
    catch (const std::exception& e) {
        showMessage("Error: " + std::string(e.what()));
    }
}

void librarian::allocateBook(int accountId, int bookId, std::function<void(std::string)> showMessage, std::function<void(bool)> onResult) {
    try {
        DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
            BorrowedBooksManager borrowedBooksManager(con);
            if (borrowedBooksManager.isBookAvailable(bookId)) {
                std::string result = borrowedBooksManager.addBorrowedBook(accountId, bookId);
                bool success = (result == "success");
                onResult(success);
            }
            else {
                showMessage("Book is not available.");
                onResult(false);
            }
		});
    }
    catch (const std::exception& e) {
        showMessage("Error: " + std::string(e.what()));
        onResult(false);
    }
}

void librarian::revertBookAllocation(int accountId, int bookId, std::function<void(std::string)> showMessage, std::function<void(bool)> onResult) {
    try {
        DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
            BorrowedBooksManager borrowedBooksManager(con);
            borrowedBooksManager.revertBorrowedBook(accountId, bookId);

            std::unique_ptr<sql::PreparedStatement> pstmt(
                con->prepareStatement("UPDATE books SET available = TRUE WHERE book_id = ?")
            );
            pstmt->setInt(1, bookId);
            int updated = pstmt->executeUpdate();
            onResult(updated > 0);
        });
    }
    catch (const std::exception& e) {
        showMessage("Error reverting book allocation: " + std::string(e.what()));
        onResult(false);
    }
}
