#ifndef SNODE_COMMANDS_H
#define SNODE_COMMANDS_H

#include "commands.h"
#include <boost/json.hpp>
#include "snode/sn_neighbor.h"
#include "core/sn_routing.h"
#include <tuple>

struct ShareRoutingCmd : public Command
{
    struct Encoder : public CommandEncoder{
        Encoder(ShareRoutingCmd* cmd){
        }

    };

    CommandEncoder* encoder()override{
        return new Encoder(this);
    }

    CommandDecoder* decoder()override{
        return new NullDecoder(this);
    }

    snode::Neighbor neighbor;
    snode::RoutingInfo info;
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
        else if(method == "share_routing"){
            cmd.reset(new ShareRoutingCmd);
            auto h = std::get<CmdDispatcher<ShareRoutingCmd>>(_dispatchers);
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
            CmdDispatcher<ShareRoutingCmd>,
            CmdDispatcher<OKCmd>
       >;
#endif /*SNODE_COMMANDS_H*/
