#ifndef AUCTION_SERVER_HPP
#define AUCTION_SERVER_HPP

#include <iostream>
#include <vector>
#include "auction_data.hpp"
#include "user_data.hpp"
#include "file_manager.hpp"

class AuctionServer {
public:
    AuctionServer(std::string auctionsFile, std::string usersFile);  // Construtor

    void start();     // Iniciar o servidor
    void stop();      // Parar o servidor

private:
    std::vector<AuctionData> auctions;  // Lista de leilões
    std::vector<UserData> users;        // Lista de usuários
    FileManager fileManager;           // Gerenciador de arquivos

    void loadAuctions();  // Carregar informações dos leilões do arquivo
    void loadUsers();     // Carregar informações dos usuários do arquivo
    void saveAuctions();  // Salvar informações dos leilões no arquivo
    void saveUsers();     // Salvar informações dos usuários no arquivo
};

#endif // AUCTION_SERVER_HPP
