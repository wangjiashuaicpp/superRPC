#pragma once
#include <functional>
#include <map>
#include <vector>
#include <future>
#include <string.h>
#include <iostream>
#include <streambuf>
#include <sstream>
#include <fstream>
#include <type_traits>
typedef std::function<void(const char* data)> CALLFUNC;




template <typename T> typename std::enable_if<std::is_same<T,std::string>::value,std::string>::type
descchar (T &i,char *pInData) { 
    //i = pInData;
	strcpy(i,pInData);
    return i;  
}

template <typename T> typename std::enable_if<std::is_same<T,const char*>::value,std::string>::type
descchar (T i,char *pInData) { 
    i = pInData;
    return i;  
}


template<typename T>
typename std::enable_if<std::is_arithmetic<T>::value, std::string>::type
descchar(T &t,char *pInData) { return std::to_string(t); }

template<typename T>
typename std::enable_if<std::is_class<T>::value, std::string>::type
descchar(T &t,char *pInData) 
{ memcpy(&t, pInData, sizeof(T)); }

class NetObject
{
public:

	long m_objectid;
	NetObject() {};
	NetObject(long netid) { m_objectid = netid; }

	friend std::ostream & operator<<( std::ostream & os,const NetObject & c);
	friend std::istream & operator>>( std::istream & is,NetObject & c);
	void sendFunc(long long index, std::string funcname, char* pData, int datasize)
	{

	}

	void recvFunc(long long index, char* pData)
	{
		auto iter = m_mapBackFunc.find(index);
		if (iter != m_mapBackFunc.end()) {
			iter->second(pData);
		}

	}

	void insertBackFunc(long long index,CALLFUNC func)
	{
		m_mapBackFunc.insert(std::make_pair(index,func));
	}
	template<typename T1>
	void backData(long long index, T1 &t)
	{
		std::stringbuf buf;
		std::ostream out(&buf);

		//NetFunc funcinfo;
		//funcinfo.index = index;
		//funcinfo.strName ="backData";
		out << t;

	}

	template<typename T>
	int descArg(char* pData, T& t1) {

		if (typeid(t1) == typeid(char*))
		{
			int size = 0;
			memcpy(&size, pData, sizeof(int));
			//memcpy(&t1, pData + sizeof(int), size);
			descchar(t1,pData + sizeof(int));
			return size;
		}
		else if (typeid(t1) == typeid(std::string)) {
			int size = 0;

			memcpy(&size, pData, sizeof(int));
			void* pNewData = malloc(size);
			memcpy(pNewData, pData + sizeof(int), size);
			descchar(t1,(char*)pNewData);
			free(pNewData);

			return size + sizeof(int );
		}
		else {
			descchar(t1,pData);
			//memcpy(&t1, pData, sizeof(T));

			return sizeof(T);
		}

		return 0;
	}

	template<typename T1, typename T2, typename T3>
	void descArg(char* pData, T1& t1,T2 &t2,T3 &t3) {
		int begin = sizeof(long long) * 2;

		begin += descArg(pData + begin, t1);
		begin += descArg(pData + begin, t2);
		begin += descArg(pData + begin, t3);
	}

	long long getNewFuncIndex()
	{
		m_funcindex++;
		return m_funcindex;
	}
	long long m_funcindex;
	std::map<long long, CALLFUNC> m_mapBackFunc;
};

std::ostream & operator<<( std::ostream & os,const NetObject & c)
{
	char arr[32] = {};
    sprintf(arr,"%p",&c);
    char* str;
    long i = strtol(arr, &str, 16);
	os << i;
	return os;
}

std::istream & operator>>( std::istream & is,NetObject & c)
{
	is >> c.m_objectid;
	return is;
}

struct  NetPack
{
	int size;
	char *pData;
	friend std::ostream & operator<<( std::ostream & os,const NetPack & c);
	friend std::istream & operator>>( std::istream & is,NetPack & c);	
};
std::ostream & operator<<( std::ostream & os,const NetPack & c)
{
	os << c.size;
	std::string str(c.pData,c.size);
	os << str;
	
	return os;
}

std::istream & operator>>( std::istream & is,NetPack & c)
{
	is >> c.size;
	std::string str;
	is >> str;
	c.pData = new char[str.size()];
	str.copy(c.pData,str.size());
	return is;
}
struct NetFunc
{
	long long index;
	std::string strName;
	friend std::ostream & operator<<( std::ostream & os,const NetFunc & c);
	friend std::istream & operator>>( std::istream & is,NetFunc & c);	
};

std::ostream & operator<<( std::ostream & os,const NetFunc & c)
{
	os << c.index;
	os << c.strName;
	
	return os;
}

std::istream & operator>>( std::istream & is,NetFunc & c)
{
	is >> c.index;
	is >> c.strName;
	return is;
}

template <typename tup,class R,class T, class... Args> typename std::enable_if<1 ==  std::tuple_size<tup>::value>::type 
	InitFuncArgs(tup& t,R(T::*Func)(Args...),T *p) 
{
	typename std::tuple_element<0, std::tuple<Args...>>::type t1;

	(p->*Func)(t1);
  std::cout << std::endl;
}

