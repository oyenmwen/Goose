cmake_minimum_required(VERSION 3.17)
project(create_a_lisp C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(create_a_lisp
        mpc.c
        mpc.h
        parsing.c)
