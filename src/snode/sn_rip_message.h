///sn_rip_message.h

#ifndef SN_RIP_MESSAGE_H
#define SN_RIP_MESSAGE_H

#include <cstdint>
#include <cstddef>
#include <vector>

namespace snode {

template<typename T, unsigned int N>
T read_bytes(const uint8_t* data) {
    static_assert(N <= sizeof(T), "too many bytes to read");

    T val(0);
    for (unsigned int i = 0; i < N; ++i)
    val |= static_cast<T>(data[i]) << ((N - 1 - i) * 8);
    return val;
}

template<typename T, unsigned int N>
void write_bytes(uint8_t* data, T val) {
    static_assert(N <= sizeof(T), "too many bytes to write");
        for (unsigned int i = 0; i < N; ++i) {
        data[i] = val >> ((N - 1 - i) * 8);
    }
    
}

struct RIPMessage{    
    enum{
        RequestCommand = 1,
        ResponseCommand,
        InvalidCommand = 255
    };

    uint8_t command=InvalidCommand;
    uint8_t version=0;
    uint8_t nfields=0;
    struct field{
        uint16_t family;
        uint64_t address;
        uint16_t metric;
    };
    static constexpr int field_size = 12;

    std::vector<field> fields;    
    size_t size()const{
        return header_size() + nfields * field_size;
    }

    static size_t header_size(){
        return 3;
    }

    ///@brief parse a bytes stream to RIP message
    ///@return A parsed message
    static RIPMessage parse(const uint8_t* buf, size_t size);

    ///@brief serialize a RIP message to bytes stream
    ///@return size of bytes stream in bytes
    static size_t serialize(const RIPMessage& msg, uint8_t* buf, size_t buf_size);

};



struct RequestRIPMsg : public RIPMessage
{

};

struct ResponseRIPMsg : public RIPMessage
{

};

}//namespace snode


#endif /*SN_RIP_MESSAGE_H*/
