cmake_minimum_required(VERSION 3.27)
project(targil1 C)

set(CMAKE_C_STANDARD 11)

add_executable(targil1 main.c
        linked_list.c
        linked_list.c
        cmake-build-debug/main.c
        tweets_generator.c
        markov_chain.h
        markov_chain.c
        ha.c)
