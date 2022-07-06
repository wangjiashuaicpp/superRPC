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
        std::string strName;
        char *pData;
        std::vector<char> vecData;
        int dataSize;
        friend std::ostream & operator<<( std::ostream & os,const NetFunc & c);
	    //friend std::istream & operator>>( std::istream & is,NetFunc & c);
    };
    inline std::ostream & operator<<( std::ostream & os,const NetFunc & c)
    {
        os << c.index;
        os << c.strName;
        //os << c.vecData;
        return os;
    }    
    typedef std::function<void(NetFunc *pData)> NETFUNC;

    class RPCObject
    {
    public:
        RPCObject(/* args */);
        ~RPCObject();

        std::int64_t getNewFuncIndex();
        void sendData(const char* name, CALLFUNC func,char *pArg);
        void sendData(const char* name, CALLFUNC func,std::vector<char> arg);
        void sendReturnData(std::int64_t index,char *pArg);
        void setObjectID(std::int64_t objectID){m_objectID = objectID;};
        void setClientID(std::string& str){m_clientID = str;};
        std::string getClientID(){return m_clientID;}
        std::int64_t m_funcindex;
        std::int64_t m_objectID;
        std::string m_clientID;
        std::string m_className;
        bool m_bNetObject;
        std::map<std::int64_t,CALLFUNC> m_mapReturnFunc;
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

        virtual std::future<char*> getTest(char *pArg){};
    };
    

    class superrpcObjectTest : public ObjectTest
    {
    public:
        superrpcObjectTest(/* args */){};
        ~superrpcObjectTest(){};

        virtual std::future<char*> getTest(char *pArg){
            if (m_bNetObject) {
                auto ff = std::make_shared<std::promise<char*>>();
                auto Recdata = [this, ff](const char* data)
                {
                    ff->set_value(decltype(ff->get_future().get())(data));
                };
                this->sendData(__func__,Recdata,pArg);
                return ff->get_future();
            }
            else {
                return ObjectTest::getTest(pArg);
            }
        };
        void initgetTest()
        {
            auto netFunc = [this](NetFunc *pArg) {
                
                std::future<char*> ff =  ObjectTest::getTest(pArg->pData);
                auto r = ff.get();
                sendReturnData(pArg->index,r);

            };
            m_mapNetfunc["getTest"] = netFunc;
        }        
    };
    
};
#endif