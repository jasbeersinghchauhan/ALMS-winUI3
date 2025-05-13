#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>
#include <vector>
#include <memory>
#include <cppconn/resultset.h>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>

class Account {
private:
    int accountId;
    std::string name;
    std::string email;
    std::string password;
    std::string phone;
    std::string role;
    std::string registeredAt;

    void loadFromResultSet(sql::ResultSet* res);
    static bool isValidRole(const std::string& r);

public:
    // Constructors
    Account();
    Account(int accountId);
    Account(int accountId, std::string role);
    Account(std::string email);
    Account(std::string name, std::string email, std::string password, std::string phone, std::string role);

    // Setters
    void setName(const std::string& n);
    void setEmail(const std::string& e);
    void setPassword(const std::string& p);
    void setPhone(const std::string& p);
    void setRole(const std::string& r);

    // Getters
    int getAccountId() const;
    std::string getName() const;
    std::string getEmail() const;
    std::string getPassword() const;
    std::string getPhone() const;
    std::string getRole() const;
    std::string getRegisteredAt() const;

    // Database operations
    std::string insertAccount(sql::Connection* con);
    std::string updateAccount(sql::Connection* con);
    std::string updateRole(sql::Connection* con);
    std::string deleteAccount(sql::Connection* con);
    std::string searchAccountByEmail(sql::Connection* con);
    std::string searchAccountByID(sql::Connection* con);

    static std::vector<Account> displayAllAccounts(sql::Connection* con, std::string& outError);
};

#endif