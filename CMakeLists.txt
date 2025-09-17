cmake_minimum_required(VERSION 3.10)
project(openu_maman14_assmbler C)

set(CMAKE_C_STANDARD 90)  # Use C99 (change to 11 or 17 if needed)
set(GCC_COVERAGE_COMPILE_FLAGS "−Wall −ansi −pedantic −g")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
# Add all source files
add_executable( openu_project
        main.c
        PreAssembler.c
        Utils.c
        FirstScan.c
        SecondScan.c
)
