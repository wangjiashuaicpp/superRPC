#include "superrpc.h"
#include "netclient.h"
#include "rpcobjectmanager.h"
#include "thread"
namespace superrpc
{
    void OnNetData(const ZMQPack *pData,int size);
    enum MSGTYPE
    {
        MSG_REGISTEROBJECT = 2,
        MSG_CALLFUNC,
    };
    std::shared_ptr<NetServer> g_netServer = nullptr;
    std::atomic_bool g_bServer;

    
    bool InitServer(std::string serverInfo)
    {
        auto pServer = std::make_shared<NetServer>();
        if(!pServer->init(serverInfo)){
            return false;
        }
        g_netServer = pServer;
        g_bServer = true;
        ObjectManager::getInstance()->setBServer(true);

        pServer->setFunCall([](const ZMQPack *pData,int size){
            OnNetData(pData,size);
        });
        return true;
    }

    void OnNetData(const ZMQPack *pData,int size)
    {
        if(pData->header == MSG_REGISTEROBJECT){
            std::string str(pData->pData,pData->dataSize);
            std::stringbuf buf(str);
		    std::istream out(&buf);
            RPCObject object;
            out >> object;
            
            ObjectManager::getInstance()->createClientObject(&object);
        }
    }

    bool RegisterObject(RPCObject *pObject)
    {
        if(g_netServer){
            std::stringbuf buf;
		    std::ostream out(&buf);
            out << (*pObject);
            std::string sendStr = buf.str();
            g_netServer->sendData((int)MSG_REGISTEROBJECT,sendStr.c_str(),(int)sendStr.size());
        }
    }
};