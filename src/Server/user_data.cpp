#include "user_data.hpp"

UserData::UserData(uint32_t id, const std::string &password, FileManager &fileManager)
    : id(id), password(password), fileManager(fileManager)
{
    // check UID IS VALID
    if (id < 100000 || id > 999999)
    {
        throw UserIdException(std::to_string(id));
    }
    // check password is valid
    if (password.length() != 8)
    {
        throw UserPasswordException(password);
    }

    for (char c : password)
    {
        if (!std::isalnum(c))
        {
            throw UserPasswordException(password);
        }
    }
}

UserData::UserData(uint32_t id, FileManager &fileManager)
    : id(id), fileManager(fileManager) {}

int UserData::getId() const
{
    return id;
}

std::string UserData::getIdString() const
{
    std::ostringstream oss;
    oss << std::setw(USER_ID_MAX_LEN) << std::setfill('0') << id;
    return oss.str();
}

const std::string &UserData::getPassword() const
{
    return password;
}

bool UserData::passwordIsCorrect(const std::string &password)
{
    std::string userPassword = fileManager.getUserPassword(std::to_string(this->id));
    if (userPassword == password)
    {
        return true;
    }
    else
    {
        throw WrongPasswordException(this->password);
        return false;
    }
}

void UserData::login()
{

    if (fileManager.UserRegistered(std::to_string(this->id)))
    {
        if (fileManager.UserLoggedIn(std::to_string(this->id)))
        {
            throw UserAlreadyLoggedInException(std::to_string(this->id));
        }
        else
        {
            if (passwordIsCorrect(this->password))
            {
                fileManager.loginUser(std::to_string(this->id));
            }
        }
    }
    else
    {
        registerUser();
        fileManager.loginUser(std::to_string(this->id));
        throw UserNotRegisteredException(std::to_string(this->id));
    }
}

void UserData::logout()
{

    if (!fileManager.UserRegistered(std::to_string(this->id)))
    {
        throw UserNotRegisteredException(std::to_string(this->id));
    }
    else if (!fileManager.UserLoggedIn(std::to_string(this->id)))
    {
        throw UserNotLoggedInException(std::to_string(this->id));
    }
    else
    {
        fileManager.logoutUser(std::to_string(this->id));
    }
}

void UserData::registerUser()
{

    std::string idString = std::to_string(id);
    fileManager.registerUser(idString, password);
}

void UserData::unregisterUser()
{

    if (!fileManager.UserRegistered(std::to_string(this->id)))
    {
        throw UserNotRegisteredException(std::to_string(this->id));
    }
    std::string idString = std::to_string(id);
    fileManager.unregisterUser(idString);
}

void UserData::openAuction(const AuctionData &data, const std::string &password)
{

    if (!fileManager.UserLoggedIn(std::to_string(this->id)))
    {
        throw UserNotLoggedInException(std::to_string(this->id));
    }
    else if (!passwordIsCorrect(password))
    {
        throw WrongPasswordException(password);
    }
    else
    {
        std::string idString = std::to_string(id);
        fileManager.openAuction(idString, data);
    }
}


void UserData::closeAuction(AuctionData &auction)
{

    // check if auction belongs to user
    if (auction.getOwnerId() != this->id)
    {
        throw AuctionDoesNotBelongToUserException(auction.getIdString(), this->getIdString());
    }
    fileManager.closeAuction(auction);
}

std::vector<std::pair<uint32_t, bool>> UserData::listMyAuctions(const std::string &directory)
{
    std::vector<std::pair<uint32_t, bool>> auctions;
    if (fileManager.UserLoggedIn(std::to_string(this->id)))
    {
        std::string idString = std::to_string(id);

        auctions = fileManager.getUserAuctions(idString, directory);

        if (auctions.empty())
        {
            throw UserHasNoAuctionsException(idString);
        }
    }
    else
    {
        throw UserNotLoggedInException(std::to_string(this->id));
    }

    return auctions;
}

// Path: src/Server/user_data.hpp