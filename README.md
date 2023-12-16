# Project RC 2022/2023 - Group 37

## Compilation

The project can be compiled by executing `make` in this directory.
This project utilizes C++17.

Once compiled, two binaries, `user` and `AS`, will be placed in this directory.

## Run the client

The available options for the `user` executable can be viewed by executing:

```bash
./user -h
```

Once the client is running, it will prompt the user for a command.
The list of commands is shown at startup and can be displayed again at any time by typing `help` or `?` at the prompt.

All commands function according to the specification, with a highlight on the `show_asset` command, which allows canceling an ongoing download.

The user logs out every time the program terminates, even when receiving a SIGINT or SIGTERM signal.

## Run the server

The available options for the `AS` executable can be viewed by executing:

```bash
./AS -h
```

The server persists data between sessions in the `ASDIR` folder, so during testing, it might make sense to delete the folder after each test.
To delete all files in the ASDIR directory, simply execute "make clean-asdir" in this directory.
Files can be opened like any other file to inspect the current state of the server.

The server also handles the SIGINT signal (CTRL + C), waiting for existing TCP connections to finish. Users can press CTRL + C again to force exit the server.

We decided to use threads for concurrency. By default, the server supports up to 50 simultaneous TCP connections, but this can be adjusted by the `TCP_WORKER_POOL_SIZE` variable in `src/common/constants.hpp`.
We use mutexes to synchronize access to shared variables.

