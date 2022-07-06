#include "rpcobject.h"

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

    std::int64_t RPCObject::getNewFuncIndex()
    {
        m_funcindex++;
        return m_funcindex;
    }
    void RPCObject::sendData(const char* name, CALLFUNC func,char *pArg)
    {
        NetFunc funcinfo;
        funcinfo.index = getNewFuncIndex();
        funcinfo.strName = name;
        funcinfo.pData = pArg;

        m_mapReturnFunc[funcinfo.index] = func;
    }

    void RPCObject::sendData(const char* name, CALLFUNC func,std::vector<char> arg)
    {

    }
    void RPCObject::sendReturnData(std::int64_t index,char *pArg)
    {

    }


      
};

