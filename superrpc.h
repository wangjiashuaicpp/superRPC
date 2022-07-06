#ifndef SUPERRPC_H
#define SUPERRPC_H
#include <string>
#include "rpcobject.h"
namespace superrpc
{

    bool InitServer(std::string serverInfo);
    bool RegisterObject(RPCObject *pObject);
    
};

#endif