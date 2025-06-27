# حداقل نسخه مورد نیاز CMake
cmake_minimum_required(VERSION 3.16)

# نام پروژه و زبان برنامه‌نویسی
project(client-src LANGUAGES CXX)

# تنظیم استاندارد C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# فعال‌سازی ابزارهای خودکار Qt برای کامپایل سیگنال/اسلات، منابع و فایل‌های UI
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# پیدا کردن پکیج‌ها و ماژول‌های مورد نیاز Qt6
# با توجه به وجود ServerHandler، ماژول Network هم اضافه شده است
find_package(Qt6 COMPONENTS Core Gui Qml Quick Network REQUIRED)

# لیست کردن فایل‌های سورس C++
set(PROJECT_SOURCES
    main.cpp
    Card.cpp
    Match.cpp
    Player.cpp
    Round.cpp
    ServerHandler.cpp
)

# لیست کردن فایل‌های هدر C++
# (این کار به نمایش بهتر فایل‌ها در برخی IDEها کمک می‌کند)
set(PROJECT_HEADERS
    Card.h
    Match.h
    Player.h
    Round.h
    ServerHandler.h
)

# برای مدیریت فایل‌های QML، بهترین راه استفاده از فایل منابع Qt (.qrc) است.
# یک فایل به نام qml.qrc بسازید و به لیست منابع اضافه کنید.
set(PROJECT_RESOURCES
    qml.qrc
)

# ساخت فایل اجرایی نهایی
add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
    ${PROJECT_RESOURCES}
)

# لینک کردن کتابخانه‌های Qt به پروژه
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
    Qt::Network
)
