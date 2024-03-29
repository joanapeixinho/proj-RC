#include "common.hpp"

#include <csignal>

bool is_shutting_down = false;

void validate_port_number(std::string &port) {
  for (char c : port) {
    if (!std::isdigit(static_cast<unsigned char>(c))) {
      throw UnrecoverableError("Invalid port: not a number");
    }
  }

  try {
    int32_t parsed_port = std::stoi(port);
    if (parsed_port <= 0 || parsed_port > ((1 << 16) - 1)) {
      throw std::runtime_error("");
    }
  } catch (...) {
    throw UnrecoverableError(
        "Invalid port: it must be a number between 1 and 65535");
  }
}

void setup_signal_handlers() {
  // set SIGINT/SIGTERM handler to close server gracefully
  struct sigaction sa;
  sa.sa_handler = terminate_signal_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;

  if (sigaction(SIGINT, &sa, NULL) == -1) {
    throw UnrecoverableError("Setting SIGINT signal handler", errno);
  }
  if (sigaction(SIGTERM, &sa, NULL) == -1) {
    throw UnrecoverableError("Setting SIGTERM signal handler", errno);
  }

  // ignore SIGPIPE
  signal(SIGPIPE, SIG_IGN);
}

void terminate_signal_handler(int sig) {
  (void)sig;
  if (is_shutting_down) {
    exit(EXIT_SUCCESS);
  }
  is_shutting_down = true;
}