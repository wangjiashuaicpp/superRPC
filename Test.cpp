
#include "Test.h"
#include <future>
#include "rpcobject.h"
#include "rpcobjectmanager.h"
#include "superrpc.h"
#include <chrono>
#include <thread>

void calldata(int i, int i2, int i3)
{
	Test2 tdata;
	tdata.getdatarr(2, 3, "");
	
}

std::future<int> getFuture()
{
	std::promise<int> promiseObj;

	//promiseObj.set_value(int("xx"));
	return promiseObj.get_future();
}

class User
{
public:
	virtual std::future<std::string> getName(std::string &arg){std::promise<std::string> p; p.set_value(m_Name); return p.get_future();}
	virtual std::future<std::string> setName(std::string &arg){m_Name = arg; std::promise<std::string> p; p.set_value("fdf"); return p.get_future();}

	std::string m_Name;
};

SUPER_CLASS_BEGIN(User)
SUPER_FUNC_STRING(getName)
SUPER_FUNC_STRING(setName)
SUPER_CLASS_END(User)

int main(int argc, char *argv[]) {

	superrpc::InitServer("tcp://*:9999");
	std::this_thread::sleep_for(std::chrono::seconds(2));
	superrpc::InitClient("tcp://127.0.0.1:9999","client1");
	std::this_thread::sleep_for(std::chrono::seconds(2));

	auto user = SUPER_CREATE(User,"client1");
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::string str = "fdsffffffffffffffffffffff";
	user->setName(str);
	auto get = user->getName(str);
	auto str2 = get.get();
	if(str2.size()){
		std::cout << "dsf" << std::endl;
	}
	while (1)
	{
		std::this_thread::sleep_for(std::chrono::seconds(5));
	}
	
	return 0;
}

