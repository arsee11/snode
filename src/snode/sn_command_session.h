//sn_command_session.h


#ifndef SN_COMMAND_SESSION_H 
#define SN_COMMAND_SESSION_H

#include <map>
#include "sn_snode_impl.h"
#include "cmd/snode_cmds.h"
#include "core/sn_transport.h"

namespace snode {

struct Transaction
{
    Transaction(const std::string& id_in):id(id_in){}
    virtual ~Transaction(){};

    virtual void commit()=0;
    virtual void rollback()=0;

    std::string id;
};

using transaction_ptr=std::shared_ptr<Transaction>;

class AddressManager;
class TransportManager;
class NeighborManager;

class CommandSession
{
public:
    CommandSession(
        Snode *snoder,
        NeighborManager* nm,
        TransportManager* transportmgr,
        const TransEndpoint& ep
    );

    void sayHello(const TransEndpoint& to, const HelloCmd::Hello& h);

protected:
    std::unique_ptr<TransportServer> _cmd_transport_server;
    std::list<trans_ptr> _cmd_transports;

    void onNewConnect(const trans_ptr &conn);
    void onRecvCmd(const void* data, int size, trans_ptr t);

private:
    cmd_ptr onRegister(Command* req);
    cmd_ptr onAddressConfirm(Command* req);
    cmd_ptr onHello(Command* req);
    cmd_ptr onCmdOK(Command* req);

    std::map<std::string, transaction_ptr> _transaction_map;
    CommandParser _cmd_parser;
    Snode* _snode;
    NeighborManager* _neighbors_m;
    AddressManager* _addrmgr;
    TransportManager* _transportmgr;
};

}//namespace snode

#endif /*SN_COMMAND_SESSION_H*/
