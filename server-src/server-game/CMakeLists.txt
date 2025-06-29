cmake_minimum_required(VERSION 3.16)
project(pofferGame LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Step 1: Find all required Qt components in a single command.
# We need both Core and Network.
find_package(Qt6 REQUIRED COMPONENTS Core Network)

add_executable(pofferSever
  main.cpp
  server.h
  server.cpp
  useraccount.h useraccount.cpp
  useraccount.h useraccount.cpp
  gamehistory.h gamehistory.cpp
        player.h
        player.cpp
        game.h
        game.cpp
        a.h
)

# Step 2: Link ALL the components you found to your target.
target_link_libraries(pofferSever PRIVATE
    Qt6::Core
    Qt6::Network
)

include(GNUInstallDirs)
install(TARGETS pofferSever

    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
