cmake_minimum_required(VERSION 3.5)

project(snode-core LANGUAGES CXX)

set(SRCS 
sn_address_manager.cpp
sn_port.cpp 
sn_transport_manager.cpp
sn_message.cpp
sn_route_table.cpp
sn_udptransport.cpp
)

add_library(${PROJECT_NAME} STATIC ${SRCS})


