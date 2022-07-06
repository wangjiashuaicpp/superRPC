
#include "Test.h"
#include <future>
#include "rpcobject.h"
#include "rpcobjectmanager.h"


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

int main(int argc, char *argv[]) {
	return 0;
}