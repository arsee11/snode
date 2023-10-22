cmake_minimum_required(VERSION 3.5)

project(snode_snode_test LANGUAGES CXX)

#set(CMAKE_SYSTEM_NAME Linux)
#set(TOOLCHAIN_PATH /home/arsee/tools/gcc-arm-10.2-2020.11-x86_64-aarch64-none-linux-gnu)
#set(CMAKE_C_COMPILER ${TOOLCHAIN_PATH}/bin/aarch64-none-linux-gnu-gcc)
#set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/bin/aarch64-none-linux-gnu-g++)
#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib_aarch64)

set(EXECUTABLE_OUTPUT_PATH ./)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS " -g ${CMAKE_CXX_FLAGS}")

include_directories(${PROJECT_SOURCE_DIR}/../../)
link_directories(${PROJECT_SOURCE_DIR}/../../../lib)

file(GLOB SRCS *.cpp)
add_executable(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} 
    snode-core
    boost_unit_test_framework
    boost_test_exec_monitor
)


