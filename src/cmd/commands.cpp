///commands.h
///


#include "commands.h"


std::string Command::genId()
{
    //ToDo use uuid

    static int c=0;
    return std::to_string(c++);
}
