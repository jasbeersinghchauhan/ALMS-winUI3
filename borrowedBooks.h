#ifndef BORROWEDBOOKS_H
#define BORROWEDBOOKS_H

#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

struct BorrowedBook {  
   int borrowId = 0;  
   int accountId = 0;  
   int bookId = 0;  
   std::string borrowDate;  
   std::string returnDate;  
   bool returned = false;  
};

class BorrowedBooksManager {
private:
    sql::Connection* conn;

public:
    BorrowedBooksManager(sql::Connection* connection);

    bool isBookAvailable(int bookId);

    std::string addBorrowedBook(int accountId, int bookId);

    void revertBorrowedBook(int accountId, int bookId);

    std::vector<BorrowedBook> getAllBorrowedBooks(std::function<void(std::string)> showMessage);

    std::vector<BorrowedBook> getBorrowedBooksByUserID(int userId, std::function<void(std::string)> showMessage);

    std::string markAsReturned(int borrowId);

    std::string deleteBorrowRecord(int borrowId);
};

#endif
