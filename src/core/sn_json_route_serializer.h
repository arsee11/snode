///sn_json_route_serializer.h

#ifndef SN_JSON_SERIALIZER_H
#define SN_JSON_SERIALIZER_H

#include <string>

namespace snode
{

template<class Router>
class JsonRouteSerializer
{
public:
    JsonRouteSerializer(Router* t)
        :_router(t)
    {
        
    }

    std::string operator()(){
        std::string str="{\"items\":[";

        if(_router != nullptr){
            auto items = _router->route_table().getAllItems();
            for(size_t i=0; i<items.size(); i++){
                const auto& item = items[i];
                std::string dst = address_s(item.dst);
                std::string mask = std::to_string(item.mask);
                std::string m = std::to_string(item.metric);
                std::string next = address_s(item.next_hop);
                std::string lport = std::to_string(item.port->transport()->local_ep().port);
                std::string lip = item.port->transport()->local_ep().ip;
                std::string rport = std::to_string(item.port->transport()->remote_ep().port);
                std::string rip = item.port->transport()->remote_ep().ip;

                str+="{\"dst\":"+dst+",\"mask\":\""+mask+"\",\"metric\":\""+m+"\",";
                str+="\"next_hop\":"+next+",";
                str+= "\"port\":{\"lep\":{\"ip\":\""+lip+"\",\"port\":\""+lport+"\"},";
                str+=           "\"rep\":{\"ip\":\""+rip+"\",\"port\":\""+rport+"\"}}";
                if(i==items.size()-1){
                    str+="}";
                }
                else{
                    str+="},";
                }
            }
        }

        str +="]}";

        return str;
    }

private:
    std::string address_s(const Address& addr){
        std::string str = "{\"sn\":\""+std::to_string(addr.sn())+"\",";
                    str += "\"en\":\""+std::to_string(addr.en())+"\"}";

        return str;
    }

private:
    Router* _router=nullptr;
};

}//snode

#endif/*SN_JSON_SERIALIZER_H*/
