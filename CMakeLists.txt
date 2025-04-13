#CMake file for environment and some settings, links to src

cmake_minimum_required(VERSION 3.15...3.31)

#set(CMAKE_FIND_DEBUG_MODE TRUE)
set(OFNIR_VERSION 0.0.2) # MAJOR.MINOR.PATCH

project(Ofnir VERSION ${OFNIR_VERSION} LANGUAGES CXX)

# Set C++ version
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)


