#include <iostream>
#include <string>
#include <functional>

#include "account.h"
#include "dbConnection.h"
#include "librarian.h"
#include "admin.h"
#include "user.h"

class librarian_Menu {
    librarian lib;
public:
    void func(std::function<void(std::string)> showMessage) {
        int choice;
        do {
            system("cls");
            std::cout << "\n--- Librarian Menu ---\n";
            std::cout << "1. Add Book\n";
            std::cout << "2. Update Book\n";
            std::cout << "3. Delete Book\n";
            std::cout << "4. View All Available Books\n";
            std::cout << "5. View All Borrowed Books\n";
            std::cout << "6. View Borrowed Books by Account ID\n";
            std::cout << "7. Allocate Book to User\n";
            std::cout << "8. Revert Book Allocation\n";
            std::cout << "9. View Account by ID\n";
            std::cout << "10. View Account by Email\n";
            std::cout << "0. Logout\n";
            std::cout << "Enter your choice: ";
            std::cin >> choice;

            switch (choice) {
            case 1: {
                std::string title, author, isbn;
                int year, quantity;
                std::cout << "Enter book title: ";
                std::cin.ignore(); std::getline(std::cin, title);
                std::cout << "Enter author: ";
                std::getline(std::cin, author);
                std::cout << "Enter ISBN: ";
                std::getline(std::cin, isbn);
                std::cout << "Enter publication year: ";
                std::cin >> year;
                std::cout << "Enter quantity: ";
                std::cin >> quantity;
                lib.addBook(title, author, isbn, year, quantity, showMessage);
                break;
            }
            case 2: {
                std::string isbn, title, author;
                int year, quantity;
                std::cout << "Enter ISBN of book to update: ";
                std::cin.ignore(); std::getline(std::cin, isbn);
                std::cout << "Enter new title (or leave empty): ";
                std::getline(std::cin, title);
                std::cout << "Enter new author (or leave empty): ";
                std::getline(std::cin, author);
                std::cout << "Enter new year (or 0 to skip): ";
                std::cin >> year;
                std::cout << "Enter new quantity (-1 to skip): ";
                std::cin >> quantity;
                lib.updateBook(isbn, title, author, year, quantity, showMessage);
                break;
            }
            case 3: {
                std::string isbn;
                std::cout << "Enter ISBN of book to delete: ";
                std::cin.ignore(); std::getline(std::cin, isbn);
                lib.deleteBook(isbn, showMessage);
                break;
            }
            case 4: {
                lib.viewAvailableBooks(
                    [](Book b) {
                        std::cout << "\nTitle: " << b.getTitle()
                            << "\nAuthor: " << b.getAuthor()
                            << "\nISBN: " << b.getISBN()
                            << "\nYear: " << b.getYear()
                            << "\nQuantity: " << b.getQuantity() << "\n";
                    },
                    showMessage
                );
                break;
            }
            case 5: {
                librarian lib;
                lib.viewAllBorrowedBooks(
                    [](BorrowedBook book) {
                        std::cout << "Borrowed Book ID: " << book.getBorrowedBookId() << "\n";
                        std::cout << "Account ID: " << book.getAccountId() << "\n";
                        std::cout << "Book ID: " << book.getBookId() << "\n";
                        std::cout << "Borrow Date: " << book.getBorrowDate() << "\n";
                        std::cout << "Return Date: " << book.getReturnDate() << "\n";
                        std::cout << "-----------------------------\n";
                    },
                    showMessage
                );
                break;
            }

            case 6: {
                int accId;
                std::cout << "Enter account ID: ";
                std::cin >> accId;
                lib.viewBorrowedBooksByAccountId(accId,
                    [](BorrowedBook b) {
                        std::cout << "\nBorrowed Book - ID: " << b.getBookId()
                            << ", Borrow Date: " << b.getBorrowDate() << "\n";
                    },
                    showMessage
                );
                break;
            }
            case 7: {
                int accId, bookId;
                std::cout << "Enter account ID: ";
                std::cin >> accId;
                std::cout << "Enter book ID: ";
                std::cin >> bookId;
                lib.allocateBook(accId, bookId, showMessage, [](bool success) {
                    std::cout << (success ? "Book allocated successfully.\n" : "Failed to allocate book.\n");
                    });
                break;
            }
            case 8: {
                int accId, bookId;
                std::cout << "Enter account ID: ";
                std::cin >> accId;
                std::cout << "Enter book ID: ";
                std::cin >> bookId;
                lib.revertBookAllocation(accId, bookId, showMessage, [](bool success) {
                    std::cout << (success ? "Book reverted successfully.\n" : "Failed to revert book.\n");
                    });
                break;
            }
            case 9: {
                int accId;
                std::cout << "Enter account ID: ";
                std::cin >> accId;
                lib.viewAccountById(accId, showMessage, [](Account acc) {
                    std::cout << "\nID: " << acc.getAccountId()
                        << "\nEmail: " << acc.getEmail()
                        << "\nRole: " << acc.getRole() << "\n";
                    });
                break;
            }
            case 10: {
                std::string email;
                std::cout << "Enter email: ";
                std::cin.ignore(); std::getline(std::cin, email);
                lib.viewAccountByEmail(email, showMessage, [](Account acc) {
                    std::cout << "\nID: " << acc.getAccountId()
                        << "\nEmail: " << acc.getEmail()
                        << "\nRole: " << acc.getRole() << "\n";
                    });
                break;
            }
            case 0:
                showMessage("Logging out...");
                break;
            default:
                std::cout << "Invalid choice.\n";
            }
            system("pause");
        } while (choice != 0);
    }
};

