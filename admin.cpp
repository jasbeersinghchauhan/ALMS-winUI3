#include "pch.h"
#include "admin.h"
#include "account.h"
#include "book.h"
#include "borrowedBooks.h"
#include "dbConnection.h"

void Admin::addAccount(const std::string& name, const std::string& email, const std::string& password,
    const std::string& phone, const std::string& role,
    std::function<void(std::string)> showMessage) {

    if (role != "admin" && role != "user" && role != "librarian") {
        showMessage("Invalid role. Please enter 'admin', 'user', or 'librarian'.");
        return;
    }

    try {
        auto con = connectToDatabase(showMessage);
        Account newAccount(name, email, password, phone, role);
        std::string resultMessage = newAccount.insertAccount(con.get());
        showMessage(resultMessage);
    }
    catch (const std::exception& e) {
        showMessage("Error: " + std::string(e.what()));
    }
}


void Admin::viewAccountById(int accountId, std::function<void(std::string)> showMessage,
    std::function<void(Account)> displayAccount) {
    try {
        auto con = connectToDatabase(showMessage);
        Account account(accountId);
        std::string resultMessage = account.searchAccountByID(con.get());
        showMessage(resultMessage);
        if (account.getAccountId() != 0) {
            displayAccount(account);
        }
    }
    catch (const std::exception& e) {
        showMessage("Error: " + std::string(e.what()));
    }
}

void Admin::viewAccountByEmail(const std::string& email, std::function<void(std::string)> showMessage,
    std::function<void(Account)> displayAccount) {
    try {
        auto con = connectToDatabase(showMessage);
        Account account(email);
        std::string resultMessage = account.searchAccountByEmail(con.get());
        showMessage(resultMessage);
        if (!account.getEmail().empty()) {
            displayAccount(account);
        }
    }
    catch (const std::exception& e) {
        showMessage("Error: " + std::string(e.what()));
    }
}


void Admin::updateAccount(int accountId, const std::string& name, const std::string& email,
    const std::string& phone, const std::string& role,
    std::function<void(std::string)> showMessage) {

    try {
        auto con = connectToDatabase(showMessage);
        Account acc(accountId);
        acc.searchAccountByID(con.get());

        if (acc.getAccountId() != 0) {
            if (!role.empty() && role != "admin" && role != "user" && role != "librarian") {
                showMessage("Invalid role. Please enter 'admin', 'user', or 'librarian'.");
                return;
            }

            if (!name.empty()) acc.setName(name);
            if (!email.empty()) acc.setEmail(email);
            if (!phone.empty()) acc.setPhone(phone);
            if (!role.empty()) acc.setRole(role);

            std::string resultMessage = acc.updateAccount(con.get());
            showMessage(resultMessage);
        }
        else {
            showMessage("Account not found with ID: " + std::to_string(accountId));
        }
    }
    catch (const std::exception& e) {
        showMessage("Error: " + std::string(e.what()));
    }
}

void Admin::updateRole(int currentAdminId, int targetAccountId, const std::string& newRole,
    std::function<void(std::string)> showMessage) {
    try {
        auto con = connectToDatabase(showMessage);
        Account current(currentAdminId);
        current.searchAccountByID(con.get());
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
        std::string resultMessage = target.updateRole(con.get());
        showMessage(resultMessage);
    }
    catch (const std::exception& e) {
        showMessage("Error: " + std::string(e.what()));
    }
}

void Admin::deleteAccount(int accountId, std::function<void(std::string)> showMessage) {
    try {
        auto con = connectToDatabase(showMessage);
        Account acc(accountId);
        acc.searchAccountByID(con.get());

        if (acc.getAccountId() != 0) {
            std::string resultMessage = acc.deleteAccount(con.get());
            showMessage(resultMessage);
        }
        else {
            showMessage("Account not found with ID: " + std::to_string(accountId));
        }
    }
    catch (const std::exception& e) {
        showMessage("Error: " + std::string(e.what()));
    }
}

void Admin::viewAllAccounts(std::function<void(Account)> displayAccount, std::function<void(std::string)> showMessage) {
    try {
        auto con = connectToDatabase(showMessage);
        Account acc;
        std::string error;
        auto accounts = acc.displayAllAccounts(con.get(), error);
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
    }
    catch (const std::exception& e) {
        showMessage("Error: " + std::string(e.what()));
    }
}


void Admin::addBook(const std::string& title, const std::string& author, const std::string& isbn, int year, int quantity,
    std::function<void(std::string)> showMessage) {
    try {
        auto con = connectToDatabase(showMessage);
        Book book(title, author, isbn, year, quantity);
        std::string resultMessage = book.insertBook(con.get());
        showMessage(resultMessage);
    }
    catch (const std::exception& e) {
        showMessage("Error: " + std::string(e.what()));
    }
}


