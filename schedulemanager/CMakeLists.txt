cmake_minimum_required(VERSION 3.10)

project(ScheduleManager)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

add_executable(schedule_manager
    src/main.cpp
    src/AccountManager.cpp
    src/Task.cpp
    src/TaskManager.cpp
    src/Util.cpp
)

find_package(OpenSSL REQUIRED)
target_link_libraries(schedule_manager OpenSSL::SSL OpenSSL::Crypto)
