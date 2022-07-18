#ifndef SUPERRPC_H
#define SUPERRPC_H
#include <string>
#include "rpcobject.h"
#include "rpcobjectmanager.h"
namespace superrpc
{

    enum MSGTYPE
    {
        MSG_REGISTEROBJECT = 2,
        MSG_CALLFUNC,
        MSG_FUNCRETURN,
    };

    bool InitServer(std::string serverInfo);
    bool InitClient(std::string serverInfo,std::string clientID);
    //bool RegisterNetObject(RPCObject *pObject);
    //bool SendFuncCall(NetFunc *pFunc);
    //bool SendFuncReturn(NetFunc *pFunc);

    template <typename T>
    std::shared_ptr<T> CreateRPCObject(std::string strClientID)
    {
        auto pOBject = std::make_shared<T>();
        InitRPCObject(pOBject.get(),strClientID);
        superrpc::ObjectManager::getInstance()->registerNetObject(pOBject);
        return pOBject;
    };
    template <typename T>
    std::shared_ptr<T> CreateServerRPCObject(std::string strClientID)
    {
        auto pOBject = std::make_shared<T>();
        InitRPCObject(pOBject.get(),strClientID);
        superrpc::ObjectManager::getInstance()->registerNetObject(pOBject);
        return pOBject;
    }; 
    template <typename T>
    std::shared_ptr<T> CreateClientRPCObject(std::string strClientID)
    {
        auto pOBject = std::make_shared<T>();
        InitRPCObject(pOBject.get(),strClientID);
        superrpc::ObjectManager::getClientInstance()->registerNetObject(pOBject);
        return pOBject;
    };     
};

#endif