
#include "Test.h"
#include <future>
#include "rpcobject.h"
#include "rpcobjectmanager.h"
#include "superrpc.h"
#include <chrono>
#include <thread>

class User
{
public:
	virtual std::future<std::string> getName(std::string &arg){
			std::promise<std::string> p; 
			p.set_value(m_Name); 
			return p.get_future();
		}
	virtual std::future<std::string> setName(std::string &arg){
			m_Name = arg; 
			std::promise<std::string> p; 
			p.set_value("fdf"); 
			return p.get_future();
		}
	virtual void excSome(){std::cout << "runsome" << std::endl;}
	
	virtual std::future<std::int64_t> getLong(std::int64_t arg){
			std::promise<std::int64_t> p; 
			p.set_value(100); 
			return p.get_future();
		}

	virtual void addEncodeData(const char *pData,std::size_t size){

		std::vector<char> v;
		v.resize(size);
		m_call(v);	
	};

	virtual void setCall(superrpc::RPCObjectCall call){
		m_call = call;
	}
	superrpc::RPCObjectCall m_call;

	std::string m_Name;
};


SUPER_CLASS_BEGIN(User)
SUPER_FUNC_STRING(getName)
SUPER_FUNC_STRING(setName)
SUPER_FUNC_VOID(excSome)
SUPER_FUNC_LONG(getLong)
SUPER_FUNC_CALL(setCall)
SUPER_FUNC_PCHAR_SIZE(addEncodeData)
SUPER_CLASS_END(User)

class VideoEncode
{

public:
	VideoEncode(/* args */){};
	~VideoEncode(){};
	virtual void init(std::int64_t width,std::int64_t height){
		//init ffmpeg or some
		std::cout << "VideoEncode init " << width << " " << height << std::endl;
	};
	//add encode data
	virtual void addEncodeData(const char *pData,std::size_t size){

		/*
			addEncodeTask(pData,size);
		*/
		superrpc::RPCStream str(pData,size);
		//encode call
		m_call(str.vectorSize);	
	};
	//encode call back 
	virtual void setCall(superrpc::RPCObjectCall call){
		m_call = call;
	}
	superrpc::RPCObjectCall m_call;	
};


SUPER_CLASS_BEGIN(VideoEncode)
SUPER_FUNC_VOID_LONG_LONG(init)
SUPER_FUNC_CALL(setCall)
SUPER_FUNC_PCHAR_SIZE(addEncodeData)
SUPER_CLASS_END(VideoEncode)

int main(int argc, char *argv[]) {
	//init main 
	superrpc::InitServer("tcp://*:9999");
	std::this_thread::sleep_for(std::chrono::seconds(2));

	//init client
	superrpc::InitClient("tcp://127.0.0.1:9999","client1");
	std::this_thread::sleep_for(std::chrono::seconds(2));
	//
	//superrpc::InitClient("tcp://127.0.0.1:9999","client2");
	//std::this_thread::sleep_for(std::chrono::seconds(2));
	//create User class from client1
	auto user = SUPER_CREATE(User,"client1");
	std::this_thread::sleep_for(std::chrono::seconds(1));

	std::string str = "hanmeimei";
	//exc on client1
	user->setName(str);
	auto get = user->getName(str);
	auto getStr = get.get();
	if(getStr.size()){
		std::cout << "string get it!" << std::endl;
	}
	user->excSome();
	auto longdata = user->getLong(123).get();

	superrpc::RPCObjectCall rpccall([](std::vector<char>& data){
		std::cout << "rpccall " << data.size() << std::endl;
	});
	user->setCall(rpccall);
	user->addEncodeData("rgbdata",7);


	//VideoEncode test //////////////
	//it will be create on client1
	auto encode1 = SUPER_CREATE(VideoEncode,"client1");
	//will be create on client2
	auto encode2 = SUPER_CREATE(VideoEncode,"client2");
	encode1->init(4098,2085);
	encode2->init(4089,2048);

	superrpc::RPCObjectCall rpccall1([](std::vector<char>& data){
		//client1 encode data
		std::string str(data.data(),data.size());
		std::cout << "call1:" <<str<< std::endl;
	});
	encode1->setCall(rpccall1);

	superrpc::RPCObjectCall rpccall2([](std::vector<char>& data){
		//client2 encode data
	});
	encode2->setCall(rpccall2);

	std::string encodeData = "fdsfffffffffffffffff3424523423fsdfsdfsd";
	encode1->addEncodeData(encodeData.c_str(),encodeData.size());
	while (1)
	{
		std::this_thread::sleep_for(std::chrono::seconds(5));
	}
	
	return 0;
}