class admin_Menu {
    Admin admin;
    void getLineSafe(std::string& input) {
        std::cin.ignore();
        std::getline(std::cin, input);
    }

    void displayBook(const Book& book) {
        std::cout << "\n--- Book Details ---\n";
        std::cout << "Title: " << book.getTitle() << "\n";
        std::cout << "ISBN: " << book.getISBN() << "\n";
        std::cout << "Author: " << book.getAuthor() << "\n";
        std::cout << "Year: " << book.getYear() << "\n\n";
    }

    void displayAccount(const Account& acc) {
        std::cout << "\n--- Account Details ---\n";
        std::cout << "ID: " << acc.getAccountId() << "\n";
        std::cout << "Name: " << acc.getName() << "\n";
        std::cout << "Email: " << acc.getEmail() << "\n";
        std::cout << "Phone: " << acc.getPhone() << "\n";
        std::cout << "Role: " << acc.getRole() << "\n\n";
    }
public:
    void func(std::function<void(std::string)> showMessage) {
        int choice;
        do {
            system("cls");
            std::cout << "\n--- Admin Menu ---\n";
            std::cout << "1. Add Account\n";
            std::cout << "2. Update Account\n";
            std::cout << "3. Update Role\n";
            std::cout << "4. Delete Account\n";
            std::cout << "5. View Account by ID\n";
            std::cout << "6. View Account by Email\n";
            std::cout << "7. View All Accounts\n";
            std::cout << "8. Add Book\n";
            std::cout << "9. Update Book\n";
            std::cout << "10. Delete Book\n";
            std::cout << "11. View All Books\n";
            std::cout << "12. Search Book by Title\n";
            std::cout << "13. Search Book by ISBN\n";
            std::cout << "14. Search Book by Author\n";
            std::cout << "15. View All Borrowed Books\n";
            std::cout << "16. View Borrowed Books by Account ID\n";
            std::cout << "0. Logout\n";
            std::cout << "Enter your choice: ";
            std::cin >> choice;

            switch (choice) {
            case 1: {
                std::string name, email, password, phone, role;
                std::cout << "Enter Name: "; getLineSafe(name);
                std::cout << "Enter Email: "; std::getline(std::cin, email);
                std::cout << "Enter Password: "; std::getline(std::cin, password);
                std::cout << "Enter Phone: "; std::getline(std::cin, phone);
                std::cout << "Enter Role (admin/user/librarian): "; std::getline(std::cin, role);
                admin.addAccount(name, email, password, phone, role, showMessage);
                break;
            }
            case 2: {
                int id;
                std::string name, email, phone, role;
                std::cout << "Enter Account ID: "; std::cin >> id;
                std::cout << "Enter New Name (or press enter to skip): "; getLineSafe(name);
                std::cout << "Enter New Email (or press enter to skip): "; std::getline(std::cin, email);
                std::cout << "Enter New Phone (or press enter to skip): "; std::getline(std::cin, phone);
                std::cout << "Enter New Role (or press enter to skip): "; std::getline(std::cin, role);
                admin.updateAccount(id, name, email, phone, role, showMessage);
                break;
            }
            case 3: {
                int currentId, targetId;
                std::string role;
                std::cout << "Enter Your Admin ID: "; std::cin >> currentId;
                std::cout << "Enter Target Account ID: "; std::cin >> targetId;
                std::cout << "Enter New Role (admin/user/librarian): "; getLineSafe(role);
                admin.updateRole(currentId, targetId, role, showMessage);
                break;
            }
            case 4: {
                int id;
                std::cout << "Enter Account ID to Delete: "; std::cin >> id;
                admin.deleteAccount(id, showMessage);
                break;
            }
            case 5: {
                int id;
                std::cout << "Enter Account ID: "; std::cin >> id;
                admin.viewAccountById(id, showMessage, [](Account acc) {
                    std::cout << acc.toString() << "\n";
                    });
                break;
            }
            case 6: {
                std::string email;
                std::cout << "Enter Email: "; getLineSafe(email);
                admin.viewAccountByEmail(email, showMessage, [](Account acc) {
                    std::cout << acc.toString() << "\n";
                    });
                break;
            }
            case 7: {
                admin.viewAllAccounts([&](Account acc) { displayAccount(acc); }, showMessage);
                break;
            }
            case 8: {
                std::string title, author, isbn;
                int year, quantity;
                std::cout << "Enter Title: "; getLineSafe(title);
                std::cout << "Enter Author: "; std::getline(std::cin, author);
                std::cout << "Enter ISBN: "; std::getline(std::cin, isbn);
                std::cout << "Enter Year: "; std::cin >> year;
                std::cout << "Enter Quantity: "; std::cin >> quantity;
                admin.addBook(title, author, isbn, year, quantity, showMessage);
                break;
            }
            case 9: {
                std::string isbn, title, author;
                int year, quantity;
                std::cout << "Enter ISBN: "; getLineSafe(isbn);
                std::cout << "Enter New Title: "; std::getline(std::cin, title);
                std::cout << "Enter New Author: "; std::getline(std::cin, author);
                std::cout << "Enter New Year: "; std::cin >> year;
                std::cout << "Enter New Quantity: "; std::cin >> quantity;
                admin.updateBook(isbn, title, author, year, quantity, showMessage);
                break;
            }
            case 10: {
                std::string isbn;
                std::cout << "Enter ISBN: "; getLineSafe(isbn);
                admin.deleteBook(isbn, showMessage);
                break;
            }
            case 11: {
                admin.viewAllBooks([](Book book) {
                    std::cout << book.toString() << "\n";
                    }, showMessage);
                break;
            }
            case 12: {
                std::string title;
                std::cout << "Enter Title: "; getLineSafe(title);
                auto foundBooks = admin.searchBookByTitle(title, showMessage);

                for (const auto& b : foundBooks) {
                    displayBook(b);
                }
                break;
            }
            case 13: {
                std::string isbn;
                std::cout << "Enter ISBN: "; getLineSafe(isbn);
                auto foundBooks = admin.searchBookByISBN(isbn, showMessage);
                for (const auto& b : foundBooks) {
                    displayBook(b);
                }
                break;
            }
            case 14: {
                std::string author;
                std::cout << "Enter Author: "; getLineSafe(author);
                auto foundBooks = admin.searchBookByAuthor(author, showMessage);
                for (const auto& b : foundBooks) {
                    displayBook(b);
                }
                break;
            }
            case 15: {
                admin.viewAllBorrowedBooks([](BorrowedBook b) {
                    std::cout << b.toString() << "\n";
                    }, showMessage);
                break;
            }
            case 16: {
                int id;
                std::cout << "Enter Account ID: ";
                std::cin >> id;
                admin.viewBorrowedBooksByAccountId(id, [](BorrowedBook b) {
                    std::cout << b.toString() << "\n";
                    }, showMessage);
                break;
            }
            case 0:
                showMessage("Logging out...");
                break;
            default:
                showMessage("Invalid choice. Try again.");
            }
            system("pause");
        } while (choice != 0);
    }
};

