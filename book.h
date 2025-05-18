#ifndef BOOK_H  
#define BOOK_H

#include <pch.h>
#include <string>
#include <vector>
#include <memory>
#include <cppconn/connection.h>
#include <cppconn/resultset.h>

class Book {
private:
    std::string title;
    std::string author;
    std::string isbn;
    int year;
    int quantity;

public:
    // Constructors
    Book();
    Book(const std::string& title, const std::string& author, const std::string& isbn, int year, int quantity);
    static Book fromTitle(const std::string& title);
    static Book fromAuthor(const std::string& author);
    static Book fromISBN(const std::string& isbn);

    // Setters
    void setTitle(const std::string& t);
    void setAuthor(const std::string& a);
    void setISBN(const std::string& i);
    void setYear(int y);
    void setQuantity(int q);

    // Getters
    std::string getTitle() const;
    std::string getAuthor() const;
    std::string getISBN() const;
    int getYear() const;
    int getQuantity() const;

    // Load book from ResultSet
    void loadFromResultSet(sql::ResultSet* res);

    // Database operations
    std::string insertBook(sql::Connection* con);
    std::string updateBook(sql::Connection* con);
    std::string deleteBook(sql::Connection* con);
    std::vector<Book> searchBooksByTitle(sql::Connection* con);
    std::vector<Book> searchBooksByISBN(sql::Connection* con);
    std::vector<Book> searchBooksByAuthor(sql::Connection* con);
    std::vector<Book> displayAllBooks(sql::Connection* con, std::string& error);
    std::vector<Book> getAvailableBooks(sql::Connection* con, std::string& error);

    // Pagination
    std::vector<Book> getPaginatedBooks(sql::Connection* con, int pageNumber, int pageSize);

    // Sorting
    static void mergeSort(std::vector<Book>& books);
    static void mergeHelper(std::vector<Book>& books, size_t left, size_t mid, size_t right);
    static void mergeSortHelper(std::vector<Book>& books, size_t left,  size_t right);

};

#endif
