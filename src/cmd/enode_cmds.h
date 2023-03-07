#ifndef ENODE_COMMANDS_H
#define ENODE_COMMANDS_H

#include "commands.h"
#include <boost/json.hpp>
#include <tuple>

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
        if(method == "address_config"){
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
            CmdDispatcher<AddressConfigCmd>,
            CmdDispatcher<OKCmd>
       >;
#endif /*ENODE_COMMANDS_H*/
