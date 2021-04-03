///sn_message.cpp

#include "sn_message.h"
#include <string.h>
#include <arpa/inet.h>

namespace snode {

Message::Message(const Address &src, const Address &dst, const void *payload, size_t size)
{
    _header.src = src;
    _header.dst = dst;
    _header.length = header_size() + size;
    _payload = new uint8_t[size];
    memcpy(_payload, payload, size);
}

void Message::decode(const uint8_t *raw, size_t size)
{

    if( *((uint32_t*)raw) == _header.start_flag)
    {
        raw += sizeof(_header.start_flag);
        _header.length = *((uint32_t*)raw);
        _header.length = ntohl(_header.length);
        raw += sizeof(_header.length);
        _header.src = Address(*(Address::raw_t*)raw, true);
        raw += _header.src.size();
        _header.dst = Address(*(Address::raw_t*)raw, true);
        raw += _header.src.size();
        _payload = new uint8_t[size-this->header_size()];
        memcpy(_payload, raw, size-this->header_size());
    }
}

std::tuple<uint8_t *, size_t> Message::encode() const
{
    uint8_t* s = new uint8_t[this->size()];
    uint8_t* t=s;

    memcpy(t, &_header.start_flag, sizeof(_header.start_flag));
    t += sizeof(_header.start_flag);

    *((uint32_t*)t) = htonl(_header.length);
    t += sizeof(_header.start_flag);

    *((Address::raw_t*)t) = _header.src.rawton();
    t += _header.src.size();
    
    *((Address::raw_t*)t) = _header.dst.rawton();
    t += _header.dst.size();

    memcpy(t, _payload, this->payload_size());

    return std::make_tuple(s, this->size());
}

}//namespace snode
