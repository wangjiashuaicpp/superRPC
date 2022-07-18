#include "rpcobject.h"
#include "superrpc.h"
#include "rpcobjectmanager.h"
namespace superrpc
{

    std::ostream & operator<<( std::ostream & os,const RPCObject & c)
    {
        os << c.m_objectID << std::endl;
        os << c.m_clientID << std::endl;
        os << c.m_className << std::endl;
        return os;
    }

    std::istream & operator>>( std::istream & is,RPCObject & c)
    {
        is >> c.m_objectID;
        is >> c.m_clientID;
        is >> c.m_className;
        return is;
    }

    RPCObject::RPCObject(/* args */)
    {
        m_bNetObject = false;
        m_funcindex = 0;
        m_objectID = 0;


    }

    RPCObject::~RPCObject()
    {
    }

    void RPCObject::init()
    {
        for(auto &iter : m_arrInit){
            iter();
        }
    }


    void RPCObject::onNetFunc(NetFunc *pFunc)
    {
        auto func = m_mapNetfunc.find(pFunc->strName);
        if(func == m_mapNetfunc.end()){
            return;
        }
        func->second(pFunc);
    }

    void RPCObject::onNetReturn(NetFunc *pFunc)
    {
        auto func = m_mapReturnFunc.find(pFunc->index);
        if(func == m_mapReturnFunc.end()){
            return;
        }
        func->second(pFunc);
        m_mapReturnFunc.erase(func);
    }

    std::int64_t RPCObject::getNewFuncIndex()
    {
        m_funcindex++;
        return m_funcindex;
    }
    void RPCObject::sendData(const char* name, NETFUNC func,std::string &arg)
    {
        NetFunc funcinfo;
        funcinfo.objectID = m_objectID;
        funcinfo.index = getNewFuncIndex();
        funcinfo.strName = name;
        funcinfo.clientID = m_clientID;
        funcinfo.data = arg;

        m_mapReturnFunc[funcinfo.index] = func;
        //SendFuncCall(&funcinfo);
        m_pManager->sendFuncCall(&funcinfo);
    }

    void RPCObject::sendData(const char* name, NETFUNC func,std::vector<char> arg)
    {

    }

    void RPCObject::sendReturnData(std::int64_t index,std::string &arg)
    {
        NetFunc funcinfo;
        funcinfo.objectID = m_objectID;
        funcinfo.index = index;
        funcinfo.clientID = m_clientID;
        funcinfo.data = arg;
        funcinfo.dataSize = arg.size();
        //SendFuncReturn(&funcinfo);
        m_pManager->sendFuncReturn(&funcinfo);
    }

    static std::int64_t g_objectID = 0;
    RPCObject* InitRPCObject(RPCObject *pObject,std::string strClientID)
    {
        g_objectID ++;
        pObject->setClientID(strClientID);
        pObject->setObjectID(g_objectID);

        //bool bServer = ObjectManager::getInstance()->getBServer();
        //pObject->m_bNetObject = bServer;

        //RegisterNetObject(pObject);
        return pObject;
    }

    static std::shared_ptr<std::unordered_map<std::string,CREATEFUNC>> g_mapCreate = nullptr;
    void AddClassTemplate(std::string strClass,superrpc::CREATEFUNC func)
    {
        if(g_mapCreate == nullptr){
            g_mapCreate = std::make_shared<std::unordered_map<std::string,CREATEFUNC>>();
        }
        
        g_mapCreate->insert(std::make_pair(strClass,func));
    }

    PTR_RPCObject CreateRPCObjectByName(std::string className)
    {
        auto findIter = g_mapCreate->find(className);
        if(findIter != g_mapCreate->end()){
            return findIter->second();
        }

        return nullptr;
    }
};

