///sn_rip_message.h

#ifndef SN_RIP_MESSAGE_H
#define SN_RIP_MESSAGE_H

#include <cstdint>
#include <vector>

namespace snode {

struct RIPMessage{
    uint8_t command;
    uint8_t version;
    uint16_t length;
    struct field {
        uint16_t family;
        uint16_t reserved;
        uint64_t address;
        uint32_t metric;
    };
    std::vector<field> fields;
};

struct RequestRIPMsg : public RIPMessage
{

};

struct ResponseRIPMsg : public RIPMessage
{

};

///serialize a RIP message to bytes stream
///@return size of bytes stream in bytes
size_t rip_message_serialize(const RIPMessage* m, uint8_t* buf, size_t buf_size);
size_t rip_message_serialize(const RIPMessage& m, uint8_t* buf, size_t buf_size);

///parse a bytes stream to RIP message
///throw a exception if any error occurs.
void rip_message_parse(RIPMessage* m, const uint8_t* buf, size_t buf_size);
void rip_message_parse(RIPMessage& m, const uint8_t* buf, size_t buf_size);

}//namespace snode


#endif /*SN_RIP_MESSAGE_H*/
