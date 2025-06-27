cmake_minimum_required(VERSION 3.16)
project(card_game)


set(CMAKE_CXX_STANDARD 20)

set(CMAKE_PREFIX_PATH "/home/jeff/Qt/6.9.0/gcc_64")


find_package(Qt6 REQUIRED COMPONENTS Quick )
message(STATUS "Using Qt from: ${Qt6_DIR}")
qt_standard_project_setup(REQUIRES 6.8)
add_executable(card_game main.cpp
        Card.cpp
        Card.h)
qt_add_qml_module(card_game
        URI card-game
        VERSION 1.0
        QML_FILES
        Main.qml Card.qml MButton.qml MTextBox.qml
        QML_FILES MainMenu.qml
        QML_FILES Login.qml
)


target_link_libraries(card_game
        PRIVATE Qt6::Quick
)

include(GNUInstallDirs)
install(TARGETS card_game
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
