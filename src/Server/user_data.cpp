#include "user_data.hpp"

UserData::UserData(int id, const std::string& username, const std::string& password)
    : id(id), username(username), password(password) {}

int UserData::getId() const {
    return id;
}

const std::string& UserData::getUsername() const {
    return username;
}

const std::string& UserData::getPassword() const {
    return password;
}
