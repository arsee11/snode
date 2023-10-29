cmake_minimum_required(VERSION 3.5)


#set(CMAKE_SYSTEM_NAME Linux)
#set(TOOLCHAIN_PATH /home/arsee/tools/gcc-arm-10.2-2020.11-x86_64-aarch64-none-linux-gnu)
#set(CMAKE_C_COMPILER ${TOOLCHAIN_PATH}/bin/aarch64-none-linux-gnu-gcc)
#set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/bin/aarch64-none-linux-gnu-g++)
#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib_aarch64)

set(EXECUTABLE_OUTPUT_PATH ./)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS " -g ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS " -Wl,-rpath ./ ${CMAKE_CXX_FLAGS}")

include_directories(../src)
include_directories(../../)

link_directories(../lib)

set(CMAKE_CXX_FLAGS " -g -fsanitize=address ${CMAKE_CXX_FLAGS}")

project(snode LANGUAGES CXX)

add_executable(${PROJECT_NAME} snode_main.cpp)
target_link_libraries(${PROJECT_NAME} 
    snode-sn
    snode-core
    snode-cmd
    boost_json
    pthread
)

project(enode LANGUAGES CXX)

add_executable(${PROJECT_NAME} enode_main.cpp)
target_link_libraries(${PROJECT_NAME} 
    snode-en
    snode-core
    snode-cmd
    boost_json
    pthread
)
