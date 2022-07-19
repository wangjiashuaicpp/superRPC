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

        std::int64_t toLong(){return std::atoll(data.c_str());}
        void    packLong(std::int64_t arg){data = std::to_string(arg);dataSize = data.size();}
    };
    inline std::ostream & operator<<( std::ostream & os,const NetFunc & c)
    {
        os << c.objectID << std::endl;
        os << c.index  << std::endl;
        os << c.strName  << std::endl;
        os << c.clientID  << std::endl;

        //std::string strData(c.pData,c.dataSize);
        os << c.data  << std::endl;
        os << c.dataSize  << std::endl;

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
        return is;
    }
    typedef std::function<void(NetFunc *pData)> NETFUNC;
    class RPCObject;
    class ObjectManager;
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
        void init();
        void sendReturnData(std::int64_t index,std::string &arg);
        void setObjectID(std::int64_t objectID){m_objectID = objectID;};
        void setClientID(std::string& str){m_clientID = str;};
        void addNetFunc(std::string strName,NETFUNC func){m_mapNetfunc[strName] = func;}
        void setObjectManager(ObjectManager *pManager){ m_pManager = pManager;};

        std::string getClientID(){return m_clientID;}
        std::int64_t m_funcindex;
        std::int64_t m_objectID;
        std::string m_clientID;
        std::string m_className;
        bool m_bNetObject;
        ObjectManager *m_pManager;
        std::map<std::int64_t,NETFUNC> m_mapReturnFunc;
        std::map<std::string, NETFUNC> m_mapNetfunc;
        std::vector<std::function<void()> > m_arrInit;

        //NETFUNC tt = [this](superrpc::NetFunc *pData){};
        
        friend std::ostream & operator<<( std::ostream & os,const RPCObject & c);
	    friend std::istream & operator>>( std::istream & is,RPCObject & c);
    };
    typedef std::shared_ptr<RPCObject> PTR_RPCObject;
    std::ostream & operator<<( std::ostream & os,const RPCObject & c);
    std::istream & operator>>( std::istream & is,RPCObject & c);

    typedef std::function<PTR_RPCObject(void)> CREATEFUNC;
    class ObjectRegister
    {
    public:
        ObjectRegister(RPCObject *pObj,std::string strName, std::function<void()> func){
            //pObj->m_arrInit.push_back(func);
            func();
        }
    };
    class ObjectTest 
    {
    public:
        ObjectTest (/* args */){};
        ~ObjectTest (){};

        virtual std::future<std::string> getTest(std::string &arg){};
    };

    

    
    RPCObject* InitRPCObject(RPCObject *pObject,std::string strClientID);
    void AddClassTemplate(std::string strClass,superrpc::CREATEFUNC func);
    template<class T>
    class TemplateRegister
    {
        public:
        TemplateRegister(std::string strClass){
            superrpc::CREATEFUNC func = [](){
                return std::make_shared<T>();
            };
            AddClassTemplate(strClass,func);

            //g_mapCreate.size();
        }
    };
    
    PTR_RPCObject CreateRPCObjectByName(std::string className);
        
#define SUPER_CLASS_TOSTR(className) "superrpc"#className

#define   SUPER_CLASS_BEGIN(className) class  superrpc##className :public className,public superrpc::RPCObject{\
public:\
    using super = className;\
    superrpc##className(){\
        m_className = "superrpc"; m_className+= #className;\
        }\
    virtual ~superrpc##className(){};\

#define SUPER_FUNC_STRING(func)\
    virtual std::future<std::string> func(std::string &arg)override {\
        if (m_bNetObject) {\
            auto ff = std::make_shared<std::promise<std::string>>();\
            auto Recdata = [this, ff](superrpc::NetFunc *pData)\
            {\
                ff->set_value(decltype(ff->get_future().get())(pData->data));\
            };\
            this->sendData(__func__,Recdata,arg);\
            return ff->get_future();\
        }\
        else {\
            return super::func(arg);\
        }\
    }\
    void init##func()\
    {\
        auto netFunc = [this](superrpc::NetFunc *pArg) {\
            std::future<std::string> ff =  super::func(pArg->data);\
            auto r = ff.get();\
            sendReturnData(pArg->index,r);\
        };\
        m_mapNetfunc[#func] = netFunc;\
    } \
    superrpc::ObjectRegister register##func = superrpc::ObjectRegister(this,#func,[this](){this->init##func();});

#define SUPER_FUNC_VOID(func)\
    virtual void func()override {\
        if (m_bNetObject) {\
            auto Recdata = [](superrpc::NetFunc *pData){};\
            std::string strArg = "";\
            this->sendData(__func__,Recdata,strArg);\
        }\
        else {\
            return super::func();\
        }\
    }\
    void init##func()\
    {\
        auto netFunc = [this](superrpc::NetFunc *pData) {\
            super::func();\
        };\
        m_mapNetfunc[#func] = netFunc;\
    } \
    superrpc::ObjectRegister register##func = superrpc::ObjectRegister(this,#func,[this](){this->init##func();});\

#define SUPER_CLASS_END(className)\
};\
superrpc::TemplateRegister<superrpc##className> templateregister##className(SUPER_CLASS_TOSTR(className));\

#define SUPER_FUNC_LONG(func)\
    virtual std::future<std::int64_t> func(std::int64_t arg)override {\
        if (m_bNetObject) {\
            auto ff = std::make_shared<std::promise<std::int64_t>>();\
            auto Recdata = [this, ff](superrpc::NetFunc *pData)\
            {\
                ff->set_value(decltype(ff->get_future().get())(pData->toLong()));\
            };\
            std::string strArg = std::to_string(arg);\
            this->sendData(__func__,Recdata,strArg);\
            return ff->get_future();\
        }\
        else {\
            return super::func(arg);\
        }\
    }\
    void init##func()\
    {\
        auto netFunc = [this](superrpc::NetFunc *pArg) {\
            std::future<std::int64_t> ff =  super::func(pArg->toLong());\
            auto r = ff.get();\
            std::string strArg = std::to_string(r);\
            sendReturnData(pArg->index,strArg);\
        };\
        m_mapNetfunc[#func] = netFunc;\
    } \
    superrpc::ObjectRegister register##func = superrpc::ObjectRegister(this,#func,[this](){this->init##func();});

#define SUPER_CREATE(className,clientID)\
    superrpc::CreateRPCObject<superrpc##className>(clientID);\

#define SUPER_CREATESERVER(className,clientID)\
    superrpc::CreateServerRPCObject<superrpc##className>(clientID);\

#define SUPER_CREATECLIENT(className,clientID)\
    superrpc::CreateClientRPCObject<superrpc##className>(clientID);\

    class superrpcObjectTest : public ObjectTest, public RPCObject
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
            std::cout << "initgetTest";
            auto netFunc = [this](NetFunc *pArg) {
                
                std::future<std::string> ff =  ObjectTest::getTest(pArg->data);
                auto r = ff.get();
                sendReturnData(pArg->index,r);

            };
            m_mapNetfunc["getTest"] = netFunc;
        };

        ObjectRegister getTestRegister = 
        ObjectRegister(this,"getTest",[this](){this->initgetTest();});
    };
    
};
#endif