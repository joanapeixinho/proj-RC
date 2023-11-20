#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <getopt.h>


#define PORT__TEST "58011"
#define PORT "58037"

std::string ASIP = "localhost"; // default value
std::string ASport = "58000"; // default value
int fd, errcode;
ssize_t n;
socklen_t addrlen; // Tamanho do endereço
/*
hints - Estrutura que contém informações sobre o tipo de conexão que será estabelecida.
        Podem-se considerar, literalmente, dicas para o sistema operacional sobre como
        deve ser feita a conexão, de forma a facilitar a aquisição ou preencher dados.

res - Localização onde a função getaddrinfo() armazenará informações sobre o endereço.
*/
struct addrinfo hints, *res;
struct sockaddr_in addr;

class UserApplication {
public:
    // Implement necessary functions for user actions
    void login();
    void openAuction();
    // Add other functions based on your specifications
};

int main(int argc, char *argv[]) {
    // Implement the main loop for the User Application
    UserApplication userApp;
    int opt;

    while ((opt = getopt(argc, argv, "n:p:")) != -1) {
        switch (opt) {
        case 'n':
            ASIP = optarg;
            break;
        case 'p':
            ASport = optarg;
            break;
        default:
            std::cerr << "Usage: " << argv[0] << " [-n ASIP] [-p ASport]\n";
            return 1;
        }
    }


    // Handle user input and send requests to the Auction Server
    return 0;
}

int login() {
    /* login UID password – following this command the User application sends 
a message to the AS, using the UDP protocol, confirm the ID, UID, and 
password of this user, or register it if this UID is not present in the AS
database */

}