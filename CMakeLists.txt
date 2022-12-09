project(geefr-aoc-2022)

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
add_compile_options( -Wall -Wextra -Wpedantic -Werror -Wno-unused-variable -Wno-unused-parameter)

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s -Os -fno-stack-protector -ffunction-sections -fdata-sections -Wl,--gc-sections -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-math-errno -fno-unroll-loops -fmerge-all-constants -fno-ident -Wl,-z,norelro -Wl,--hash-style=gnu -Wl,--build-id=none")

add_executable(1 1/main.c)
add_executable(2 2/main.c)
add_executable(3 3/main.c)
add_executable(4 4/main.c)
add_executable(5 5/main.c)
add_executable(6 6/main.c)
add_executable(7 7/main.c)
add_executable(8 8/main.c)
add_executable(9 9/main.c)

install( TARGETS
	 1 2 3 4 5 6 7 8 9
	 DESTINATION .
	 )
