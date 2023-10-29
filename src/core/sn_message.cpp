///sn_message.cpp

#include "sn_message.h"
#include <string.h>
#include <arpa/inet.h>

namespace snode {

bool Message::is_link_update_message(const Message &msg)
{
    return msg.payload_type() == LinkUpdateMsg;
}

Message* Message::create_link_update_message(const Address &src, const Address &dst, const void *payload, size_t size)
{
    return new Message(src, dst, LinkUpdateMsg, payload, size);
}

Message::Message(const Address &src, const Address &dst, const void *payload, size_t size)
{
    _header.src = src;
    _header.dst = dst;
    _header.length = header_size() + size;
    _payload.reset(new uint8_t[size], [](auto p){delete[] p;});
    memcpy(_payload.get(), payload, size);
}

Message::Message(const Address& src, const Address& dst, uint8_t payload_type, const void *payload, size_t size)
{
    _header.src = src;
    _header.dst = dst;
    _header.length = header_size() + size;
    _header.payload_type = payload_type;
    _payload.reset(new uint8_t[size], [](auto p){delete[] p;});
    memcpy(_payload.get(), payload, size);
}

void Message::decode(const uint8_t *raw, size_t size)
{

    if( *((uint32_t*)raw) == _header.start_flag)
    {
        raw += sizeof(_header.start_flag);
        _header.length = *((uint32_t*)raw);
        _header.length = ntohl(_header.length);
        raw += sizeof(_header.length);

        _header.payload_type = *((uint8_t*)raw);
        raw += sizeof(_header.payload_type);

        _header.src = Address(*(Address::raw_t*)raw, true);
        raw += _header.src.size();

        _header.dst = Address(*(Address::raw_t*)raw, true);
        raw += _header.src.size();

        _payload.reset(new uint8_t[size-this->header_size()], [](auto p){delete[] p;});
        memcpy(_payload.get(), raw, size-this->header_size());
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

    *((uint8_t*)t) = _header.payload_type;
    t += sizeof(_header.payload_type);

    *((Address::raw_t*)t) = _header.src.rawton();
    t += _header.src.size();
    
    *((Address::raw_t*)t) = _header.dst.rawton();
    t += _header.dst.size();

    memcpy(t, _payload.get(), this->payload_size());

    return std::make_tuple(s, this->size());
}

}//namespace snode
