#include "user_data.hpp"

UserData::UserData(uint32_t id, const std::string& password)
    : id(id), password(password) {
    
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

    }

UserData::UserData(const std::string& id)
    : id(std::stoi(id)) {}

UserData::UserData() : id(NULL), password("") {}

int UserData::getId() const {
    return id;
}

const std::string& UserData::getPassword() const {
    return password;
}


void UserData::login() {
  
    if (FileManager::UserRegistered(std::to_string(this->id))) {
        std::string userPassword = FileManager::getUserPassword(std::to_string(this->id));
        if (userPassword == this->password) {
            FileManager::loginUser(std::to_string(this->id));
        } else {
            throw WrongPasswordException(this->password);
        }
    } else {
        registerUser();
        FileManager::loginUser(std::to_string(this->id));
        throw UserNotRegisteredException(std::to_string(this->id));
    }
}

void UserData::logout() {

    if (!FileManager::UserRegistered(std::to_string(this->id))) {
        throw UserNotRegisteredException(std::to_string(this->id));
    } else if (!FileManager::UserLoggedIn(std::to_string(this->id))) {
        throw UserNotLoggedInException(std::to_string(this->id));
    } else {
         FileManager::logoutUser(std::to_string(this->id));
    }
   
}

void UserData::registerUser() {

    std::string idString = std::to_string(id);
    FileManager::registerUser(idString, password);
}

void UserData::unregisterUser() {

    if (!FileManager::UserRegistered(std::to_string(this->id))) {
        throw UserNotRegisteredException(std::to_string(this->id));
    }
    std::string idString = std::to_string(id);
    FileManager::unregisterUser(idString);
}


void UserData::openAuction (const AuctionData& data) {
    
    std::string idString = std::to_string(id);
    FileManager::openAuction(idString, data);
}
// Path: src/Server/user_data.hpp
