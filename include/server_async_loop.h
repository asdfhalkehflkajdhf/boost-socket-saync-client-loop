#ifndef SERVER_ASYNC_LOOP_H
#define SERVER_ASYNC_LOOP_H


#include <algorithm>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <set>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/function.hpp>
//#include <functional>
#include "client_msg.hpp"
#include "client_error.h"

using boost::asio::ip::tcp;

//----------------------------------------------------------------------

//rpc 消息队列，每一个会话都会有独立的
typedef std::shared_ptr<RPCStruct> RPCStruct_ptr;
typedef std::deque<RPCStruct_ptr> RPCStruct_queue;
//头问共享指针
typedef std::shared_ptr<RPCStructHead> recvHead_ptr;

//----------------------------------------------------------------------
//回调函数使用的参数结构
struct CallStruct{
	CallStruct(int s):size_(s){
		if(size_>0)
			data_ = new char[size_];
	};
	~CallStruct(){if(data_)delete data_;}
	void data(const char *msg){
		if(size_>0)
			memcpy(data_, msg, size_);
	}
	char* data(){return data_;};
	int size(){return size_;};
private:
	int size_;
	char *data_;
};
typedef std::shared_ptr<CallStruct> CallStruct_ptr;

using CALLBACK_FUN=boost::function<CallStruct_ptr (const char * const msg, int msg_len)>;
//----------------------------------------------------------------------
//一个会话表示一个socket
class link_session
  : public boost::enable_shared_from_this<link_session>
{
public:
  	link_session(boost::asio::io_service& io_service)
    	: io_service_(io_service),
    		socket_(io_service)
  	{}

 	tcp::socket& socket(){
		return socket_;
  	}

  	void start(){//每生成一个新的link_session都会调用,开始读取头
        do_read();
  	}

  	void do_read(){
		std::cout<<std::endl;
		
		recvHead_ptr t(new RPCStructHead());
      	// RPCStruct_ptr readBuf(new RPCStruct(io_service_));
      	boost::asio::async_read(socket_,
          	boost::asio::buffer(t->data(), t->data_len()),
          	boost::bind(
            	&link_session::handle_read_header,
            	shared_from_this(),
            	t,
            	boost::asio::placeholders::error)); //异步读客户端发来的消息
  	}
  	void handle_read_header(recvHead_ptr head, const boost::system::error_code& error)
  	{
	    if (!error)
	    {
			RPCStruct_ptr readBuf(new RPCStruct(io_service_, *(head.get())));
			std::cout<<"1 handle_read_header: reqid=["<<readBuf->head().req_id()<<"] body_len=["<<readBuf->head().body_len()<<"]"<<std::endl;
	      	boost::asio::async_read(socket_,
	          	boost::asio::buffer(readBuf->recv_msg_buff(), readBuf->recv_msg_len()),
	          	boost::bind(&link_session::handle_read_msg,
	            	shared_from_this(),
	            	readBuf,
		        	boost::asio::placeholders::error));
	    }
	    else
	    {
	        std::cout<<"1 sal handle_read_header :"<<" ["<<error_msg(error.value())<<"]"<<std::endl;
	    }
  	}


	//分两个部分，第一部分，回复，第二部分，继续读取
	void handle_read_msg(RPCStruct_ptr readBuf, const boost::system::error_code& error)
	{
		if (!error)
		{
			std::cout<<"2 sal handle_read_msg from client recv: "<<socket_.remote_endpoint().address()<<":" <<socket_.remote_endpoint().port()<<":"<<socket_.local_endpoint().port();
			std::cout<<" body=["<<readBuf->body()<<"] body_len["<<readBuf->head().body_len()<<"]"<<std::endl;
			readBuf->showInfo();
			//第一部分，接收成功，回调函数，生成回复数据进行发送，必须是阻塞线性的
			reply(readBuf);

			//第二部分,开始读取下一个数据包
			start();
		}
		else
		{
			std::cout<<"2 sal handle_read_msg :"<<" ["<<error_msg(error.value())<<"]"<<std::endl;
		}
	}

	void do_write(){
		std::cout<<"3 do write | "<<write_msgs_.front()->body()<<std::endl;
		write_msgs_.front()->showInfo();
		boost::asio::async_write(socket_,
			boost::asio::buffer(write_msgs_.front()->data(), write_msgs_.front()->data_len()),
			boost::bind(&link_session::handle_write, 
				shared_from_this(),
				boost::asio::placeholders::error));
	}
	void reply(const RPCStruct_ptr msg)
	{
		
		RPCStructHead th;
		RPCStruct_ptr newMsg(new RPCStruct(io_service_));
		if(call_fun){
			CallStruct_ptr res = call_fun(msg->body(), msg->head().body_len());
			if(res && res->size()>0 && res->data()){
				th.req_id(msg->head().req_id());
				th.body_len(res->size());
				newMsg->head(th);
				//设置meta
				char meta_s[9]="12345678";
				newMsg->meta(meta_s);
				//设置body
				newMsg->body(res->data());
				
				write_msgs_.push_back(newMsg);
			}else{
				//如果回调返回结果为空，则进行回显操作
				write_msgs_.push_back(msg);
			}
		}else{
			//如果回调函数为空，则进行回显操作
			write_msgs_.push_back(msg);
		}
		
		//bool write_in_progress = !write_msgs_.empty();
		//把room中保存的消息挂到write_msgs队尾
		//不为空时
		if (write_msgs_.empty() == false)
		{
			do_write();
		}		

	}

	void handle_write(const boost::system::error_code& error)
	{
		if (!error)
		{
			std::cout<<"3 sal handle_write ok"<<std::endl;
			write_msgs_.pop_front();
			if (!write_msgs_.empty())
			{
				std::cout<<"3 do write next"<<std::endl;
				do_write();
			}
		}
		else
		{
			std::cout<<"3 sal handle_write :"<<" ["<<error_msg(error.value())<<"]"<<std::endl;
		}
	}

	void register_callback(CALLBACK_FUN f){
		call_fun=boost::move(f);
		if(call_fun){
			std::cout<<"link_session rc callback_fun"<<std::endl;
		}else
			std::cout<<"link_session rc callback_fun error"<<std::endl;
				
	}
private:
	boost::asio::io_service& io_service_;
	tcp::socket socket_;
	RPCStruct_queue write_msgs_;
	//boost::function默认值为空，可以进行bool判断
	CALLBACK_FUN call_fun;

};