void Admin::updateBook(const std::string& isbn, const std::string& title, const std::string& author,
    int year, int quantity, std::function<void(std::string)> showMessage) {
    try {
        auto con = connectToDatabase(showMessage);
        Book book = Book::fromISBN(isbn);
        std::vector<Book> foundBooks = book.searchBooksByISBN(con.get());

        if (foundBooks.empty()) {
            showMessage("Book not found with ISBN: " + isbn);
            return;
        }

        Book& bookToUpdate = foundBooks[0];
        if (!title.empty()) bookToUpdate.setTitle(title);
        if (!author.empty()) bookToUpdate.setAuthor(author);
        if (year > 0) bookToUpdate.setYear(year);
        if (quantity >= 0) bookToUpdate.setQuantity(quantity);

        std::string updateResult = bookToUpdate.updateBook(con.get());
        showMessage(updateResult);
    }
    catch (const std::exception& e) {
        showMessage("Error: " + std::string(e.what()));
    }
}



void Admin::deleteBook(const std::string& isbn, std::function<void(std::string)> showMessage) {
    try {
        auto con = connectToDatabase(showMessage);
        Book book = Book::fromISBN(isbn);
        std::vector<Book> foundBooks = book.searchBooksByISBN(con.get());

        if (!foundBooks.empty()) {
            Book& bookToDelete = foundBooks[0];
            std::string resultMessage = bookToDelete.deleteBook(con.get());
            showMessage(resultMessage);
        }
        else {
            showMessage("Book not found with ISBN: " + isbn);
        }
    }
    catch (const std::exception& e) {
        showMessage("Error: " + std::string(e.what()));
    }
}


void Admin::viewAllBooks(std::function<void(Book)> displayBook, std::function<void(std::string)> showMessage) {
    try {
        auto con = connectToDatabase(showMessage);
        Book book;
        std::string error;
        auto books = book.displayAllBooks(con.get(), error);

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
    }
    catch (const std::exception& e) {
        showMessage("Error: " + std::string(e.what()));
    }
}

std::vector<Book> Admin::searchBookByTitle(const std::string& title, std::function<void(std::string)> showMessage) {
    std::vector<Book> foundBooks;
    try {
        auto con = connectToDatabase(showMessage);
        Book book = Book::fromTitle(title);
        foundBooks = book.searchBooksByTitle(con.get());

        if (foundBooks.empty()) {
            showMessage("No books found with that title.");
        }
        else {
            showMessage(std::to_string(foundBooks.size()) + " book(s) found.");
        }
    }
    catch (const sql::SQLException& e) {
        showMessage("Search failed: " + std::string(e.what()));
    }
    catch (const std::exception& e) {
        showMessage("Error: " + std::string(e.what()));
    }
    return foundBooks;
}

std::vector<Book> Admin::searchBookByISBN(const std::string& isbn, std::function<void(std::string)> showMessage) {
    std::vector<Book> foundBooks;
    try {
        auto con = connectToDatabase(showMessage);
        Book book = Book::fromISBN(isbn);
        foundBooks = book.searchBooksByISBN(con.get());

        if (foundBooks.empty()) {
            showMessage("No books found with that ISBN.");
        }
        else {
            showMessage(std::to_string(foundBooks.size()) + " book(s) found.");
        }
    }
    catch (const sql::SQLException& e) {
        showMessage("Search failed: " + std::string(e.what()));
    }
    catch (const std::exception& e) {
        showMessage("Error: " + std::string(e.what()));
    }
    return foundBooks;
}

std::vector<Book> Admin::searchBookByAuthor(const std::string& author, std::function<void(std::string)> showMessage) {
    std::vector<Book> foundBooks;
    try {
        auto con = connectToDatabase(showMessage);
        Book book = Book::fromAuthor(author);
        foundBooks = book.searchBooksByAuthor(con.get());

        if (foundBooks.empty()) {
            showMessage("No books found with that author.");
        }
        else {
            showMessage(std::to_string(foundBooks.size()) + " book(s) found.");
        }
    }
    catch (const sql::SQLException& e) {
        showMessage("Search failed: " + std::string(e.what()));
    }
    catch (const std::exception& e) {
        showMessage("Error: " + std::string(e.what()));
    }
    return foundBooks;
}



void Admin::viewAllBorrowedBooks(std::function<void(BorrowedBook)> displayBorrowedBook,
    std::function<void(std::string)> showMessage) {
    try {
        auto con = connectToDatabase(showMessage);
        BorrowedBooksManager manager(con.get());
        auto borrowed = manager.getAllBorrowedBooks(showMessage);
        for (const auto& book : borrowed) {
            displayBorrowedBook(book);
        }
    }
    catch (const std::exception& e) {
        showMessage("Error: " + std::string(e.what()));
    }
}

void Admin::viewBorrowedBooksByAccountId(int accountId, std::function<void(BorrowedBook)> displayBorrowedBook,
    std::function<void(std::string)> showMessage) {
    try {
        auto con = connectToDatabase(showMessage);
        BorrowedBooksManager manager(con.get());
        auto borrowed = manager.getBorrowedBooksByUserID(accountId, showMessage);
        for (const auto& book : borrowed) {
            displayBorrowedBook(book);
        }
    }
    catch (const std::exception& e) {
        showMessage("Error: " + std::string(e.what()));
    }
}
