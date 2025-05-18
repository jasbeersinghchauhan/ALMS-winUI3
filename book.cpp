#include "pch.h"
#include "book.h"
#include <iostream>
#include <cppconn/prepared_statement.h>

Book::Book() : year(0), quantity(0) {}
Book::Book(const std::string& title, const std::string& author, const std::string& isbn, int year, int quantity)
    : title(title), author(author), isbn(isbn), year(year), quantity(quantity) {
}
Book Book::fromTitle(const std::string& title) {
    Book b;
    b.title = title;
    return b;
}

Book Book::fromAuthor(const std::string& author) {
    Book b;
    b.author = author;
    return b;
}

Book Book::fromISBN(const std::string& isbn) {
    Book b;
    b.isbn = isbn;
    return b;
}

// --- Setters ---
void Book::setTitle(const std::string& t) { title = t; }
void Book::setAuthor(const std::string& a) { author = a; }
void Book::setISBN(const std::string& i) { isbn = i; }
void Book::setYear(int y) { year = y; }
void Book::setQuantity(int q) { quantity = q; }

// --- Getters ---
std::string Book::getTitle() const { return title; }
std::string Book::getAuthor() const { return author; }
std::string Book::getISBN() const { return isbn; }
int Book::getYear() const { return year; }
int Book::getQuantity() const { return quantity; }

// --- Load from ResultSet ---
void Book::loadFromResultSet(sql::ResultSet* res) {
    if (res) {
        title = res->getString("title");
        author = res->getString("author");
        isbn = res->getString("isbn");
        year = res->getInt("year");
        quantity = res->getInt("quantity");
    }
}

// --- Database Operations ---
std::string Book::insertBook(sql::Connection* con) {
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("INSERT INTO books (isbn, title, author, year, quantity) VALUES (?, ?, ?, ?, ?)"));
        pstmt->setString(1, isbn);
        pstmt->setString(2, title);
        pstmt->setString(3, author);
        pstmt->setInt(4, year);
        pstmt->setInt(5, quantity);
        pstmt->executeUpdate();
        return "Book inserted successfully.";
    }
    catch (sql::SQLException& e) {
        std::string errorMsg = e.what();
        if (errorMsg.find("Duplicate") != std::string::npos) {
            return "Insert failed: Book with this ISBN already exists.";
        }
        return "Insert failed: " + errorMsg;
    }
}



std::string Book::updateBook(sql::Connection* con) {
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("UPDATE books SET title = ?, author = ?, year = ?, quantity = ? WHERE isbn = ?"));
        pstmt->setString(1, title);
        pstmt->setString(2, author);
        pstmt->setInt(3, year);
        pstmt->setInt(4, quantity);
        pstmt->setString(5, isbn);
        int affected = pstmt->executeUpdate();
        if (affected == 0) {
            return "No book found with the given ISBN";
        }
        return "updated book successfully";
    }
    catch (sql::SQLException& e) {
        return "Update failed : " + std::string(e.what());
    }
}

std::string Book::deleteBook(sql::Connection* con) {
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("DELETE FROM books WHERE isbn = ?"));
        pstmt->setString(1, isbn);
        int affected = pstmt->executeUpdate();
        if (affected == 0) {
            return "No book found with the given ISBN.";
        }
        return "Book deleted successfully.";
    }
    catch (sql::SQLException& e) {
        return "Delete failed: " + std::string(e.what());
    }
}


std::vector<Book> Book::searchBooksByISBN(sql::Connection* con) {
    std::vector<Book> results;
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("SELECT * FROM books WHERE isbn = ?"));
        pstmt->setString(1, isbn);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        while (res->next()) {
            Book book;
            book.loadFromResultSet(res.get());
            results.push_back(book);
        }
    }
    catch (const sql::SQLException&) {
        throw;
    }

    return results;
}


std::vector<Book> Book::searchBooksByTitle(sql::Connection* con) {
    std::vector<Book> results;
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("SELECT * FROM books WHERE title = ?"));
        pstmt->setString(1, title);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        while (res->next()) {
            Book book;
            book.loadFromResultSet(res.get());
            results.push_back(book);
        }
    }
    catch (const sql::SQLException&) {
        throw;
    }

    return results;
}


