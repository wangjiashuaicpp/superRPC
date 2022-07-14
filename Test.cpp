
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
	virtual std::future<std::string> getName(std::string &arg){}
	virtual std::future<std::string> setName(std::string &arg){}
};

SUPER_CLASS_BEGIN(User)
SUPER_FUNC_STRING(getName)
SUPER_FUNC_STRING(setName)
SUPER_CLASS_END(User)

int main(int argc, char *argv[]) {

	superrpc::InitServer("tcp://*:9999");

	superrpc::InitClient("tcp://127.0.0.1:9999","client1");

	User *user = SUPER_CREATE(User,"client1");
	std::string str;
	user->getName(str);

	while (1)
	{
		std::this_thread::sleep_for(std::chrono::seconds(5));
	}
	
	return 0;
}

