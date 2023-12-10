#ifndef USERDATA_HPP
#define USERDATA_HPP

#include <string>

#include "../common/exceptions.hpp"
#include "../common/file_manager.hpp"

class UserData {
public:
    UserData(uint32_t id, const std::string& password, FileManager& fileManager);
    UserData(uint32_t id, FileManager& fileManager);
    uint32_t getId() const;
    std::string getIdString() const;
    const std::string& getPassword() const;
    void login();
    void logout();
    void registerUser();
    void unregisterUser();
    std::vector <std::pair<uint32_t, bool>> listMyAuctions(const std::string& directory);
    void openAuction (const AuctionData& data, const std::string& password);
    void closeAuction (AuctionData& auction);
    bool passwordIsCorrect(const std::string& password);
    void bid(AuctionData& auction, uint32_t bidValue, const std::string& password);

private:
    uint32_t id;
    std::string password;
    FileManager& fileManager;
};

#endif  // USERDATA_HPP
