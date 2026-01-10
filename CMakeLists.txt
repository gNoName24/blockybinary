cmake_minimum_required(VERSION 3.24)
project(blockybinary VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

function(project_message msg_type msg_text)
    message(${msg_type} "[blockybinary] ${msg_text}")
endfunction()

project_message(STATUS "CMAKE_PROJECT_VERSION: ${CMAKE_PROJECT_VERSION}")
project_message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

# xxHash (https://github.com/Cyan4973/xxHash)
project_message(STATUS "xxHash - https://github.com/Cyan4973/xxHash")
FetchContent_Declare(
    xxhash
    GIT_REPOSITORY https://github.com/Cyan4973/xxHash.git
    GIT_TAG        v0.8.3
)
FetchContent_MakeAvailable(xxhash)

project_message(STATUS "Collecting all .cpp files into blockybinary_sources")
file(GLOB_RECURSE blockybinary_sources "src/noname24/blockybinary/*.cpp")

add_library(blockybinary
    src/noname24/blockybinary.cpp
    ${blockybinary_sources}
)
target_include_directories(blockybinary PRIVATE
    include
)
target_link_libraries(blockybinary PRIVATE
    xxhash
)

# Тесты
enable_testing()

add_executable(blockybinary_test tests/main.cpp)
target_include_directories(blockybinary_test PRIVATE include)
target_link_libraries(blockybinary_test PRIVATE blockybinary)

add_test(NAME blockybinarytests COMMAND blockybinary_test)
