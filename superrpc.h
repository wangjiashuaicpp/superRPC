#ifndef SUPERRPC_H
#define SUPERRPC_H
#include <string>
#include "rpcobject.h"
namespace superrpc
{

    bool InitServer(std::string serverInfo);
    bool InitClient(std::string serverInfo,std::string clientID);
    bool RegisterObject(RPCObject *pObject);
    bool SendFuncCall(NetFunc *pFunc);
    bool SendFuncReturn(NetFunc *pFunc);
};

#endif