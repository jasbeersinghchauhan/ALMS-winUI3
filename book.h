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
    Book(std::string isbn);
    Book(std::string title, std::string author, std::string isbn, int year, int quantity);

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
    std::string searchBookByISBN(sql::Connection* con);
    std::string searchBookByTitle(sql::Connection* con);
    std::string searchBookByAuthor(sql::Connection* con);
    std::vector<Book> displayAllBooks(sql::Connection* con);
    std::vector<Book> getAvailableBooks(sql::Connection* con);

    // Pagination
    std::vector<Book> getPaginatedBooks(sql::Connection* con, int pageNumber, int pageSize);

    // Sorting
    static void mergeSort(std::vector<Book>& books);
    static void mergeSortHelper(std::vector<Book>& books, int left, int right);
    static void mergeHelper(std::vector<Book>& books, int left, int mid, int right);

    // Custom search algorithms
    static int linearSearchByISBN(const std::vector<Book>& books, const std::string& isbn);
    static int linearSearchByTitle(const std::vector<Book>& books, const std::string& title);
    static int linearSearchByAuthor(const std::vector<Book>& books, const std::string& author);
};

#endif