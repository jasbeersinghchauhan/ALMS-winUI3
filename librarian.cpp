#include "pch.h"
#include "librarian.h"
#include "dataBaseService.h"

void librarian::viewAccountById(int accountId, std::function<void(std::string)> showMessage,
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

void librarian::viewAccountByEmail(const std::string& email, std::function<void(std::string)> showMessage,
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

void librarian::viewAllAccounts(std::function<void(Account)> displayAccount, std::function<void(std::string)> showMessage) {
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

void librarian::addBook(const std::string& title, const std::string& author,
    const std::string& isbn, int year, int quantity,
    std::function<void(std::string)> showMessage) {
    DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
        Book book(title, author, isbn, year, quantity);
        std::string resultMessage = book.insertBook(con);
        showMessage(resultMessage);
        });
}

void librarian::updateBook(const std::string& isbn, const std::string& title, const std::string& author,
    int year, int quantity, std::function<void(std::string)> showMessage) {

    DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
        Book book(isbn);
        std::string searchResult = book.searchBookByISBN(con);

        if (!searchResult.empty()) {
            showMessage(searchResult);
            return;
        }

        if (title.empty() && author.empty() && year <= 0 && quantity < 0) {
            showMessage("No valid fields provided for update.");
            return;
        }

        if (!title.empty()) book.setTitle(title);
        if (!author.empty()) book.setAuthor(author);
        if (year > 0) book.setYear(year);
        if (quantity >= 0) book.setQuantity(quantity);

        std::string updateResult = book.updateBook(con);
        showMessage(updateResult);
        });
}


void librarian::deleteBook(const std::string& isbn, std::function<void(std::string)> showMessage) {
    DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
        Book book(isbn);
        std::string searchResult = book.searchBookByISBN(con);

        if (!searchResult.empty()) {
            showMessage(searchResult);
            return;
        }

        std::string resultMessage = book.deleteBook(con);
        showMessage(resultMessage);
        });
}

void librarian::viewAllBooks(std::function<void(Book)> displayBook, std::function<void(std::string)> showMessage) {
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

void librarian::viewAvailableBooks(std::function<void(Book)> displayBook, std::function<void(std::string)> showMessage) {
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


void librarian::viewAllBorrowedBooks(std::function<void(BorrowedBook)> displayBorrowedBook,
    std::function<void(std::string)> showMessage) {
    DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
        BorrowedBooksManager manager(con);
        auto borrowed = manager.getAllBorrowedBooks(showMessage);
        for (const auto& book : borrowed) {
            displayBorrowedBook(book);
        }
        });
}

void librarian::viewBorrowedBooksByAccountId(int accountId, std::function<void(BorrowedBook)> displayBorrowedBook,
    std::function<void(std::string)> showMessage) {

    DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
        BorrowedBooksManager manager(con);
        auto borrowed = manager.getBorrowedBooksByUserID(accountId, showMessage);
        for (const auto& book : borrowed) {
            displayBorrowedBook(book);
        }
        });
}

void librarian::allocateBook(int accountId, int bookId, std::function<void(std::string)> showMessage, std::function<void(bool)> onResult) {

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

void librarian::revertBookAllocation(int accountId, int bookId, std::function<void(std::string)> showMessage, std::function<void(bool)> onResult) {

    DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
        BorrowedBooksManager borrowedBooksManager(con);
        try {
            borrowedBooksManager.revertBorrowedBook(accountId, bookId);
            std::unique_ptr<sql::PreparedStatement> pstmt(
                con->prepareStatement("UPDATE books SET available = TRUE WHERE book_id = ?")
            );
            pstmt->setInt(1, bookId);
            int updated = pstmt->executeUpdate();
            onResult(updated > 0);
        }
        catch (const std::exception& e) {
            showMessage("Error reverting book allocation: " + std::string(e.what()));
            onResult(false);
        }
        });
}
