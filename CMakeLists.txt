cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_COMPILER "/usr/bin/g++" CACHE STRING "g++ compiler" FORCE)
set(CMAKE_C_COMPILER "/usr/bin/gcc" CACHE STRING "gcc compiler" FORCE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(PLOT_IMAGE LANGUAGES C CXX)

# find packages
find_package(Eigen3 REQUIRED)
find_package(OpenCV REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Boost 1.74.0 REQUIRED
             COMPONENTS date_time system)
set(BOOST_LIBRARIES
    boost_system boost_date_time)


file(GLOB EXEC_MAIN_SRCS *.cpp)

foreach(EXEC_MAIN_SRC ${EXEC_MAIN_SRCS})

    get_filename_component(EXEC_NAME ${EXEC_MAIN_SRC} NAME_WE)
    add_executable(${EXEC_NAME} ${EXEC_MAIN_SRC} ${COMMON_SRCS})
    target_include_directories(${EXEC_NAME} PRIVATE
        ${EIGEN3_INCLUDE_DIRS}
        ${OpenCV_INCLUDE_DIR}
        ${ZLIB_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/external/cvui
        )
    target_link_libraries(${EXEC_NAME}
        ${OpenCV_LIBRARIES}
        ${ZLIB_LIBRARIES}
        )

    target_compile_options(${EXEC_NAME} PRIVATE
            -g -Ofast -Wfatal-errors)

endforeach()
