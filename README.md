# superRPC

对RPC的一种封装，使开发者像平常编程一样写代码，程序则自动在远端电脑分配执行。可方便的用于客户端巨量数据计算集群开发，而在编程上减少分布式编程问题。

An encapsulation of RPC that enables developers to write code as usual, and the program is automatically distributed and executed on the remote computer. It can be easily used in the development of client-side massive data computing cluster, and reduce the problem of distributed programming in programming.

class User
{
public:
	virtual std::future<std::string> getName(std::string &arg){std::promise<std::string> p; p.set_value(m_Name); return p.get_future();}
	virtual std::future<std::string> setName(std::string &arg){m_Name = arg; std::promise<std::string> p; p.set_value("fdf"); return p.get_future();}
	virtual void excSome(){std::cout << "runsome" << std::endl;}
	virtual std::future<std::int64_t> getLong(std::int64_t arg){std::promise<std::int64_t> p; p.set_value(100); return p.get_future();}
	std::string m_Name;
};

SUPER_CLASS_BEGIN(User)
SUPER_FUNC_STRING(getName)
SUPER_FUNC_STRING(setName)
SUPER_FUNC_VOID(excSome)
SUPER_FUNC_LONG(getLong)
SUPER_CLASS_END(User)

int main(int argc, char *argv[]) {
	//init main 
	superrpc::InitServer("tcp://*:9999");
	std::this_thread::sleep_for(std::chrono::seconds(2));

	//init client
	superrpc::InitClient("tcp://127.0.0.1:9999","client1");
	std::this_thread::sleep_for(std::chrono::seconds(2));

	//create User class from client1
	auto user = SUPER_CREATE(User,"client1");
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::string str = "fdsffffffffffffffffffffff";
	//exc on client1
	user->setName(str);
	auto get = user->getName(str);
	auto str2 = get.get();

	user->excSome();

	auto longdata = user->getLong(123).get();
	if(str2.size()){
		std::cout << "dsf" << std::endl;
	}
	while (1)
	{
		std::this_thread::sleep_for(std::chrono::seconds(5));
	}
	
	return 0;
}

