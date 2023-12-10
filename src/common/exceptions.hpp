// exceptions.hpp
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <string>
#include <stdexcept>

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

class UserNotRegisteredException : public std::runtime_error {
public:
    UserNotRegisteredException(const std::string& userId)
        : std::runtime_error("User not registered: " + userId) {}
};

class UserNotLoggedInException : public std::runtime_error {
public:
    UserNotLoggedInException(const std::string& userId)
        : std::runtime_error("User not logged in: " + userId) {}
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

class InvalidAuctionNameException : public std::runtime_error { 
public:
    InvalidAuctionNameException(const std::string& auctionName)
        : std::runtime_error("Invalid auction name: " + auctionName) {}
};

class InvalidAuctionInitialBidException : public std::runtime_error {
public:
    InvalidAuctionInitialBidException(const std::string& initialBid)
        : std::runtime_error("Invalid auction initial bid: " + initialBid) {}
};

class InvalidAuctionDurationException : public std::runtime_error {
public:
    InvalidAuctionDurationException(const std::string& duration)
        : std::runtime_error("Invalid auction duration: " + duration) {}
};

class InvalidAuctionAssetException : public std::runtime_error {
public:
    InvalidAuctionAssetException(const std::string& asset)
        : std::runtime_error("Invalid auction asset: " + asset) {}
};

class InvalidAuctionIdException : public std::runtime_error {
public:
    InvalidAuctionIdException(const std::string& auctionId)
        : std::runtime_error("Invalid auction id: " + auctionId) {}
};

class MaximumAuctionsException : public std::runtime_error {
public:
    MaximumAuctionsException(const std::string& auctionId)
        : std::runtime_error("Maximum number of auctions reached: " + auctionId) {}
};
    
class AuctionNotActiveException : public std::runtime_error {
public:
    AuctionNotActiveException(const std::string& auctionId)
        : std::runtime_error("Auction not active: " + auctionId) {}
};

class UserHasNoAuctionsException : public std::runtime_error {
public:
    UserHasNoAuctionsException(const std::string& userId)
        : std::runtime_error("User has no auctions: " + userId) {}
};

class NoAuctionsException : public std::runtime_error {
public:
    NoAuctionsException()
        : std::runtime_error("No auctions: ") {}
};

class AuctionDoesNotExistException : public std::runtime_error {
public:
    AuctionDoesNotExistException(const std::string& auctionId)
        : std::runtime_error("Auction does not exist: " + auctionId) {}
};

#endif

