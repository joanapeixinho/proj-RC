#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <getopt.h>

#include "User.hpp"

extern bool is_shutting_down;

int main(int argc, char *argv[]) {
    try {
        setup_signal_handlers();

        ClientConfig config(argc, argv);
        UserState state(config.host, config.port);

        CommandManager commandManager;
        registerCommands(commandManager);

        commandManager.printHelp();

        while (!std::cin.eof() && !is_shutting_down) {
            commandManager.waitForCommand(state);
        }

        if (state.isLoggedIn()){
          // Logout User before quitting
          LogoutCommand logoutCommand;
          try {
            std::cout << "\nLogging out..." << std::endl;
            logoutCommand.handle(" ", state);
          } catch (std::exception& e) {
            std::cout << "[ERROR] " << e.what() << std::endl;
          } catch (...) {
            std::cout << "[ERROR] An unknown error occurred." << std::endl;
          }
        }

        std::cout << std::endl
                << "Shutting down... Press CTRL + C (again) to forcefully close "
                    "the application."
                << std::endl;


    } catch (std::exception &e) {
        std::cerr << "Encountered unrecoverable error while running the "
                 "application. Shutting down..."
              << std::endl
              << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Encountered unrecoverable error while running the "
                 "application. Shutting down..."
              << std::endl;
        return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}


void registerCommands(CommandManager &manager) {
  manager.registerCommand(std::make_shared<LoginCommand>());        // UDP
  manager.registerCommand(std::make_shared<LogoutCommand>());       // UDP
  manager.registerCommand(std::make_shared<UnregisterCommand>());   // UDP
  manager.registerCommand(std::make_shared<ExitCommand>());
  manager.registerCommand(std::make_shared<OpenAuctionCommand>());  // TCP
  manager.registerCommand(std::make_shared<CloseAuctionCommand>()); // TCP
  manager.registerCommand(std::make_shared<ListMyAuctionsCommand>());   // UDP
  manager.registerCommand(std::make_shared<MyBidsCommand>());       // UDP
  manager.registerCommand(std::make_shared<ListCommand>());         // UDP
  manager.registerCommand(std::make_shared<ShowAssetCommand>());    // TCP
  manager.registerCommand(std::make_shared<BidCommand>());          // TCP
  manager.registerCommand(std::make_shared<ShowRecordCommand>());   // TCP
  manager.registerCommand(std::make_shared<HelpCommand>(manager));
}

ClientConfig::ClientConfig(int argc, char *argv[]) {
  program_path = argv[0];
  int opt;

  while ((opt = getopt(argc, argv, "n:p:")) != -1) {
    switch (opt) {
      case 'n':
        host = std::string(optarg);
        break;
      case 'p':
        port = std::string(optarg);
        break;
      default:
        std::cerr << std::endl;
        printHelp(std::cerr);
        exit(EXIT_FAILURE);
    }
  }

  validate_port_number(port);
}

void ClientConfig::printHelp(std::ostream &stream) {
  stream << "Usage: " << program_path << " [-n GSIP] [-p GSport]"
         << std::endl;
  stream << "Available options:" << std::endl;
  stream << "-n GSIP\t\tSet hostname of Auction Server. Default: "
         << DEFAULT_HOSTNAME << std::endl;
  stream << "-p GSport\tSet port of Auction Server. Default: " << DEFAULT_PORT
         << std::endl;
  stream << "-h\t\tPrint this menu." << std::endl;
}