#include <iostream>

#include "server_async_loop.h"

using namespace std;

CallStruct_ptr call_p(const char * const msg, int msg_len)
{
	std::cout<<"call_p | "<<msg_len<<" | "<<msg<<std::endl;
	CallStruct_ptr res(new CallStruct(14));
	string sendMsg="hello client";
	
	res->data(sendMsg.c_str());
	return res;
}

int main(int argc, char* argv[])
{
  try
  {
    if (argc < 2)
    {
      std::cerr << "Usage: link_server <port> [<port> ...]\n";
      return 1;
    }

    boost::asio::io_service io_service;

    link_server_list servers;
    for (int i = 1; i < argc; ++i)
    {
      using namespace std; // For atoi.
      tcp::endpoint endpoint(tcp::v4(), atoi(argv[i]));
      link_server_ptr server(new link_server(io_service, endpoint));
	  //注册回调函数
	  server->register_callback(&call_p);
	  //开始监听，如果没有注册回调，则服务进行回显服务
	  server->start_accept();
	  // server->register_callback(boost::bind(&call_p, std::placeholders::_1, std::placeholders::_2));
	  //回入到server list
      servers.push_back(server);
    }

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
