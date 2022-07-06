#include "rpcobjectmanager.h"
namespace superrpc
{

    
    ObjectManager* ObjectManager::getInstance()
    {
        static ObjectManager g_objetManager;
        return &g_objetManager;
    }

    ObjectManager::ObjectManager(/* args */)
    {
        m_objectIndex = 1;
    }
    
    ObjectManager::~ObjectManager()
    {
    }

    void ObjectManager::registerObject(PTR_RPCObject pObject)
    {
        for(auto& iter : m_mapObject){
            if(iter.second == pObject){
                return;
            }
        }


        m_objectIndex ++;
        m_mapObject[m_objectIndex] = pObject;

        
    }

    void ObjectManager::createClientObject(RPCObject* pObject)
    {
        
    }
}