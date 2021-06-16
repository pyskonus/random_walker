cmake_minimum_required(VERSION 3.19)
project(random_walker)

set(CMAKE_CXX_STANDARD 17)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra")

add_executable(random_walker main.cpp inc/rw_image.h src/rw_image.cpp inc/main_computations.h src/main_computations.cpp)
target_link_libraries(random_walker png Eigen3::Eigen)