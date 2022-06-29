
#include "Test.h"
#include <future>



void calldata(int i, int i2, int i3)
{
	Test2 tdata;
	tdata.getdatarr(2, 3, "");
	
}

std::future<int> getFuture()
{
	std::promise<int> promiseObj;

	promiseObj.set_value(int("xx"));
	return promiseObj.get_future();
}

