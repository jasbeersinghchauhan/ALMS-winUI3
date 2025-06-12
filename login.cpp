#include "account.h"
#include "DatabaseService.h"
#include "login.h"
#include <functional>


	Login::Login(const std::string& user, const std::string& pass) : userID(user), password(pass) {}

	std::string Login::authenticate(std::function<void(std::string)> showMessage) {
		std::string userRole = "";
		DatabaseService::withConnection(showMessage, [&](sql::Connection* con) {
			std::string query = "SELECT * FROM accounts WHERE account_id = ? AND password = ?";
			std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(query));
			pstmt->setString(1, userID);
			pstmt->setString(2, password);
			std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
			if(res->next()) {
				userRole = res->getString("role");
				showMessage("Login successful!");
			} else {
				showMessage("Login failed. Please check your credentials.");
			}
		});
		return userRole;
	}
