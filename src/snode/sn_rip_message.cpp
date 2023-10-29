///sn_rip_message.cpp


#include "sn_rip_message.h"

namespace snode {

RIPMessage RIPMessage::parse(const uint8_t* buf, size_t size)
{
    RIPMessage msg;

    if(size < RIPMessage::header_size()){
        msg.command = RIPMessage::InvalidCommand;
        return msg;
    }

    msg.command = read_bytes<uint8_t, 1>(buf);
    buf += 1;
    msg.version = read_bytes<uint8_t, 1>(buf);
    buf += 1;
    msg.nfields = read_bytes<uint8_t, 1>(buf);
    buf += 1;
    if( size-RIPMessage::header_size() < msg.nfields*field_size){
            msg.command = RIPMessage::InvalidCommand;
        return msg;
    }
    for(int i=0; i<msg.nfields; i++){
        field f;
        f.family = read_bytes<uint16_t, 2>(buf);
        buf += 2;
        f.address = read_bytes<uint64_t, 8>(buf);
        buf += 8;
        f.metric = read_bytes<uint16_t, 2>(buf);
        buf += 2;
        msg.fields.push_back(f);
    }

    return msg;

}

///@brief serialize a RIP message to bytes stream
///@return size of bytes stream in bytes
size_t RIPMessage::serialize(const RIPMessage& msg, uint8_t* buf, size_t buf_size)
{
    if(buf_size < msg.size()){
        return 0;
    }

    write_bytes<uint8_t, 1>(buf, msg.command);
    buf += 1;
    write_bytes<uint8_t, 1>(buf, msg.version);
    buf += 1;
    write_bytes<uint8_t, 1>(buf, msg.nfields);
    buf += 1;
    for(int i=0; i<msg.nfields; i++){
        const field& f = msg.fields[i];
        write_bytes<uint16_t, 2>(buf, f.family);
        buf += 2;
        write_bytes<uint64_t, 8>(buf, f.address);
        buf += 8;
        write_bytes<uint16_t, 2>(buf, f.metric);
        buf += 2;
    }

    return msg.size();
}

}//namespace snode

