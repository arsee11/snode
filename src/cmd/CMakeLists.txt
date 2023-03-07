
project(snode-cmd LANGUAGES CXX)

set(SRCS 
commands.cpp
)

add_library(${PROJECT_NAME} STATIC ${SRCS})


