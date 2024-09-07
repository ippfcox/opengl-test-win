cmake_minimum_required(VERSION 3.13)

get_filename_component(DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${DIR_NAME})

aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}" SRC)
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/../common" SRC)
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/glad/src" SRC)

add_executable(${PROJECT_NAME} ${SRC})

set(CMAKE_PREFIX_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/spdlog/lib/cmake/spdlog"
    "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/Assimp/lib/cmake/assimp-5.4"
)

find_package(spdlog REQUIRED)
find_package(assimp REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../common"
    "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/glad/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/glfw/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/stb/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/glm/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/Assimp/include"
)

target_link_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/glfw/lib-vc2019"
)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_link_options(${PROJECT_NAME} PRIVATE "/NODEFAULTLIB:MSVCRT")
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC
    spdlog::spdlog
    assimp::assimp
    "glfw3"
    "opengl32"
    "legacy_stdio_definitions"
)