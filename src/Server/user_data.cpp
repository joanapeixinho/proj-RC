#include "user_data.hpp"

UserData::UserData(uint32_t id, const std::string& password)
    : id(id), password(password) {}

int UserData::getId() const {
    return id;
}

const std::string& UserData::getPassword() const {
    return password;
}


void UserData::login(uint32_t id, const std::string& password) {
    
  
    if (FileManager::UserInDir(std::to_string(id))) {
        std::string userPassword = FileManager::getUserPassword(std::to_string(id));
        if (userPassword == password) {
            FileManager::loginUser(std::to_string(id), password);
        } else {
            throw WrongPasswordException(password);
        }
    } else {
        registerUser(id, password);
    }
}

void UserData::registerUser(uint32_t id, const std::string& password) {
    
    //check UID IS VALID
    if (id < 100000 || id > 999999) {
        throw UserIdException(std::to_string(id));
    }

    //check password is valid
    
    if (password.length() != 8) {
        throw UserPasswordException(password);
    }

    for (char c : password) {
        if (!std::isalnum(c)) {
            throw UserPasswordException(password);
        }
    }

    this->id = id;
    this->password = password;

    //id from uint32_t to string
    std::string idString = std::to_string(id);
    
    FileManager::registerUser(idString, password);
}

// Path: src/Server/user_data.hpp
