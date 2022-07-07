#ifndef RPCOBJECT_H
#define RPCOBJECT_H
#include <functional>
#include <map>
#include <vector>
#include <future>
#include <string.h>
#include <string.h>
#include <iostream>
#include <streambuf>
#include <sstream>
#include <fstream>
#include <unordered_map>
namespace superrpc
{
    typedef std::function<void(const char* data)> CALLFUNC;
    
    struct NetFunc
    {
        std::int64_t index;
        std::int64_t objectID;
        std::string strName;
        std::string clientID;
        std::string data;
        int dataSize;
        friend std::ostream & operator<<( std::ostream & os,const NetFunc & c);
	    friend std::istream & operator>>( std::istream & is,NetFunc & c);
    };
    inline std::ostream & operator<<( std::ostream & os,const NetFunc & c)
    {
        os << c.objectID;
        os << c.index;
        os << c.strName;
        os << c.clientID;

        //std::string strData(c.pData,c.dataSize);
        os << c.data;
        os << c.dataSize;

        return os;
    }    
    inline std::istream & operator>>( std::istream & is,NetFunc & c)
    {
        is >> c.objectID;
        is >> c.index;
        is >> c.strName;
        is >> c.clientID;
        //std::string strData;
        is >> c.data;
        is >> c.dataSize;
    }
    typedef std::function<void(NetFunc *pData)> NETFUNC;

    class RPCObject
    {
    public:
        RPCObject(/* args */);
        ~RPCObject();

        std::int64_t getNewFuncIndex();
        void sendData(const char* name, NETFUNC func,std::string &arg);
        void sendData(const char* name, NETFUNC func,std::vector<char> arg);
        void onNetFunc(NetFunc *pFunc);
        void onNetReturn(NetFunc *pFunc);
        
        void sendReturnData(std::int64_t index,std::string &arg);
        void setObjectID(std::int64_t objectID){m_objectID = objectID;};
        void setClientID(std::string& str){m_clientID = str;};
        std::string getClientID(){return m_clientID;}
        std::int64_t m_funcindex;
        std::int64_t m_objectID;
        std::string m_clientID;
        std::string m_className;
        bool m_bNetObject;
        std::map<std::int64_t,NETFUNC> m_mapReturnFunc;
        std::map<std::string, NETFUNC> m_mapNetfunc;


        friend std::ostream & operator<<( std::ostream & os,const RPCObject & c);
	    friend std::istream & operator>>( std::istream & is,RPCObject & c);
    };
    typedef std::shared_ptr<RPCObject> PTR_RPCObject;
    std::ostream & operator<<( std::ostream & os,const RPCObject & c);
    std::istream & operator>>( std::istream & is,RPCObject & c);


    class ObjectTest : public RPCObject
    {
    public:
        ObjectTest (/* args */){};
        ~ObjectTest (){};

        virtual std::future<std::string> getTest(std::string &arg){};
    };
    
#define   SUPER_CLASS_BEGIN(className) class  superrpc##className{\
public:\
    using super = className;\
    superrpc##className(){}\
    virtual ~superrpc##className(){};\

#define SUPER_FUNC_STRING(func)\
    virtual std::future<std::string> func(std::string &arg)override {\
        if (m_bNetObject) {\
            auto ff = std::make_shared<std::promise<std::string>>();\
            auto Recdata = [this, ff](NetFunc *pData)\
            {\
                ff->set_value(decltype(ff->get_future().get())(pData->data));\
            };\
            this->sendData(__func__,Recdata,arg);\
            return ff->get_future();\
        }\
        else {\
            return super::getTest(arg);\
        }\
    }\

#define SUPER_CLASS_END\
}\

    class superrpcObjectTest : public ObjectTest
    {
    public:
        superrpcObjectTest(/* args */){};
        ~superrpcObjectTest(){};

        virtual std::future<std::string> getTest(std::string &arg){
            if (m_bNetObject) {
                auto ff = std::make_shared<std::promise<std::string>>();
                auto Recdata = [this, ff](NetFunc *pData)
                {
                    ff->set_value(decltype(ff->get_future().get())(pData->data));
                };
                this->sendData(__func__,Recdata,arg);
                return ff->get_future();
            }
            else {
                return ObjectTest::getTest(arg);
            }
        };
        void initgetTest()
        {
            auto netFunc = [this](NetFunc *pArg) {
                
                std::future<std::string> ff =  ObjectTest::getTest(pArg->data);
                auto r = ff.get();
                sendReturnData(pArg->index,r);

            };
            m_mapNetfunc["getTest"] = netFunc;
        }        
    };
    
};
#endif