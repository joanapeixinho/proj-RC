#ifndef USERDATA_HPP
#define USERDATA_HPP

#include <string>

#include "../common/exceptions.hpp"
#include "../common/file_manager.hpp"

class UserData {
public:
    UserData(uint32_t id, const std::string& password, FileManager& fileManager);
    UserData(uint32_t id, FileManager& fileManager);
    
    int getId() const;
    const std::string& getPassword() const;
    void login();
    void logout();
    void registerUser();
    void unregisterUser();
    std::vector <std::pair<uint32_t, bool>> listMyAuctions(const std::string& directory);
    void openAuction (const AuctionData& data);

private:
    uint32_t id;
    std::string password;
    FileManager& fileManager;
};

#endif  // USERDATA_HPP
