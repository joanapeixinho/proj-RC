// exceptions.hpp
#include <stdexcept>
#include <string>

class FileOpenException : public std::runtime_error {
public:
    FileOpenException(const std::string& filename)
        : std::runtime_error("Failed to open file: " + filename) {}
};

class FileWriteException : public std::runtime_error {
public:
    FileWriteException(const std::string& filename)
        : std::runtime_error("Failed to write to file: " + filename) {}
};

class FileReadException : public std::runtime_error {
public:
    FileReadException(const std::string& filename)
        : std::runtime_error("Failed to read from file: " + filename) {}
};

class UserIdException : public std::runtime_error {
public:
    UserIdException(const std::string& userId)
        : std::runtime_error("Invalid user id: " + userId) {}
};

class UserPasswordException : public std::runtime_error {
public:
    UserPasswordException(const std::string& userPassword)
        : std::runtime_error("Invalid user password: " + userPassword + '\n' + "Must be a 8 alphanumeric string") {}
};

class AuctionIdException : public std::runtime_error {
public:
    AuctionIdException(const std::string& auctionId)
        : std::runtime_error("Invalid auction id: " + auctionId) {}
};

class BidValueException : public std::runtime_error {
public:
    BidValueException(const std::string& bidValue)
        : std::runtime_error("Invalid bid value: " + bidValue) {}
};

class WrongPasswordException : public std::runtime_error {
public:
    WrongPasswordException(const std::string& password)
        : std::runtime_error("Wrong password: " + password) {}
};



