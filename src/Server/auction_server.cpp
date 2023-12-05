#include "auction_server.hpp"

AuctionServer::AuctionServer() {
    // Inicialize o servidor, carregue dados, etc.
    loadAuctions();
    loadUsers();
}

AuctionServer::~AuctionServer() {
    // Faça qualquer limpeza necessária ao encerrar o servidor
    saveAuctions();
    saveUsers();
}

void AuctionServer::start() {
    // Implemente a lógica para iniciar o servidor de leilões
    // Pode envolver aceitar conexões, processar comandos, etc.
}

void AuctionServer::stop() {
    // Implemente a lógica para parar o servidor de leilões
    // Pode envolver fechar conexões, salvar dados, etc.
}

void AuctionServer::loadAuctions() {
    // Implemente a lógica para carregar dados dos leilões do arquivo
    // Use o FileManager para ler os dados
}

void AuctionServer::loadUsers() {
    // Implemente a lógica para carregar dados dos usuários do arquivo
    // Use o FileManager para ler os dados
}

void AuctionServer::saveAuctions() {
    // Implemente a lógica para salvar dados dos leilões no arquivo
    // Use o FileManager para escrever os dados
}

void AuctionServer::saveUsers() {
    // Implemente a lógica para salvar dados dos usuários no arquivo
    // Use o FileManager para escrever os dados
}
