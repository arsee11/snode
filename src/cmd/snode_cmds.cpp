
#include "snode_cmds.h"



HelloCmd::Encoder::Encoder(HelloCmd* cmd)
{
    stream = "{\"method\":\"hello\",\"transaction_id\":\""+cmd->id+"\",";
    stream+= "\"params\":{";
    stream+= "\"address\":"+std::to_string(cmd->hello.address)+",";
    stream += "\"transport\":{";
    stream += "\"ip\":\""+cmd->hello.ip+"\",";
    stream += "\"port\":"+std::to_string(cmd->hello.port)+"}";
    stream+= "}}"; 
}



HelloCmd::Decoder::Decoder(HelloCmd* c)
    :cmd(c)
{
}

void HelloCmd::Decoder::decode(void* params){
    auto& jo = *static_cast<boost::json::object*>(params);
    try{
        cmd->hello.address = jo["address"].as_int64();
        auto tjo = jo["transport"].as_object();
        cmd->hello.ip = std::string(tjo["ip"].as_string().c_str());
        cmd->hello.port = tjo["port"].as_int64();
    }catch(std::invalid_argument& e){
        cmd->hello.address = 0;
        cmd->hello.port = 0;
    }
}

        
   