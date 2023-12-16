#include "user_data.hpp"

UserData::UserData(uint32_t __id, const std::string &__password,
                   FileManager &__fileManager)
    : id(__id), password(__password), fileManager(__fileManager) {
  // check UID IS VALID
  if (id > USER_ID_MAX) {
    throw UserIdException(std::to_string(id));
  }
  // check password is valid
  if (password.length() != 8) {
    throw UserPasswordException(password);
  }

  for (char c : password) {
    if (!std::isalnum(c)) {
      throw UserPasswordException(password);
    }
  }
}

UserData::UserData(uint32_t __id, FileManager &__fileManager)
    : id(__id), fileManager(__fileManager) {}

uint32_t UserData::getId() const { return id; }

std::string UserData::getIdString() const {
  std::ostringstream oss;
  oss << std::setw(USER_ID_STR_LEN) << std::setfill('0') << id;
  return oss.str();
}

const std::string &UserData::getPassword() const { return password; }

bool UserData::passwordIsCorrect(const std::string &_password) {
  std::string userPassword =
      fileManager.getUserPassword(std::to_string(this->id));
  if (userPassword == _password) {
    return true;
  } else {
    throw WrongPasswordException(this->password);
    return false;
  }
}

void UserData::login() {

  if (fileManager.UserRegistered(std::to_string(this->id))) {
    if (fileManager.UserLoggedIn(std::to_string(this->id))) {
      throw UserAlreadyLoggedInException(std::to_string(this->id));
    } else {
      if (passwordIsCorrect(this->password)) {
        fileManager.loginUser(std::to_string(this->id));
      } else {
        throw WrongPasswordException(this->password);
      }
    }
  } else {
    registerUser();
    fileManager.loginUser(std::to_string(this->id));
    throw UserNotRegisteredException(std::to_string(this->id));
  }
}

void UserData::logout() {

  if (!fileManager.UserRegistered(std::to_string(this->id))) {
    throw UserNotRegisteredException(std::to_string(this->id));
  } else if (!fileManager.UserLoggedIn(std::to_string(this->id))) {
    throw UserNotLoggedInException(std::to_string(this->id));
  } else {
    fileManager.logoutUser(std::to_string(this->id));
  }
}

void UserData::registerUser() {

  std::string idString = std::to_string(id);
  fileManager.registerUser(idString, password);
}

void UserData::unregisterUser() {

  if (!fileManager.UserRegistered(std::to_string(this->id))) {
    throw UserNotRegisteredException(std::to_string(this->id));
  }
  std::string idString = std::to_string(id);
  fileManager.unregisterUser(idString);
}

void UserData::openAuction(const AuctionData &data,
                           const std::string &_password) {

  if (!fileManager.UserLoggedIn(std::to_string(this->id))) {
    throw UserNotLoggedInException(std::to_string(this->id));
  } else if (!passwordIsCorrect(_password)) {
    throw WrongPasswordException(_password);
  } else {
    std::string idString = std::to_string(this->id);
    fileManager.openAuction(idString, data);
  }
}

void UserData::closeAuction(AuctionData &auction) {

  // check if auction belongs to user
  if (auction.getOwnerId() != this->id) {
    throw AuctionDoesNotBelongToUserException(auction.getIdString(),
                                              this->getIdString());
  }
  fileManager.closeAuction(auction);
}

std::vector<std::pair<uint32_t, bool>>
UserData::listMyAuctions(const std::string &directory) {
  std::vector<std::pair<uint32_t, bool>> auctions;
  if (fileManager.UserLoggedIn(std::to_string(this->id))) {
    std::string idString = std::to_string(id);

    auctions = fileManager.getUserAuctions(idString, directory);

    if (auctions.empty()) {
      throw UserHasNoAuctionsException(idString);
    }
  } else {
    throw UserNotLoggedInException(std::to_string(this->id));
  }

  return auctions;
}

void UserData::bid(AuctionData &auction, uint32_t bidValue,
                   const std::string &_password) {

  if (!fileManager.UserLoggedIn(std::to_string(this->id))) {
    throw UserNotLoggedInException(std::to_string(this->id));
  } else if (!passwordIsCorrect(_password)) {
    throw WrongPasswordException(_password);
  } else if (auction.getOwnerId() == this->id) {
    throw UserIsOwnerException(this->getIdString());
  } else if (bidValue > BID_MAX_VALUE) {
    throw BidValueException(std::to_string(bidValue));
  } else if (bidValue <= auction.getHighestBidValue()) {
    throw LargerBidAlreadyExistsException(std::to_string(bidValue));
  } else {
    fileManager.bid(auction, bidValue, this->getIdString());
  }
}

// Path: src/Server/user_data.hpp