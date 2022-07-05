#ifndef RPCOBJECTMANAGER_H
#define RPCOBJECTMANAGER_H

#include "rpcobject.h"
namespace superrpc
{
    class ObjectManager
    {
    private:
        
    public:
        ObjectManager(/* args */);
        ~ObjectManager();

        void registerObject(PTR_RPCObject pObject);
        std::unordered_map<std::int64_t,PTR_RPCObject> m_mapObject;

        std::atomic<std::int64_t> m_objectIndex;
    };
};
#endif