#ifndef COMMANDS_H
#define COMMANDS_H

#include "core/sn_transport_manager.h"
#include "core/sn_address_manager.h"
#include "core/sn_port.h"
#include <boost/json.hpp>
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

template<class ...CmdDispatchers>
class CommandParserT
{
public:
    CommandParserT(){
        _dispatchers = std::make_tuple(CmdDispatchers()...);
    }

    template<class Dispatcher>
    void setDispatcher(Dispatcher d){
        auto& h = std::get<Dispatcher>(_dispatchers);
        h= d;
    }

    cmd_ptr parse(const char* stream, int size){
        cmd_ptr cmd;
        boost::json::error_code ec;
        json_parser.reset();
        json_parser.write(stream, size, ec);
        if(ec){
            std::cout<<"cmd decode failed:"<<ec.message()<<std::endl;
            return nullptr;
        }
        auto jv = json_parser.release();
        auto jo = jv.as_object();
        boost::json::string method = jo["method"].as_string();
        if(method == "register"){
            cmd.reset(new RegisterCmd);
            auto h = std::get<CmdDispatcher<RegisterCmd>>(_dispatchers);
            cmd->dispatcher = h.dispatcher;
        }
        else if(method == "address_confirm"){
            cmd.reset(new AddressConfirmCmd);
            auto h = std::get<CmdDispatcher<AddressConfirmCmd>>(_dispatchers);
            cmd->dispatcher = h.dispatcher;
        }
        else if(method == "address_config"){
            cmd.reset(new AddressConfigCmd);
            auto h = std::get<CmdDispatcher<AddressConfigCmd>>(_dispatchers);
            cmd->dispatcher = h.dispatcher;
        }
        else if(method == "ok"){
            cmd.reset(new OKCmd);
            auto h = std::get<CmdDispatcher<OKCmd>>(_dispatchers);
            cmd->dispatcher = h.dispatcher;
        }
        else{
        }

        cmd->decoder()->decode(&jo["params"]);

        if(cmd != nullptr){
            cmd->id = std::string(jo["transaction_id"].as_string().c_str());
        }
        return std::move(cmd);
    }

private:
    boost::json::parser json_parser;
    std::tuple<CmdDispatchers...> _dispatchers;
};

using CommandParser = CommandParserT<
            CmdDispatcher<RegisterCmd>,
            CmdDispatcher<AddressConfirmCmd>,
            CmdDispatcher<AddressConfigCmd>,
            CmdDispatcher<OKCmd>
       >;
#endif /*COMMANDS_H*/
