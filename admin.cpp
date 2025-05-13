#include "admin.h"
#include "account.h"
#include "book.h"
#include "borrowedBooks.h"

#include <string>

void Admin::addAccount(sql::Connection* con, const std::string& name, const std::string& email, const std::string& password, const std::string& phone, const std::string& role, std::function<void(std::string)> showMessage) {
    if (role != "admin" && role != "user" && role != "librarian") {
        showMessage("Invalid role. Please enter 'admin', 'user', or 'librarian'");
        return;
    }

    Account newAccount(name, email, password, phone, role);
    newAccount.insertAccount(con);
    showMessage("Account added successfully.");
}

void Admin::viewAccountById(sql::Connection* con, int accountId, std::function<void(std::string)> showMessage, std::function<void(Account)> displayAccount) {
    Account viewAccount(accountId);
    viewAccount.searchAccountByID(con);
    if (viewAccount.getAccountId() != 0) {
        displayAccount(viewAccount);
    } else {
        showMessage("No account found with that ID.");
    }
}

void Admin::viewAccountByEmail(sql::Connection* con, const std::string& email, std::function<void(std::string)> showMessage, std::function<void(Account)> displayAccount) {
    Account viewAccount(email);
    viewAccount.searchAccountByEmail(con);

    if (!viewAccount.getEmail().empty()) {
        displayAccount(viewAccount);
    } else {
        showMessage("No account found with that email.");
    }
}

void Admin::updateAccount(sql::Connection* con, int accountId, const std::string& name, const std::string& email, const std::string& phone, const std::string& role, std::function<void(std::string)> showMessage) {
    Account updateAccount(accountId);
    updateAccount.searchAccountByID(con);

    if (updateAccount.getAccountId() != 0) {
        if (!role.empty() && role != "admin" && role != "user" && role != "librarian") {
            showMessage("Invalid role. Please enter 'admin', 'user', or 'librarian'.");
            return;
        }

        if (!name.empty()) updateAccount.setName(name);
        if (!email.empty()) updateAccount.setEmail(email);
        if (!phone.empty()) updateAccount.setPhone(phone);
        if (!role.empty()) updateAccount.setRole(role);

        updateAccount.updateAccount(con);
        showMessage("Account updated successfully.");
    } else {
        showMessage("Account not found with ID: " + std::to_string(accountId));
    }
}

void Admin::updateRole(sql::Connection* con, int currentAdminId, int targetAccountId, const std::string& newRole, std::function<void(std::string)> showMessage) {
    Account currentUser(currentAdminId);
    currentUser.searchAccountByID(con);

    if (currentUser.getRole() != "admin") {
        showMessage("Access denied. Only admins can update roles.");
        return;
    }

    if (newRole != "admin" && newRole != "user" && newRole != "librarian") {
        showMessage("Invalid role. Please enter 'admin', 'user', or 'librarian'.");
        return;
    }

    Account targetAccount(targetAccountId);
    targetAccount.setRole(newRole);
    targetAccount.updateRole(con);

    showMessage("Role updated successfully.");
}

void Admin::deleteAccount(sql::Connection* con, int accountId, std::function<void(std::string)> showMessage) {
    Account deleteAccount(accountId);

    if (deleteAccount.getAccountId() != 0) {
        deleteAccount.deleteAccount(con);
        showMessage("Account deleted successfully.");
    } else {
        showMessage("Account not found with ID: " + std::to_string(accountId));
    }
}

void Admin::viewAllAccounts(sql::Connection* con, std::function<void(Account)> displayAccount) {  
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

void Admin::addBook(sql::Connection* con, const std::string& title, const std::string& author, const std::string& isbn, int year, int quantity, std::function<void(std::string)> showMessage) {
    Book newBook(title, author, isbn, year, quantity);
    newBook.insertBook(con);
    showMessage("Book added successfully.");
}

void Admin::updateBook(sql::Connection* con, const std::string& isbn, const std::string& title, const std::string& author, int year, int quantity, std::function<void(std::string)> showMessage) {
    Book updateBook(isbn);
    updateBook.searchBookByISBN(con);

    if (!updateBook.getISBN().empty()) {
        if (!title.empty()) updateBook.setTitle(title);
        if (!author.empty()) updateBook.setAuthor(author);
        if (year > 0) updateBook.setYear(year);
        if (quantity >= 0) updateBook.setQuantity(quantity);

        updateBook.updateBook(con);
        showMessage("Book updated successfully.");
    } else {
        showMessage("Book not found with ISBN: " + isbn);
    }
}

void Admin::deleteBook(sql::Connection* con, const std::string& isbn, std::function<void(std::string)> showMessage) {
    Book deleteBook(isbn);
    deleteBook.searchBookByISBN(con);

    if (!deleteBook.getISBN().empty()) {
        deleteBook.deleteBook(con);
        showMessage("Book deleted successfully.");
    } else {
        showMessage("Book not found with ISBN: " + isbn);
    }
}

void Admin::viewAllBooks(sql::Connection* con, std::function<void(Book)> displayBook) {
    Book viewAllBooks;
    std::vector<Book> books = viewAllBooks.displayAllBooks(con);
    for (const auto& book : books) {
        displayBook(book);
    }
}

void Admin::viewAllBorrowedBooks(sql::Connection* con, std::function<void(BorrowedBook)> displayBorrowedBook) {
    BorrowedBooksManager borrowedBooksManager(con);
    std::vector<BorrowedBook> borrowedBooks = borrowedBooksManager.getAllBorrowedBooks();
    for (const auto& book : borrowedBooks) {
        displayBorrowedBook(book);
    }
}

void Admin::viewBorrowedBooksByAccountId(sql::Connection* con, int accountId, std::function<void(BorrowedBook)> displayBorrowedBook) {
    BorrowedBooksManager borrowedBooksManager(con);
    std::vector<BorrowedBook> borrowedBooks = borrowedBooksManager.getBorrowedBooksByUserID(accountId);
    for (const auto& book : borrowedBooks) {
        displayBorrowedBook(book);
    }
}
