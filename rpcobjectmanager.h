#ifndef RPCOBJECTMANAGER_H
#define RPCOBJECTMANAGER_H

#include "rpcobject.h"
namespace superrpc
{
    class Net;
    class ObjectManager
    {
    public:
        static ObjectManager* getInstance();
        static ObjectManager* getClientInstance();
    private:
        ObjectManager(/* args */);
    public:
        
        ~ObjectManager();

        void setBServer(bool b){m_bServer = b;}
        bool getBServer(){return m_bServer;}

        void registerObject(PTR_RPCObject pObject);
        void createClientObject(RPCObject* pObject);
        void onNetFunc(NetFunc *pFunc);
        void onNetReturn(NetFunc *pFunc);
        bool sendFuncReturn(NetFunc *pFunc);
        bool sendFuncCall(NetFunc *pFunc);
        bool registerNetObject(PTR_RPCObject pObject);
        void setNet(std::shared_ptr<Net> pNet){m_pNet = pNet;};
        std::unordered_map<std::int64_t,PTR_RPCObject> m_mapObject;

        std::atomic<std::int64_t> m_objectIndex;
        std::shared_ptr<Net> m_pNet;
        bool m_bServer;
    };
};
#endif