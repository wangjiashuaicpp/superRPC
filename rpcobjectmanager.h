#ifndef RPCOBJECTMANAGER_H
#define RPCOBJECTMANAGER_H

#include "rpcobject.h"
namespace superrpc
{
    class ObjectManager
    {
    public:
        static ObjectManager* getInstance();
    private:
        ObjectManager(/* args */);
    public:
        
        ~ObjectManager();

        void setBServer(bool b){m_bServer = b;}

        void registerObject(PTR_RPCObject pObject);
        void createClientObject(RPCObject* pObject);
        void onNetFunc(NetFunc *pFunc);
        void onNetReturn(NetFunc *pFunc);
        std::unordered_map<std::int64_t,PTR_RPCObject> m_mapObject;

        std::atomic<std::int64_t> m_objectIndex;

        bool m_bServer;
    };
};
#endif