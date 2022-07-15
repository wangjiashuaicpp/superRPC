#include "rpcobjectmanager.h"
namespace superrpc
{

    
    ObjectManager* ObjectManager::getInstance()
    {
        static ObjectManager g_objetManager;
        return &g_objetManager;
    }
    ObjectManager* ObjectManager::getClientInstance()
    {
        static ObjectManager g_objetClientManager;
        return &g_objetClientManager;
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
        auto pClientObject = CreateRPCObjectByName(pObject->m_className);
        m_mapObject[pObject->m_objectID] = pClientObject;
    }

    void ObjectManager::onNetFunc(NetFunc *pFunc)
    {
        auto object = m_mapObject.find(pFunc->objectID);
        if(object == m_mapObject.end()){
            return;
        }
        object->second->onNetFunc(pFunc);
    }

    void ObjectManager::onNetReturn(NetFunc *pFunc)
    {
        auto object = m_mapObject.find(pFunc->objectID);
        if(object == m_mapObject.end()){
            return;
        }
        object->second->onNetReturn(pFunc);
    }
}