cmake_minimum_required(VERSION 2.8)

project(AdventOfCode2022)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_executable(AdventOfCode2022
	AdventOfCode2022.cpp
	AdventOfCode2022.h
	Problem1.cpp
	Problem2.cpp
	Problem3.cpp
	Problem4.cpp
	_clang-format
)

