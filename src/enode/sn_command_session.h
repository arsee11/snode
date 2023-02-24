//sn_command_session.h


#ifndef SN_COMMAND_SESSION_H 
#define SN_COMMAND_SESSION_H

#include <map>
#include "sn_enode.h"
#include "cmd/enode_cmds.h"

namespace snode {

class AddressManager;
class TransportManager;

namespace enode {

struct Transaction
{
    Transaction(const std::string& id_in):id(id_in){}
    virtual ~Transaction(){};

    virtual void commit()=0;
    virtual void rollback()=0;

    std::string id;
};

using transaction_ptr=std::shared_ptr<Transaction>;



class CommandSession
{
public:
    CommandSession(ENode* en,
        TransportManager* transportmgr
    );

    bool open(const TransEndpoint &local_ep, const TransEndpoint &snode_ep);

protected:
    std::unique_ptr<Transport> _cmd_transport;
    void onRecvCmd(const void* data, int size);

private:
    cmd_ptr onConfigAddress(Command* req);
    cmd_ptr onCmdOK(Command* req);

    std::map<std::string, transaction_ptr> _transaction_map;
    CommandParser _cmd_parser;
    ENode* _enode;
    AddressManager* _addressmgr;
    TransportManager* _transportmgr;
};

}//namespace enode

}//namespace snode

#endif /*SN_COMMAND_SESSION_H*/
