cmake_minimum_required(VERSION 3.10)

project(database)

add_executable(main ./src/main.c ./src/functions.c)

target_include_directories(main PUBLIC ./include)
target_link_libraries(main PUBLIC ncurses z) #z это флаг для zlib 