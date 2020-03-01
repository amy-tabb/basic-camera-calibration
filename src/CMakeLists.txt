cmake_minimum_required(VERSION 3.0)

project(basic-camera-cali)

# set the flags, may want to add/change debugging or optimization (-O3) flags as appropriate
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -std=c++11 ")

find_package (Eigen3 3.3 REQUIRED NO_MODULE)

# OpenCV required, can specify version.  OpenCV 4.0 not strictly necessary, see the README for more details.
find_package( OpenCV REQUIRED ) 


add_executable(basic-chessboard-cali
basic-camera-cali.cpp
Calibration2.cpp
Calibration2.hpp
DirectoryFunctions.hpp
DirectoryFunctions.cpp
Includes.hpp
)

target_link_libraries(basic-chessboard-cali Eigen3::Eigen ${OpenCV_LIBS})

set_property(TARGET basic-chessboard-cali PROPERTY INSTALL_RPATH_USE_LINK_PATH TRUE)

install(TARGETS basic-chessboard-cali DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
