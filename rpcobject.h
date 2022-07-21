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
    
    struct RPCStream
    {
        std::vector<char> vectorSize;
        int index = 0;
        RPCStream(int size){index = 0;vectorSize.resize(size);}
        RPCStream(const char* pData,int size){
            vectorSize.resize(size);
            index = 0;
            memcpy(vectorSize.data(),pData,size);
        };
        RPCStream& operator<<(char c)
        {
            memcpy(vectorSize.data() + index,&c,sizeof(c));
            index += sizeof(c);
            return *this;
        }
        RPCStream& operator<<(std::int64_t c)
        {
            memcpy(vectorSize.data() + index,&c,sizeof(c));
            index += sizeof(c);
            int size = vectorSize.size();
            if(size > 0){

            }
            return *this;
        }  
        RPCStream& operator<<(std::string c)
        {
            std::int64_t strsize = c.size();
            memcpy(vectorSize.data() + index,&strsize,sizeof(strsize));
            index+= sizeof(strsize);
            memcpy(vectorSize.data() + index,c.c_str(),c.size());
            index += c.size();
            return *this;
        }  
        RPCStream& operator<<(bool c)
        {
            memcpy(vectorSize.data() + index,&c,sizeof(c));
            index += sizeof(c);
            return *this;
        }   
        RPCStream& operator<<(int c)
        {
            memcpy(vectorSize.data() + index,&c,sizeof(c));
            index += sizeof(c);
            return *this;
        }        
        ////////////////////////////     


        RPCStream& operator >> (char& c)
        {
            memcpy(&c,vectorSize.data() + index,sizeof(c));
            index += sizeof(c);
            return *this;
        }
        RPCStream& operator >> (std::int64_t& c)
        {
            memcpy(&c,vectorSize.data() + index,sizeof(c));
            index += sizeof(c);
            return *this;
        }  
        RPCStream& operator >> (std::string& c)
        {
            std::int64_t strsize = 0;
            memcpy(&strsize,vectorSize.data() + index,sizeof(strsize));
            index += sizeof(strsize);
            auto pchar = new char[strsize];
            memcpy(pchar,vectorSize.data() + index,strsize);
            c.assign(pchar,strsize);
            delete []pchar;
            index += c.size();
            return *this;
        }  
        RPCStream& operator>>(bool& c)
        {
            memcpy(&c,vectorSize.data() + index,sizeof(c));
            index += sizeof(c);
            return *this;
        }   
        RPCStream& operator>>(int& c)
        {
            memcpy(&c,vectorSize.data() + index,sizeof(c));
            index += sizeof(c);
            return *this;
        } 
    };
    
    struct NetFunc
    {

        std::int64_t index;
        std::int64_t objectID;
        std::string strName;
        std::string clientID;
        std::string data;
        int dataSize;
        bool bCall = false;
        friend RPCStream & operator<<( RPCStream & os,const NetFunc & c);
	    friend RPCStream & operator>>( RPCStream & is,NetFunc & c);

        std::int64_t toLong(){return std::atoll(data.c_str());}
        void    packLong(std::int64_t arg){data = std::to_string(arg);dataSize = data.size();}
    };
    inline RPCStream & operator<<( RPCStream& os,const NetFunc & c)
    {
        os << c.objectID ;
        os << c.index  ;
        os << c.strName  ;
        os << c.clientID  ;

        //std::string strData(c.pData,c.dataSize);
        os << c.data  ;
        os << c.dataSize  ;
        os << c.bCall ;

        return os;
    }    
    inline RPCStream & operator>>( RPCStream & is,NetFunc & c)
    {
        is >> c.objectID;
        is >> c.index;
        is >> c.strName;
        is >> c.clientID;
        //std::string strData;
        
        is >> c.data;
        is >> c.dataSize;
        is >> c.bCall;
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
        void sendCallData(std::string callAddress,std::string& arg);
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
        
        friend RPCStream & operator<<( RPCStream & os,const RPCObject & c);
	    friend RPCStream & operator>>( RPCStream & is,RPCObject & c);
    };
    typedef std::shared_ptr<RPCObject> PTR_RPCObject;
    RPCStream & operator<<( RPCStream & os,const RPCObject & c);
    RPCStream & operator>>( RPCStream & is,RPCObject & c);

    typedef std::function<PTR_RPCObject(void)> CREATEFUNC;


    class ObjectRegister
    {
    public:
        ObjectRegister(RPCObject *pObj,std::string strName, std::function<void()> func){
            //pObj->m_arrInit.push_back(func);
            func();
        }
    };

    class RPCObjectCall : public RPCObject
    {
    public:
        RPCObjectCall(){buildAddress();}
        RPCObjectCall(const std::function<void(std::string&)> func){
            m_funcall = std::make_shared<std::function<void(std::string&)>>();
            (*m_funcall) = func;
            buildAddress();
        };
        ~RPCObjectCall(){};
        
        virtual void call(std::string& data);
        RPCObjectCall & operator = (const std::function<void(std::string&)> func)
        {
            m_funcall = std::make_shared<std::function<void(std::string&)>>();
            (*m_funcall) = func;
            buildAddress();
            return *this;
        }
        RPCObjectCall & operator = (const RPCObjectCall &func)
        {
            //m_funcall = func.m_funcall;
            //m_funcall = std::make_shared<std::function<void(std::string)>>();
            m_funcall = func.m_funcall;
            m_bNetObject = func.m_bNetObject;
            m_pParent = func.m_pParent;
            m_funcall = func.m_funcall;
            m_strCallAddress = func.m_strCallAddress;
            //buildAddress();
            return *this;
        }            
        bool operator ()(std::string& data) {
            if(m_bNetObject){
                if(m_pParent){
                    m_pParent->sendCallData(m_strCallAddress,data);
                }
            }
            else{
                if(m_funcall){
                    if(m_funcall.get())
                    (*m_funcall)(data);
                }
            }

            return true;
        }

        void buildAddress()
        {
            char arr[32] = {};
            sprintf(arr,"%p",m_funcall.get());
            m_strCallAddress = arr;

            // m_objectID = 0;
            // m_className = "RPCObjectCall";
            // m_clientID = "null";
        }
         std::shared_ptr<std::function<void(std::string&)>> m_funcall;
         std::string m_strCallAddress;
         RPCObject *m_pParent;

        friend RPCStream & operator<<( RPCStream & os,const RPCObjectCall & c);
	    friend RPCStream & operator>>( RPCStream & is,RPCObjectCall & c);
    };


    class ObjectTest 
    {
    public:
        ObjectTest (/* args */){};
        ~ObjectTest (){};

        virtual std::future<std::string> getTest(std::string &arg){};

        virtual void setCall(RPCObjectCall call){
            m_call = call;
        }
        RPCObjectCall m_call;
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

#define SUPER_FUNC_VOID_STRING(func)\
    virtual void func(std::string arg)override {\
        if (m_bNetObject) {\
            auto Recdata = [this](superrpc::NetFunc *pData)\
            {\
            };\
            this->sendData(__func__,Recdata,arg);\
        }\
        else {\
            return super::func(arg);\
        }\
    }\
    void init##func()\
    {\
        auto netFunc = [this](superrpc::NetFunc *pArg) {\
            super::func(pArg->data);\
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

#define SUPER_FUNC_PCHAR_SIZE(func)\
    virtual void func(const char *pData,std::size_t size) override {\
        if (m_bNetObject) {\
            auto Recdata = [this](superrpc::NetFunc *pData)\
            {\
            };\
            std::string strArg(pData,size);\
            this->sendData(__func__,Recdata,strArg);\
        }\
        else {\
            return super::func(pData,size);\
        }\
    }\
    void init##func()\
    {\
        auto netFunc = [this](superrpc::NetFunc *pArg) {\
            super::func(pArg->data.c_str(),pArg->dataSize);\
        };\
        m_mapNetfunc[#func] = netFunc;\
    } \
    superrpc::ObjectRegister register##func = superrpc::ObjectRegister(this,#func,[this](){this->init##func();});

#define SUPER_FUNC_CALL(func)\
    virtual void func(superrpc::RPCObjectCall call)override {\
        if(m_bNetObject){\
            auto Recdata = [](superrpc::NetFunc *pData)\
            {\
            };\
            superrpc::RPCStream out(64);\
            out << call;\
            std::string sendStr(out.vectorSize.data(),out.vectorSize.size()); \
            this->sendData(__func__,Recdata,sendStr);\
            call.m_bNetObject = false;\
            super::func(call);\
        }\
        else{\
            return super::func(call);\
        }\
    }\
    void init##func()\
    {\
        auto netFunc = [this](superrpc::NetFunc *pArg) {\
            superrpc::RPCObjectCall call;\
            superrpc::RPCStream buf(pArg->data.c_str(),pArg->data.size());\
            call.m_bNetObject = true;\
            call.m_pParent = this;\
            buf >> call;\
            super::func(call);\
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

        virtual void setCall(RPCObjectCall call){
            if(m_bNetObject){
                auto Recdata = [this](NetFunc *pData)
                {
                };
                //std::stringbuf buf;
                superrpc::RPCStream out(64);
                out << call;
                std::string sendStr(out.vectorSize.data(),out.vectorSize.size()); 
                this->sendData(__func__,Recdata,sendStr);
                call.m_bNetObject = false;
                ObjectTest::setCall(call);
            }
            else{
                return ObjectTest::setCall(call);
            }
        }

        void initsetCall()
        {
            auto netFunc = [this](NetFunc *pArg) {
                RPCObjectCall call;
                //std::string str = pArg->data;
                superrpc::RPCStream buf(pArg->data.c_str(),pArg->data.size());
                //std::istream out(&buf);
                call.m_bNetObject = true;
                call.m_pParent = this;
                buf >> call;
                ObjectTest::setCall(call);

            };
            m_mapNetfunc["getTest"] = netFunc;
        };        
    };
    
};
#endif