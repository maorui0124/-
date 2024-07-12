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
target_link_libraries(schedule_manager PRIVATE OpenSSL::SSL OpenSSL::Crypto)

find_package(nlohmann_json 3.2.0 REQUIRED)
target_link_libraries(schedule_manager PRIVATE nlohmann_json::nlohmann_json)