//----------------------------------------------------------------------
//监听服务，主要功能负责请的请求时，开始一个新的socket,一个server表示一个房间
//一个房间可以有多个人，每个人表示一个会话(socket)
typedef boost::shared_ptr<link_session> link_session_ptr;
class link_server
{
public:
	link_server(boost::asio::io_service& io_service,
		  const tcp::endpoint& endpoint)
		: io_service_(io_service),
		  acceptor_(io_service, endpoint) //全局只有一个io_service和一个acceptor
	{}

	void start_accept()
	{
		link_session_ptr new_session(new link_session(io_service_));
		//session注册回调处理函数
		new_session->register_callback(call_fun);
		acceptor_.async_accept(new_session->socket(),
			boost::bind(&link_server::handle_accept, this, new_session,
			boost::asio::placeholders::error));
	}

	void handle_accept(link_session_ptr session,
		const boost::system::error_code& error)
	{
		if (!error){
			session->start();
			std::cout<<"sal add session"<<std::endl;
		}

		start_accept(); //开始监听下一个链接
	}
	
	void register_callback(CALLBACK_FUN f){
		call_fun=boost::move(f);
		if(call_fun){
			std::cout<<"link_server rc callback_fun"<<std::endl;
		}else
			std::cout<<"link_server rc callback_fun error"<<std::endl;
	}

private:
	boost::asio::io_service& io_service_;
	tcp::acceptor acceptor_;
	//boost::function默认值为空，可以进行bool判断
	CALLBACK_FUN call_fun;
	
};

typedef boost::shared_ptr<link_server> link_server_ptr;
typedef std::list<link_server_ptr> link_server_list;

//----------------------------------------------------------------------




#endif // SERVER_ASYNC_LOOP_H
