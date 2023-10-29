
project(snode-cmd LANGUAGES CXX)

set(SRCS 
commands.cpp
snode_cmds.cpp
)

add_library(${PROJECT_NAME} STATIC ${SRCS})


