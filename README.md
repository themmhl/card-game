# Card Game Project
Created by Mohammad mahdi heydari & Hosein poorpirali
This is a client-server card game built with C++ and Qt/QML.

## Features

* **User Authentication**: Sign up, login, and logout functionality.
* **Game Lobby**: Join a game and wait for an opponent.
* **Real-time Gameplay**: Play a card game against another player in real-time.
* **Game History**: View a history of your past games.

## Tech Stack

* **Backend**: C++ with Qt for networking and application logic.
* **Frontend**: QML for the user interface.

## How to Build

1.  **Prerequisites**:
    * A C++ compiler
    * CMake
    * Qt 6 (specifically, the `Quick`, `Network`, and `Core` modules)

2.  **Configure CMake**:
    * Open `CMakeLists.txt`.
    * Update the `CMAKE_PREFIX_PATH` to point to your Qt 6 installation directory.

3.  **Build the Project**:
    * Create a build directory: `mkdir build && cd build`
    * Run CMake: `cmake ..`
    * Compile the project: `make`

## How to Run

After building the project, you'll get two execcutables named `card_game` & `pofferSever` .
