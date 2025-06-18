#pragma once
#include <string>
#include <functional>

class Login {
private:
	std::string userID;
	std::string password;
public:
	Login(const std::string& user, const std::string& pass);

	std::string authenticate(std::function<void(std::string)> showMessage);
};
