// g++ work.cpp -lboost_system -lboost_filesystem -lboost_thread 

#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

#include <sys/time.h>
#include <time.h>

#ifndef _SOCKET_TYPE_H
#define _SOCKET_TYPE_H
 
#ifdef _WIN32
#include <winsock2.h>
#define ERRNO		(WSAGetLastError())
#define NEEDBLOCK	WSAEWOULDBLOCK
typedef int socklen_t;
#else
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#ifndef INVALID_SOCKET
#define INVALID_SOCKET	(SOCKET)(~0)
#endif
#ifndef SOCKET_ERROR
#define SOCKET_ERROR	(-1)
#endif
#ifndef closesocket
#define closesocket(x)	::close(x)
#endif
typedef int SOCKET;
#define ERRNO (errno)
#define NEEDBLOCK	EAGAIN
#endif
 
#endif

using namespace boost;
using namespace boost::asio;

//不起作用

// SOCKET native_sock = my_socket.native();
// int result = SOCKET_ERROR;

// if (INVALID_SOCKET != native_sock)
// {
    // result = setsockopt(native_sock, SOL_SOCKET, <the pertinent params you want to use>);
// }

char buf[1024];
	io_service ios;
	boost::asio::ip::tcp::socket socket_(ios);

void handle_read(const boost::system::error_code& error)
{
	if (!error)
	{
		std::cout<<"read ok"<<std::endl;
	}else{
		std::cout<<"read "<<error.message()<<std::endl;
	}
}
void do_read(){
		boost::asio::async_read(socket_,
				boost::asio::buffer(buf, 1024), //读取数据报头
				boost::bind(&handle_read,
					boost::asio::placeholders::error));

}

void handle_connect(const boost::system::error_code& error)
{
	if (!error)
	{
		std::cout<<"conn ok"<<std::endl;
		do_read();
	}else{
		std::cout<<"conn "<<error.message()<<std::endl;
	}
}





int main(){
	

	//设置超时时间
	// TimeoutAdjust adjust(1000);
	// socket_.set_option(adjust);
	
	SOCKET native_sock = socket_.native();
	int result = SOCKET_ERROR;

	if (INVALID_SOCKET != native_sock)
	{
		int nNetTimeout=1000;//1秒
		struct timeval timeout = {1,0};
		// struct timeval { 
// __kernel_time_t tv_sec; /* seconds */ 
// __kernel_suseconds_t tv_usec; /* microseconds */ 
// };
//		result = setsockopt(native_sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
		result = setsockopt(native_sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(nNetTimeout));

		}
	
	try {
		socket_.async_connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 6688),
		boost::bind(&handle_connect,
				boost::asio::placeholders::error)); 
	} catch(boost::system::system_error e) {
		std::cout << e.code() << std::endl;
	}
	
		
		


	
	int thread_num = 1;
	thread *t[thread_num] = {0};
	
	//启动work,　ios中没有任务不会退出，注释掉的话会立刻退出　使用work.reset()来结束其生命周期
	//io_service::work work(ios);
	
	
	// 创建线程池
	for(int i = 0; i < thread_num; ++i)
	{
		t[i] = new thread(bind(&io_service::run, &ios));
	}

	
	// 向任务队列中投递任务，该任务将随机由一个调用run方法的线程执行

	// 等待线程退出
	for(int i = 0; i < thread_num; ++i)
	{
		t[i]->join();
	}	

	std::cout<<"exit"<<std::endl;
	return 0;
}
