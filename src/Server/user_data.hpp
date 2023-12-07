#ifndef USERDATA_HPP
#define USERDATA_HPP

#include <string>
#include "common/file_manager.hpp"

class UserData {
public:
    UserData(uint32_t id, const std::string& password);
    
    int getId() const;
    const std::string& getPassword() const;
    void login(uint32_t id, const std::string& password);

private:
    uint32_t id;
    std::string password;
};

#endif  // USERDATA_HPP