std::vector<Book> Book::searchBooksByAuthor(sql::Connection* con) {
    std::vector<Book> results;
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("SELECT * FROM books WHERE author = ?"));
        pstmt->setString(1, Book::author);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        while (res->next()) {
            Book book;
            book.loadFromResultSet(res.get());
            results.push_back(book);
        }
    }
    catch (const sql::SQLException&) {
        throw;
    }
    return results;
}


std::vector<Book> Book::displayAllBooks(sql::Connection* con, std::string& error) {
    std::vector<Book> books;
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("SELECT * FROM books"));
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        while (res->next()) {
            Book b;
            b.loadFromResultSet(res.get());
            books.push_back(b);
        }
        mergeSort(books);
    }
    catch (sql::SQLException& e) {
        error = "Display failed: " + std::string(e.what());
    }
    return books;
}

std::vector<Book> Book::getPaginatedBooks(sql::Connection* con, int pageNumber, int pageSize) {
    std::vector<Book> books;

    int offset = (pageNumber - 1) * pageSize;

    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("SELECT * FROM books LIMIT ? OFFSET ?"));
        pstmt->setInt(1, pageSize);
        pstmt->setInt(2, offset);

        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        while (res->next()) {
            Book book;
            book.loadFromResultSet(res.get());
            books.push_back(book);
        }

        mergeSort(books);
    }
    catch (sql::SQLException& e) {
        std::cerr << "Pagination query failed: " << e.what() << std::endl;
    }

    return books;
}


std::vector<Book> Book::getAvailableBooks(sql::Connection* con, std::string& error) {
    std::vector<Book> books;
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(
            "SELECT title, author, isbn, year, quantity FROM books WHERE quantity > 0"
        ));
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        while (res->next()) {
            Book book;
            book.setTitle(res->getString("title"));
            book.setAuthor(res->getString("author"));
            book.setISBN(res->getString("isbn"));
            book.setYear(res->getInt("year"));
            book.setQuantity(res->getInt("quantity"));
            books.push_back(book);
        }
    }
    catch (sql::SQLException& e) {
        error = e.what();
    }
    return books;
}


// --- Sorting Algorithms ---
void Book::mergeSort(std::vector<Book>& books) {
    mergeSortHelper(books, 0, books.size() - 1);
}

void Book::mergeSortHelper(std::vector<Book>& books, size_t left, size_t right) {
    if (left < right) {
        size_t mid = left + (right - left) / 2;
        mergeSortHelper(books, left, mid);
        mergeSortHelper(books, mid + 1, right);
        mergeHelper(books, left, mid, right);
    }
}


void Book::mergeHelper(std::vector<Book>& books, size_t left, size_t mid, size_t right) {
    size_t n1 = mid - left + 1;
    size_t n2 = right - mid;

    std::vector<Book> leftBooks(n1);
    std::vector<Book> rightBooks(n2);

    for (size_t i = 0; i < n1; ++i) {
        leftBooks[i] = books[left + i];
    }
    for (size_t i = 0; i < n2; ++i) {
        rightBooks[i] = books[mid + 1 + i];
    }

    size_t i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        std::string leftTitle = leftBooks[i].getTitle();
        std::string rightTitle = rightBooks[j].getTitle();
        std::transform(leftTitle.begin(), leftTitle.end(), leftTitle.begin(),
            [](char c) -> char { return static_cast<char>(std::tolower(static_cast<unsigned char>(c))); });

        std::transform(rightTitle.begin(), rightTitle.end(), rightTitle.begin(),
            [](char c) -> char { return static_cast<char>(std::tolower(static_cast<unsigned char>(c))); });


        if (leftTitle < rightTitle)
        {
            books[k++] = leftBooks[i++];
        }
        else {
            books[k++] = rightBooks[j++];
        }
    }

    while (i < n1) {
        books[k++] = leftBooks[i++];
    }

    while (j < n2) {
        books[k++] = rightBooks[j++];
    }
}
