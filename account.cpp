#include "account.h"
#include <iostream>
#include <set>
#include <iomanip>
#include <sstream>
#include <cppconn/prepared_statement.h>

Account::Account() : accountId(0) {}
Account::Account(int accountId) : accountId(accountId) {}
Account::Account(int accountId, std::string role) : accountId(accountId), role(std::move(role)) {}
Account::Account(std::string email) : accountId(0), email(std::move(email)) {}
Account::Account(std::string name, std::string email, std::string password, std::string phone, std::string role)
    : accountId(0), name(std::move(name)), email(std::move(email)), password(std::move(password)),
    phone(std::move(phone)), role(std::move(role)) {
}

void Account::setName(const std::string& n) { name = n; }
void Account::setEmail(const std::string& e) { email = e; }
void Account::setPassword(const std::string& p) { password = p; }
void Account::setPhone(const std::string& p) { phone = p; }
void Account::setRole(const std::string& r) { role = r; }

int Account::getAccountId() const { return accountId; }
std::string Account::getName() const { return name; }
std::string Account::getEmail() const { return email; }
std::string Account::getPassword() const { return password; }
std::string Account::getPhone() const { return phone; }
std::string Account::getRole() const { return role; }
std::string Account::getRegisteredAt() const { return registeredAt; }

void Account::loadFromResultSet(sql::ResultSet* res) {
    if (res) {
        accountId = res->getInt("account_id");
        name = res->getString("name");
        email = res->getString("email");
        password = res->getString("password");
        phone = res->getString("phone");
        role = res->getString("role");
        registeredAt = res->getString("registered_at");
    }
}

bool Account::isValidRole(const std::string& r) {
    static const std::set<std::string> validRoles = { "user", "admin", "librarian" };
    return validRoles.count(r) > 0;
}

std::string Account::insertAccount(sql::Connection* con) {
    if (!isValidRole(role)) {
        return "Invalid role: " + role;
    }

    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "INSERT IGNORE INTO accounts (name, email, password, phone, role) "
                "VALUES (?, ?, ?, ?, ?)"));
        pstmt->setString(1, name);
        pstmt->setString(2, email);
        pstmt->setString(3, password);
        pstmt->setString(4, phone);
        pstmt->setString(5, role);

        int affectedRows = pstmt->executeUpdate();
        if (affectedRows == 0) {
            return "Insert skipped: Account with email " + email + "' already exists.";
        }
        else {
            return  "Account inserted successfully.";
        }
    }
    catch (sql::SQLException& e) {
        return "Insert failed: " + std::string(e.what());
    }
}

std::string Account::updateAccount(sql::Connection* con) {
    if (!isValidRole(role)) {
        return "Invalid role: " + role;
    }

    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "UPDATE accounts SET name = ?, password = ?, phone = ?, role = ? WHERE email = ?"));
        pstmt->setString(1, name);
        pstmt->setString(2, password);
        pstmt->setString(3, phone);
        pstmt->setString(4, role);
        pstmt->setString(5, email);

        int affected = pstmt->executeUpdate();
        if (affected == 0) {
            return "No account found with the given email.";
        }
        else {
            return "Account updated successfully.";
        }
    }
    catch (sql::SQLException& e) {
        return "Update failed: " + std::string(e.what());
    }
}


std::string Account::updateRole(sql::Connection* con) {
    if (!isValidRole(role)) {
        return "Invalid role: " + role;
    }

    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("UPDATE accounts SET role = ? WHERE account_id = ?"));
        pstmt->setString(1, role);
        pstmt->setInt(2, accountId);

        int affected = pstmt->executeUpdate();
        if (affected == 0) {
            return "No account found with the given account ID.";
        }
        else {
            return "Role updated successfully.";
        }
    }
    catch (sql::SQLException& e) {
        return "Update role failed: " + std::string(e.what());
    }
}


std::string Account::deleteAccount(sql::Connection* con) {
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("DELETE FROM accounts WHERE email = ?"));
        pstmt->setString(1, email);

        int affected = pstmt->executeUpdate();
        if (affected == 0) {
            return "No account found with the given email";
        }
        else {
            return "Account deleted successfully.";
        }
    }
    catch (sql::SQLException& e) {
        return "Delete failed: " + std::string(e.what());
    }
}


std::string Account::searchAccountByEmail(sql::Connection* con) {
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("SELECT * FROM accounts WHERE email = ?"));
        pstmt->setString(1, email);

        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        if (res->next()) {
            loadFromResultSet(res.get());
            return "Account found.";
        }
        else {
            return "No account found with that email";
        }
    }
    catch (sql::SQLException& e) {
        return "Search by email failed: " + std::string(e.what());
    }
}


std::string Account::searchAccountByID(sql::Connection* con) {
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("SELECT * FROM accounts WHERE account_id = ?"));
        pstmt->setInt(1, accountId);

        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        if (res->next()) {
            loadFromResultSet(res.get());
            return "Account found.";
        }
        else {
            return "No account found with that ID";
        }
    }
    catch (sql::SQLException& e) {
        return "Search by ID failed: " + std::string(e.what());
    }
}


std::vector<Account> Account::displayAllAccounts(sql::Connection* con, std::string& outError) {
    std::vector<Account> accounts;

    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("SELECT * FROM accounts"));
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        while (res->next()) {
            Account acc;
            acc.loadFromResultSet(res.get());
            accounts.push_back(acc);
        }
    }
    catch (sql::SQLException& e) {
        outError = "Query failed: " + std::string(e.what());
    }

    return accounts;
}