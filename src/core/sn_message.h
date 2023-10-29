///sn_message.h

#ifndef SN_MESSAGE_H
#define SN_MESSAGE_H

#include <cstdint>
#include <tuple>
#include "sn_address.h"
#include <memory>

namespace snode
{

class Message 
{
public:
    enum{
        UnknownMsg=-1,
        NormalMsg=0,
        LinkUpdateMsg
    };
    
public:
    static bool is_link_update_message(const Message& msg);
    static Message* create_link_update_message(const Address& src, const Address& dst, const void *payload, size_t size);

public:
    Message(){}
    
     //@note:will make a copy of the payload 
    Message(const Address& src, const Address& dst, const void *payload, size_t size);
    Message(const Address& src, const Address& dst, uint8_t payload_type, const void *payload, size_t size);


    const Address& src()const{ return _header.src; }
    const Address& dst()const{ return _header.dst; }
    uint8_t* payload()const{ return _payload.get(); }
    uint8_t payload_type()const{ return _header.payload_type; }

    size_t header_size()const{ return sizeof(_header.start_flag)
                         +sizeof(_header.length)
                         +sizeof(_header.payload_type)
                         +_header.src.size()
                         +_header.dst.size();}
                         
    size_t payload_size()const{ return this->size() - header_size(); }

    //total bytes of this message
    size_t size()const{ return _header.length; }

    //@decode from raw bytes stream
    void decode(const uint8_t *raw, size_t size);

    //@encode to raw bytes stream
    //@return bytes stream and size of stream
	//@note caller will own the raw stream pointer
    std::tuple<uint8_t*, size_t> encode()const;



private:
    struct Header{
        uint32_t start_flag=0xffffffff; //start flag 0xffffffff
        uint32_t length=0;              //total length of the message inlcude header and palyload
        uint8_t payload_type=NormalMsg;
        Address src;                    //address of the sender
        Address dst;                    //address of the receiver
    };

	Header _header;	
    std::shared_ptr<uint8_t> _payload;
};

}

#endif /*SN_MESSAGE_H*/