class user_Menu {};

int main() {
    std::function<void(std::string)> showMessage = [](const std::string& msg) {
        std::cout << msg << std::endl;
        };

    auto db = connectToDatabase(showMessage);
    if (!db) {
        std::cerr << "Failed to connect to the database." << std::endl;
        return 1;
    }
    int choice;
    while (true) {
        system("cls");
        std::cout << "\n--- Main Menu ---\n";
        std::cout << "1. Login\n";
        std::cout << "2. Exit\n";
        do {
            std::cout << "Enter your choice: ";
            std::cin >> choice;
        } while (std::cin.fail() || choice < 1 || choice > 2);

        switch (choice) {
        case 1: {
            std::string email, password;
            std::cout << "Enter email: ";
            std::cin >> email;
            std::cout << "Enter password: ";
            std::cin >> password;

            Account account(email);
            std::string searchResult = account.searchAccountByEmail(db.get());

            if (account.getEmail().empty()) {
                showMessage("No account found with this email.");
                break;
            }

            if (account.getPassword() != password) {
                showMessage("Incorrect password.");
                break;
            }

            std::string role = account.getRole();

            showMessage("Login successful. Role: " + role);

            if (role == "librarian") {
                librarian_Menu menu;
                showMessage("Redirecting to librarian menu...");
                menu.func(showMessage);
            }
            else if (role == "admin") {
                admin_Menu menu;
                showMessage("Redirecting to admin menu...");
                menu.func(showMessage);
            }
            else if (role == "user") {
                user_Menu menu;
                showMessage("Redirecting to user menu...");
                //menu.func(showMessage);
            }
            else {
                showMessage("Unknown role: " + role);
            }

        } break;
        case 2:
            return 0;
        default:
            std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }
    return 0;
}