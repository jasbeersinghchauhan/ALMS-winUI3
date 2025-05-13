#include "librarian.h"

void librarian::viewAccountById(sql::Connection* con, int accountId, std::function<void(std::string)> showMessage, std::function<void(Account)> displayAccount) {
    Account viewAccount(accountId);
    viewAccount.searchAccountByID(con);
    if (viewAccount.getAccountId() != 0) {
        displayAccount(viewAccount);
    }
    else {
        showMessage("No account found with that ID.");
    }
}

void librarian::viewAccountByEmail(sql::Connection* con, const std::string& email, std::function<void(std::string)> showMessage, std::function<void(Account)> displayAccount) {
    Account viewAccount(email);
    viewAccount.searchAccountByEmail(con);

    if (!viewAccount.getEmail().empty()) {
        displayAccount(viewAccount);
    }
    else {
        showMessage("No account found with that email.");
    }
}

void librarian::viewAllAccounts(sql::Connection* con, std::function<void(Account)> displayAccount) {
    Account viewAllAccounts;
    std::string outError; // Add a string variable to capture the error message  
    std::vector<Account> accounts = viewAllAccounts.displayAllAccounts(con, outError);

    if (!outError.empty()) {
        // Handle the error if any  
        throw std::runtime_error(outError);
    }

    for (const auto& account : accounts) {
        displayAccount(account); // Use the displayAccount function for each account  
    }
}

void librarian::addBook(sql::Connection* con, const Book& newBook) {
    Book book = newBook;
    book.insertBook(con);
}

void librarian::updateBook(sql::Connection* con, const Book& updatedBook) {
    Book book = updatedBook;
    book.updateBook(con);
}

void librarian::deleteBook(sql::Connection* con, const std::string& isbn) {
    Book book(isbn);
    book.deleteBook(con);
}

void librarian::viewAllBooks(sql::Connection* con, std::function<void(Book)> displayBook) {
    Book viewAllBooks;
    std::vector<Book> books = viewAllBooks.displayAllBooks(con);
    for (const auto& book : books) {
        displayBook(book);
    }
}

void librarian::viewAvailableBooks(sql::Connection* con, std::vector<Book>& outBooks) {
    Book book;
    outBooks = book.getAvailableBooks(con);
}

void librarian::viewAllBorrowedBooks(sql::Connection* con, std::vector<BorrowedBook>& outBorrowedBooks) {
    BorrowedBooksManager borrowedBook(con);
    outBorrowedBooks = borrowedBook.getAllBorrowedBooks();
}

void librarian::viewBorrowedBooksByAccountId(sql::Connection* con, int accountId, std::vector<BorrowedBook>& outBorrowedBooks) {
    BorrowedBooksManager borrowedBook(con);
    outBorrowedBooks = borrowedBook.getBorrowedBooksByUserID(accountId);
}

bool librarian::allocateBook(sql::Connection* con, int accountId, int bookId) {
    BorrowedBooksManager borrowedBooksManager(con);

    if (borrowedBooksManager.isBookAvailable(bookId)) {
        // If available, add it as a borrowed book
        std::string result = borrowedBooksManager.addBorrowedBook(accountId, bookId);
        if (result == "success") {
            return true;
        }
        else {
            // Handle the error if needed (e.g., book allocation failed)
            return false;
        }
    }
    return false;
}

bool librarian::revertBookAllocation(sql::Connection* con, int accountId, int bookId) {
    BorrowedBooksManager borrowedBooksManager(con);

    try {
        borrowedBooksManager.revertBorrowedBook(accountId, bookId);
        return true;
    }
    catch (const std::exception& e) {
        return false;
    }
}

