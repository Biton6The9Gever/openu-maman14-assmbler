cmake_minimum_required(VERSION 3.10)
project(openu_maman14_assmbler C)

set(CMAKE_C_STANDARD 99)  # Use C99 (change to 11 or 17 if needed)

# Add all source files
add_executable( openu_project
        main.c
        PreAssembler.c
        Utils.c
)
