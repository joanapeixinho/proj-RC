#ifndef USERDATA_HPP
#define USERDATA_HPP

#include <string>

class UserData {
public:
    UserData(int id, const std::string& username, const std::string& password);
    
    int getId() const;
    const std::string& getUsername() const;
    const std::string& getPassword() const;

private:
    int id;
    std::string username;
    std::string password;
};

#endif  // USERDATA_HPP
