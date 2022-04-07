cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

message(STATUS "C++ standard set to ${CMAKE_CXX_STANDARD}")

project(Hephaestus CXX)

add_compile_definitions(CMAKE_PROJECT_NAME="${PROJECT_NAME}")

add_compile_definitions(_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    message(STATUS "Compiler is Clang")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    message(STATUS "Compiler is GCC")
    add_compile_options(-Wno-volatile)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    message(STATUS "Compiler is Intel C++")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    message(STATUS "Compiler is Visual Studio C++")
    add_compile_options(/MP /utf-8)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS NOMINMAX)
endif ()

add_executable(${PROJECT_NAME} main.cc)

find_package(cryptopp CONFIG REQUIRED)
find_package(Drogon CONFIG REQUIRED)
find_package(hidapi CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)

target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        cryptopp-static
        Drogon::Drogon
        hidapi::hidapi
        hidapi::include
        magic_enum::magic_enum
)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_link_libraries(
            ${PROJECT_NAME}
            PRIVATE
            hidapi::hidraw
            hidapi::libusb
    )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_link_libraries(
            ${PROJECT_NAME}
            PRIVATE
            hidapi::winapi
    )
endif ()

aux_source_directory(controllers CTL_SRC)
aux_source_directory(filters FILTER_SRC)
aux_source_directory(helpers HELPER_SRC)
aux_source_directory(plugins PLUGIN_SRC)
aux_source_directory(strategies STRATEGY_SRC)
aux_source_directory(structures STRUCT_SRC)
aux_source_directory(types TYPE_SRC)
aux_source_directory(utils UTIL_SRC)

target_include_directories(${PROJECT_NAME}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/models)

target_sources(
        ${PROJECT_NAME}
        PRIVATE
        ${CTL_SRC}
        ${FILTER_SRC}
        ${PLUGIN_SRC}
        ${STRATEGY_SRC}
        ${STRUCT_SRC}
        ${TYPE_SRC}
        ${UTIL_SRC}
        ${HELPER_SRC}
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/config.json DESTINATION ${CMAKE_BINARY_DIR})
