cmake_minimum_required(VERSION 3.13)

get_filename_component(DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${DIR_NAME})

add_subdirectory(triangle)