#include "superrpc.h"
#include "netclient.h"
#include "rpcobjectmanager.h"
#include "thread"
namespace superrpc
{
    void OnNetData(const ZMQPack *pData,int size);
    void OnNetClientData(const ZMQPack *pData,int size);

    std::shared_ptr<NetServer> g_netServer = nullptr;
    std::shared_ptr<NetClient> g_netClient = nullptr;
    std::atomic_bool g_bServer;

    bool InitClient(std::string serverInfo,std::string clientID)
    {
        ObjectManager::getClientInstance()->setBServer(false);
        auto pClient = std::make_shared<NetClient>();
        if(!pClient->init(serverInfo,clientID)){
            return false;
        }
        ObjectManager::getClientInstance()->setNet(pClient);
        g_netClient = pClient;

        pClient->setFunCall([](const ZMQPack *pData,int size){
            OnNetClientData(pData,size);
        });        
        pClient->runClient();

        
    }

    bool InitServer(std::string serverInfo)
    {
        auto pServer = std::make_shared<NetServer>();
        if(!pServer->init(serverInfo)){
            return false;
        }
        ObjectManager::getInstance()->setNet(pServer);
        g_netServer = pServer;
        g_bServer = true;
        ObjectManager::getInstance()->setBServer(true);

        pServer->setFunCall([](const ZMQPack *pData,int size){
            OnNetData(pData,size);
        });

        pServer->runServer();
        
        return true;
    }


    void OnNetClientData(const ZMQPack *pData,int size)
    {
        std::cout << "OnNetClientData" << std::endl;
        if(pData->header == MSG_REGISTEROBJECT){
            std::string str(pData->pData,pData->dataSize);
            std::stringbuf buf(str);
		    std::istream out(&buf);
            RPCObject object;
            out >> object;
            ObjectManager::getClientInstance()->createClientObject(&object);
            
            
        }
        else if(pData->header == MSG_CALLFUNC){
            std::string str(pData->pData,pData->dataSize);
            std::stringbuf buf(str);
		    std::istream out(&buf);
            NetFunc func;
            out >> func;
            ObjectManager::getClientInstance()->onNetFunc(&func);
                 
        }
        else if(pData->header == MSG_FUNCRETURN){
            std::string str(pData->pData,pData->dataSize);
            std::stringbuf buf(str);
		    std::istream out(&buf);
            NetFunc func;
            out >> func;
            ObjectManager::getClientInstance()->onNetReturn(&func);
                      
        }
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
        else if(pData->header == MSG_CALLFUNC){
            std::string str(pData->pData,pData->dataSize);
            std::stringbuf buf(str);
		    std::istream out(&buf);
            NetFunc func;
            out >> func;
            ObjectManager::getInstance()->onNetFunc(&func);
        
        }
        else if(pData->header == MSG_FUNCRETURN){
            std::string str(pData->pData,pData->dataSize);
            std::stringbuf buf(str);
		    std::istream out(&buf);
            NetFunc func;
            out >> func;
            ObjectManager::getInstance()->onNetReturn(&func);
           
        }
    }

    // bool RegisterNetObject(RPCObject *pObject)
    // {
    //     std::stringbuf buf;
    //     std::ostream out(&buf);
    //     out << (*pObject);
    //     std::string sendStr = buf.str();
    //     if(g_netServer){
    //         g_netServer->sendData((int)MSG_REGISTEROBJECT,sendStr.c_str(),(int)sendStr.size(),pObject->m_clientID);
    //     }
    //     else{
    //         g_netClient->sendData((int)MSG_REGISTEROBJECT,sendStr.c_str(),(int)sendStr.size());
    //     }
    // }

    // bool SendFuncCall(NetFunc *pFunc)
    // {
    //     std::stringbuf buf;
    //     std::ostream out(&buf);
    //     out << (*pFunc);
    //     std::string sendStr = buf.str();        
    //     if(g_netServer){
    //         g_netServer->sendData((int)MSG_CALLFUNC,sendStr.c_str(),(int)sendStr.size(),pFunc->clientID);
    //     }
    //     else{
    //         g_netClient->sendData((int)MSG_CALLFUNC,sendStr.c_str(),(int)sendStr.size());
    //     }
    // }

    // bool SendFuncReturn(NetFunc *pFunc)
    // {
    //     std::stringbuf buf;
    //     std::ostream out(&buf);
    //     out << (*pFunc);
    //     std::string sendStr = buf.str();        
    //     if(g_netServer){
    //         g_netServer->sendData((int)MSG_FUNCRETURN,sendStr.c_str(),(int)sendStr.size(),pFunc->clientID);
    //     }
    //     else{
    //         g_netClient->sendData((int)MSG_FUNCRETURN,sendStr.c_str(),(int)sendStr.size());
    //     }        
    // }
};