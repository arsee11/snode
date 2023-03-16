///commands.h
///

#ifndef COMMANDS_H
#define COMMANDS_H

#include <boost/json.hpp>
#include <memory>
#include <tuple>

class CommandEncoder
{
public:
   const char* buf()const { return stream.c_str(); }
   int size()const{ return stream.size(); }

protected:
    std::string stream;
};

class Command;
using cmd_ptr = std::unique_ptr<Command>;;

class CommandDecoder
{
public:
    virtual void decode(void* params)=0;
};

struct NullDecoder : public CommandDecoder{
        NullDecoder(Command* cmd){
        }

        void decode(void* params)override{
        }
};


struct Command
{
    static std::string genId();
    //push req cmd and return a rsp cmd
    using Dispatcher = std::function<cmd_ptr(Command* req)>;
    std::string id;//transaction id
    Dispatcher dispatcher;
    
    cmd_ptr dispatch(){ 
        if(dispatcher != nullptr){
            return dispatcher(this);
        }
        return nullptr;
    }

    virtual CommandEncoder* encoder()=0;
    virtual CommandDecoder* decoder()=0;
    virtual ~Command(){};

    
};

struct OKCmd : public Command
{
    struct Encoder : public CommandEncoder{
        Encoder(OKCmd* cmd){
           stream = "{\"method\":\"ok\",\"transaction_id\":\""+cmd->id+"\"}";
        }
    };

    CommandEncoder* encoder()override{
        return new Encoder(this);
    }

    CommandDecoder* decoder()override{
        return new NullDecoder(this);
    }
};

struct AddressConfigCmd : public Command
{
    struct Encoder : public CommandEncoder{
        Encoder(AddressConfigCmd* cmd){
           stream = "{\"method\":\"address_config\",\"transaction_id\":\""+cmd->id+"\",";
           stream+= "\"params\":{";
           stream+= "\"address\":"+std::to_string(cmd->address)+",";
           stream += "\"transport\":{";
           stream += "\"ip\":\""+cmd->ip+"\",";
           stream += "\"port\":"+std::to_string(cmd->port)+"}";
           stream+= "}}"; 
        }
    };

    struct Decoder : public CommandDecoder{
        Decoder(AddressConfigCmd* c)
            :cmd(c)
        {
        }

        void decode(void* params)override{
            auto& jo = *static_cast<boost::json::object*>(params);
            try{
                cmd->address = jo["address"].as_int64();
                auto tjo = jo["transport"].as_object();
                cmd->ip = std::string(tjo["ip"].as_string().c_str());
                cmd->port = tjo["port"].as_int64();
            }catch(std::invalid_argument& e){
                cmd->address = 0;
                cmd->port = 0;
            }
        }
        AddressConfigCmd* cmd;
    };

    CommandEncoder* encoder()override{
        return new Encoder(this);
    }

    CommandDecoder* decoder()override{
        return new Decoder(this);
    };

    uint64_t address;
    std::string ip;
    uint16_t port=0;
};


struct RegisterCmd : public Command
{
    struct Encoder : public CommandEncoder{
        Encoder(RegisterCmd* cmd){
           stream = "{\"method\":\"register\",\"transaction_id\":\""+cmd->id+"\"}";
        }

    };

    CommandEncoder* encoder()override{
        return new Encoder(this);
    }

    CommandDecoder* decoder()override{
        return new NullDecoder(this);
    }
};


struct AddressConfirmCmd : public Command
{
    struct Encoder : public CommandEncoder{
        Encoder(AddressConfirmCmd* cmd){
           stream = "{\"method\":\"address_confirm\",\"transaction_id\":\""+cmd->id+"\",";
           stream+= "\"params\":{";
           stream += "\"transport\":{";
           stream += "\"ip\":\""+cmd->ip+"\",";
           stream += "\"port\":"+std::to_string(cmd->port)+"}";
           stream += "}}"; 
        }

    };

    struct Decoder : public CommandDecoder{
        Decoder(AddressConfirmCmd* c)
            :cmd(c)
        {
        }

        void decode(void* params)override{
            auto& jo = *static_cast<boost::json::object*>(params);
            try{
                auto tjo = jo["transport"].as_object();
                cmd->ip = std::string(tjo["ip"].as_string().c_str());
                cmd->port = tjo["port"].as_int64();
            }catch(std::invalid_argument& e){
                cmd->port=0;
            }
        }
        AddressConfirmCmd* cmd;
    };

    CommandEncoder* encoder()override{
        return new Encoder(this);
    }

    CommandDecoder* decoder()override{
        return new Decoder(this);
    };

    std::string ip;
    uint16_t port;
};


template<class Cmd>
struct CmdDispatcher
{
    Command::Dispatcher dispatcher;
};

#endif /*COMMANDS_H*/
