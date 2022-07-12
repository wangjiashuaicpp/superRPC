#include "rpcobject.h"
#include "superrpc.h"
namespace superrpc
{

    std::ostream & operator<<( std::ostream & os,const RPCObject & c)
    {
        os << c.m_objectID;
        os << c.m_clientID;
        os << c.m_className;
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
        SendFuncCall(&funcinfo);
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
        SendFuncReturn(&funcinfo);
    }

};

