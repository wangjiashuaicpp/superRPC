#include "rpcobject.h"

namespace superrpc
{


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

    void RPCObject::sendReturnData(std::int64_t index,char *pArg)
    {

    }


      
};

