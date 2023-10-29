
project(snode-en LANGUAGES CXX)

set(SRCS 
sn_command_session.cpp
sn_enode.cpp
sn_enode_impl.cpp
)

add_library(${PROJECT_NAME} STATIC ${SRCS})


