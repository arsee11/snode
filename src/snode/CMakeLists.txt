
project(snode-sn LANGUAGES CXX)

set(SRCS 
sn_command_session.cpp
sn_snode.cpp
sn_snode_impl.cpp
sn_neighbor.cpp
sn_rip_routing.cpp
sn_rip_message.cpp
sn_router_impl.cpp

)

add_library(${PROJECT_NAME} STATIC ${SRCS})


