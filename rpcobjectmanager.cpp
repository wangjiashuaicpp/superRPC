#include "rpcobjectmanager.h"
#include "netclient.h"
#include "superrpc.h"
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

        pObject->setObjectManager(this);
        m_objectIndex ++;
        m_mapObject[m_objectIndex] = pObject;

        
    }

    void ObjectManager::createClientObject(RPCObject* pObject)
    {
        auto pClientObject = CreateRPCObjectByName(pObject->m_className);
        pClientObject->setObjectManager(this);
        pClientObject->setClientID(pObject->m_clientID);
        pClientObject->setObjectID(pObject->m_objectID);
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

    bool ObjectManager::sendFuncReturn(NetFunc *pFunc)
    {
        RPCStream out(pFunc->dataSize + 128);
        out << (*pFunc);
        std::string sendStr(out.vectorSize.data(),out.vectorSize.size());      
        m_pNet->sendData((int)MSG_FUNCRETURN,sendStr.c_str(),(int)sendStr.size(),pFunc->clientID);    
    }    

    bool ObjectManager::sendFuncCall(NetFunc *pFunc)
    {
        //std::stringbuf buf;
        RPCStream out(pFunc->dataSize + 128);
        out << (*pFunc);
        std::string sendStr(out.vectorSize.data(),out.vectorSize.size());        
        m_pNet->sendData((int)MSG_CALLFUNC,sendStr.c_str(),(int)sendStr.size(),pFunc->clientID);

    }

    bool ObjectManager::registerNetObject(PTR_RPCObject pObject)
    {
        pObject->m_bNetObject = true;
        pObject->setObjectManager(this);
        m_mapObject[pObject->m_objectID] = pObject;
        //std::stringbuf buf;
        RPCStream out(64);
        out << (*pObject);
        std::string sendStr(out.vectorSize.data(),out.vectorSize.size());
        m_pNet->sendData((int)MSG_REGISTEROBJECT,sendStr.c_str(),(int)sendStr.size(),pObject->m_clientID);

    }
}