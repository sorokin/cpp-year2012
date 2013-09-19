cmake_minimum_required(VERSION 2.6) 
project(DERIVATIVE)

list(APPEND CMAKE_CXX_FLAGS "-std=c++11")

add_executable (derivative
    main.cpp
    make_unique.h
    parser.cpp
    parser.h
    tree.cpp
    tree.h) 
