///sn_message.h

#ifndef SN_MESSAGE_H
#define SN_MESSAGE_H

#include <cstdint>
#include <tuple>
#include "sn_address.h"

namespace snode
{

class Message 
{
public:
    Message(){}

    ~Message(){
        if( _payload != nullptr)
            delete[] _payload;
    }

    Message(const Address& src, const Address& dst, const void *data, size_t size);

    const Address& src()const{ return _header.src; }
    const Address& dst()const{ return _header.dst; }
    uint8_t* payload()const{ return _payload; }
    size_t payload_size()const{ return this->size() - header_size(); }

    //total bytes of this message
    size_t size()const{ return _header.length; }

    //@decode from raw bytes stream
    void decode(const uint8_t *raw, size_t size);

    //@encode to raw bytes stream
    //@return bytes stream and size of stream
    std::tuple<uint8_t*, size_t> encode()const;


private:
    size_t header_size()const{ return sizeof(_header.start_flag)
                         +sizeof(_header.length)
                         +_header.src.size()
                         +_header.dst.size();}

private:
    struct Header{
        uint32_t start_flag=0xffffffff; //start flag 0xffffffff
        uint32_t length=0;              //total length of the message inlcude header and palyload
        Address src;                    //address of the sender
        Address dst;                    //address of the receiver
    };

	Header _header;	
    uint8_t* _payload=nullptr;
};

}

#endif /*SN_MESSAGE_H*/
