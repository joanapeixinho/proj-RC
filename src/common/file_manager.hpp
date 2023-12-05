// file_manager.h
#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

class FileManager {
public:
    void initServer(/* parameters */);
    void saveAuction(/* parameters */);
    void loadAuction(/* parameters */);
    bool writeToFile(const std::string& fileName, const std::string& data, const std::string& directory);
    bool readFromFile(const std::string& fileName, std::string& data, const std::string& directory);
    void saveUser(/* parameters */);
    void loadUser(/* parameters */);

    // Outras operações relacionadas ao gerenciamento de arquivos
};

#endif