template <typename tup,class R,class T, class... Args> typename std::enable_if<2 ==  std::tuple_size<tup>::value>::type 
	InitFuncArgs(tup& t,R(T::*Func)(Args...),T *p) 
{
	typename std::tuple_element<0, std::tuple<Args...>>::type t1;
	typename std::tuple_element<1, std::tuple<Args...>>::type t2;
	(p->*Func)(t1,t2);
  std::cout << std::endl;
}
template <typename tup,class R,class T, class... Args> typename std::enable_if<3 == std::tuple_size<tup>::value>::type 
	InitFuncArgs(tup& t,R(T::*Func)(Args...),T *p) 
{
	typename std::tuple_element<0, std::tuple<Args...>>::type t1;
	typename std::tuple_element<1, std::tuple<Args...>>::type t2;
	typename std::tuple_element<2, std::tuple<Args...>>::type t3;
	(p->*Func)(t1,t2,t3);
  std::cout << std::endl;
}

template <class R,class T, class... Args>
size_t InitFunc(R(T::*Func)(Args...),T *t)
 {
	size_t size = sizeof...(Args);
	typedef typename std::tuple_element<0, std::tuple<Args...>>::type type;


	
	std::tuple<Args...> tups;
	InitFuncArgs(tups,Func,t);
}


class sendClass : public NetObject
{
public :
	template<typename... T2>
	void sendData(const char *name, CALLFUNC func,T2... arg)
	{
		int size = sizeof...(arg);

		
		return sendData(name,arg...);
	}


	template<typename T2, typename T3, typename T4>
	void sendData(const char* name, CALLFUNC func,T2 t, T3 tt, T4 ttt)
	{
		std::stringbuf buf;
		std::ostream out(&buf);
		out << (*this);

		NetFunc funcinfo;
		funcinfo.index = getNewFuncIndex();
		funcinfo.strName = name;
		out << funcinfo;
		out << name;
		out << t;
		out << tt;
		out << ttt;

		NetPack pack;
		auto str = buf.str();
		//pack.size = str.size();
		//pack.pData = str.data();
		insertBackFunc(funcinfo.index,func);
	}

	bool m_bSend;

	std::map<std::string, CALLFUNC> m_mapRecvfunc;
};

class Test
{
public:
	virtual std::future<long> getdata(int i, long e, const char* p) { std::promise<long> r; return r.get_future(); };
	virtual void getsis(int i, long e, const char* p) {};
};

template<typename T2> void funtest(T2& t)
{

}


template <bool, typename T=void>
struct enable_if {
};
template <typename T>
struct enable_if<true, T> {
  using type = T;
};
#define DEARG(...) __VA_ARGS__
#define INITFUNC(R,FUNC,ARGS,...)\
	virtual R FUNC(ARGS)\
	{\
	FUNC(__VA_ARGS__);\
	}
class Test2 : public Test,sendClass
{
public:

	Test2() {

	}
	INITFUNC(int,ttt,DEARG(int I,int W),I,W);
	virtual std::future<long> initFunc()
	{
		//InitFunc(&Test::getdata,(Test*)this);
		
	}


	CALLFUNC callfunc;
	template<typename... T2>
	std::future<long>  getdata(T2... arg)
	{
		if (m_bSend) {
			auto ff = std::make_shared<std::promise<long>>();

			auto Recdata = [this, ff](const char* data)
			{
				ff->set_value(decltype(ff->get_future().get())(data));
			};
			
			
			sendData(__func__, Recdata, arg...);
			return ff->get_future();
		}
		else {
			return Test::getdata(arg...);
		}

		

		auto netFunc = [arg...,this](const char *data) {
			
			long long funcIndex = 0;
			descArg((char*)data, arg...);
			std::future<long> ff =  this->getdata(arg...);

			auto r = ff.get();

			backData(funcIndex,r);

		};
		callfunc = netFunc;
		callfunc("");

		//getNumArgs(&Test2::getdata);

		
	}

#define DEFFUNC3(func)public:  template<typename... T2> auto  func(T2... arg){\
	if (m_bSend) {\
		sendData(__func__, arg...);\
	}\
	else {\
		return __super::func(arg...);\
	}\
}\
template<typename... T2> void  init##func(T2... arg)\
{		auto Add = [arg..., this](const char *data) {\
			this->get##func(data, arg...);\
		};\
	callfunc = Add;\
}\
template<typename T2, typename T3, typename T4> void get##func(const char *data, T2 t, T3 tt, T4 ttt)\
{\
	this->func(t, tt, ttt);\
}

	template<typename... T2>
	void  getdatarr(T2... arg)
	{
		auto Add = [arg..., this](const char *data) {
			this->runnet(data, arg...);
		};
		callfunc = Add;
		callfunc("");
	}



	template<typename T2, typename T3, typename T4>
	void runnet(const char *data, T2 t, T3 tt, T4 ttt)
	{
		long long funcIndex = 0;
		descArg(data, t, tt, ttt);
		std::future<long> ff =  this->getdata(t, tt, ttt);

		ff.get();
		//backData(funcIndex);
	}

#define INITFUN3(func,arg1,arg2,arg3)\
	{arg1 t1;arg2 t2;arg3 t3;init##func(t1,t2,t3);}

#define INITBEGIN void init(){
#define INITEND }

	INITBEGIN
	
	INITEND

		std::map<int, int> m_mapint;
		std::function<void(void)> m_func = []() {};
		

		
};



