#include "user_data.hpp"

UserData::UserData(uint32_t id, const std::string& password)
    : id(id), password(password) {}

int UserData::getId() const {
    return id;
}

const std::string& UserData::getPassword() const {
    return password;
}

/* Login. Each user is identified by a user ID UID, a 6-digit IST student number, and a
password composed of 8 alphanumeric (only letters and digits) characters. When the
AS receives a login request it will inform of a successful or incorrect login attempt
or, in case the UID was not present at the AS, register a new user. */

void UserData::login(uint32_t id, const std::string& password) {



}

// Path: src/Server/user_data.hpp
