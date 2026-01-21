cmake_minimum_required(VERSION 3.24)
project(blockybinary VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# include(FetchContent)
find_package(PkgConfig REQUIRED)

option(NONAME24_BLOCKYBINARY_USE_TESTS "Enable tests" ON)
option(NONAME24_BLOCKYBINARY_USE_DEMOS "Enable demos" ON)

function(project_message msg_type msg_text)
    message(${msg_type} "[blockybinary] ${msg_text}")
endfunction()

project_message(STATUS "CMAKE_PROJECT_VERSION: ${CMAKE_PROJECT_VERSION}")
project_message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

# xxHash (https://github.com/Cyan4973/xxHash)
project_message(STATUS "xxHash - https://github.com/Cyan4973/xxHash - PkgConfig")
pkg_check_modules(libxxhash REQUIRED IMPORTED_TARGET libxxhash)

# Miniz (https://github.com/richgel999/miniz)
project_message(STATUS "Miniz - https://github.com/richgel999/miniz - PkgConfig")
pkg_check_modules(miniz REQUIRED IMPORTED_TARGET miniz)

# Miniz (https://github.com/richgel999/miniz)
project_message(STATUS "Crypto++ - https://github.com/weidai11/cryptopp - find_library + find_path")
find_library(CRYPTOPP_LIBRARY cryptopp REQUIRED)
find_path(CRYPTOPP_INCLUDE_DIR cryptopp/cryptlib.h REQUIRED)

add_library(CryptoPP::CryptoPP UNKNOWN IMPORTED)
set_target_properties(CryptoPP::CryptoPP PROPERTIES
    IMPORTED_LOCATION "${CRYPTOPP_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${CRYPTOPP_INCLUDE_DIR}"
)

project_message(STATUS "Collecting all .cpp files into blockybinary_sources")
file(GLOB_RECURSE blockybinary_sources "src/noname24/blockybinary/*.cpp")

add_library(blockybinary
    src/noname24/blockybinary.cpp
    ${blockybinary_sources}
)
target_include_directories(blockybinary PUBLIC
    include
)
target_link_libraries(blockybinary PRIVATE
    PkgConfig::libxxhash
    PkgConfig::miniz
    CryptoPP::CryptoPP
)

# Макросы

# Тесты
if(USE_TESTS)
    project_message(STATUS "Tests enable")
    enable_testing()

    add_executable(blockybinary_test tests/main.cpp)
    target_include_directories(blockybinary_test PRIVATE include)
    target_link_libraries(blockybinary_test PRIVATE blockybinary)

    add_test(NAME blockybinarytests COMMAND blockybinary_test)
endif()

# Примеры
if(USE_DEMOS)
    project_message(STATUS "Demos enable")
    add_subdirectory(demos/blockyarchiver)
endif()
