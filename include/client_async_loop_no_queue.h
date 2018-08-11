#ifndef CLIENT_ASYNC_LOOP_H
#define CLIENT_ASYNC_LOOP_H


//测试不使用消息队列，直接使用异步写队列


#include <cstdlib>
#include <deque>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <string>
#include <atomic>
#include <memory>
#include <cstdarg>

#include <thread>         // std::thread, std::thread::id, std::this_thread::get_id
#include <chrono>         // std::chrono::seconds


#include "client_msg.hpp"
#include "safemap.hpp"
#include "safequeue.hpp"
#include "client_error.h"

using boost::asio::ip::tcp;
using namespace std;
typedef std::shared_ptr<RPCStruct> RPCStruct_ptr;
typedef std::shared_ptr<RPCStructHead> recvHead_ptr;

typedef SafeQueue<RPCStruct_ptr> RPCStruct_queue;
typedef SafeMap<int , RPCStruct_ptr> RPCStruct_map;


class client_loop
{
private:
    string ip_;
    int port_;
public:
    boost::asio::io_service &io_service_;
private:
    tcp::socket *socket_;
    //请求ＩＤ
    std::atomic_int msg_index_=ATOMIC_VAR_INIT(1);
    //消息接收map
    RPCStruct_map recv_msg_map_;
    //消息发送队列
    // RPCStruct_queue write_msg_queue_;

    //定时器
    //删除
    boost::asio::deadline_timer e_timer;
    //日志
    boost::asio::deadline_timer l_timer;
	int e_timer_i;
	int l_timer_i;

    //不需要初始化成员变量
    SafeQueue<int> del_reqid_list;

	//主要使用是对write动作的限定，在服务器中断之后仍然发消息，会产生Broken pipe错误
	std::atomic<client_status_type> clietn_status=ATOMIC_VAR_INIT(CT_UNKNOWN);

public:
    //初始化参数说明：应该按类中变量顺序时行排列，否则会出warning
    client_loop(boost::asio::io_service &io_service, string ip, int port)
        : ip_(ip), port_(port),
          io_service_(io_service),
            socket_(nullptr), //使得成员函数能直接使用这些变量
            e_timer(io_service),
            l_timer(io_service),
			e_timer_i(3),//删除定时器间隔，单位秒
			l_timer_i(60*10)//队列日志定时器间隔，单位秒
    {
		init_socket();
        do_connect();
        log_timer();
    }
	~client_loop(){
		delete socket_;
	}

	int write(const char *msg, int msgL)
	{
		//std::cout<<socket_->is_open()<<clietn_status<<std::endl;
		//新建一个发送发送msg，　在发送动作完成时删除，不进行状态更新检测。结果由接收端进行超时删除。
		//和一个接收msg　buff，　在取走或超时时删除
		RPCStruct_ptr tmsg = newMsgBuf(msg,msgL);
		io_service_.post(boost::bind(&client_loop::do_write, this, tmsg)); //将消息主动投递给io_service
		return tmsg->head().req_id();
	}
	//读取回复请求消息
	RPCStruct_ptr read(int reqid){

		if(recv_msg_map_.find(reqid) == recv_msg_map_.end()){
			//不太可能发生，
			std::cout<<"cal read error reqid="<<reqid<<std::endl;
			return nullptr;
		}

		RPCStruct_ptr res =recv_msg_map_[reqid];
		std::cout<<"cal read stat reqid="<<res->head().req_id()<<" "<<reqid<<" s="<<res->get_status()<<std::endl;

		//监听状态
		while(true){
			switch(res->get_status()){
				case CT_ERROR:
				case CT_READ_OK:
				case CT_TIMEOUT:
					//设置删除状态标记,reqid添加到删除队列
					res->erase_flag.store(CT_ERASE_STATE, std::memory_order_release);
					std::cout<<"cal read ok reqid="<<reqid<<std::endl;

					del_reqid_list.push(res->head().req_id());
					return res;
				default:
					std::this_thread::sleep_for(std::chrono::milliseconds (1));
					//使用boost sleep时不同版本有问题，编译不过
					//boost::this_thread::sleep(boost::posix_time::milliseconds(1));
					break;
			}
		}
		std::cout<<"cal read ok reqid="<<reqid<<std::endl;
		return res;
	}

    void close(){
        io_service_.post(boost::bind(&client_loop::do_close, this)); //这个close函数是客户端要主动终止时调用  do_close函数是从服务器端
                                                                //读数据失败时调用
    }
	
    bool is_open(){
		bool res = socket_->is_open();
		if(false == res)
			do_connect();
		return res;
    }



private:
	void init_socket(){
		socket_=new tcp::socket(io_service_);
	}
	void reset_socket(){
		if(socket_)
			delete socket_;
		socket_=new tcp::socket(io_service_);
	}

    void do_connect(){
		if(clietn_status == CT_CONN)return;
		clietn_status=CT_CONN;
		socket_->async_connect(
			boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(ip_), port_),
			boost::bind(&client_loop::handle_connect,
				this,
				boost::asio::placeholders::error)); //所有的操作都采用异步的方式

    }
    void handle_connect(const boost::system::error_code& error)
    {
        if (!error)
        {
			clietn_status=CT_CONN_OK;
            do_read_head();
            erase_msg_timer();
        }else{
			clietn_status=CT_ERROR;
			//链接异常后，不进行处理，则client socket是开启状态，需要关闭再新建链接
			reset_socket();
			std::cout<<"cal handle_connect ["<<error_msg(error.value())<<"]"<<std::endl;
        }
    }

    void do_read_head(){
		if(clietn_status != CT_CONN_OK){
			//return;
		}
		//设置临时头 recvHead_ptr 类型是共享智能指针类型，接收头数据后，查找body　map 释放
		recvHead_ptr t(new RPCStructHead());
			boost::asio::async_read(*socket_,
				boost::asio::buffer(t->data(), t->data_len()), //读取数据报头
				boost::bind(&client_loop::handle_read_header,
					this,
					t,
					boost::asio::placeholders::error));
    }
	
    void handle_read_header(recvHead_ptr head, const boost::system::error_code& error){
		if(!is_open())return;
		if (!error) //分别处理数据报头和数据部分
        {
			//接收map中找到
			if(recv_msg_map_.find(head->req_id()) != recv_msg_map_.end()){
				//未超时	 //超时 都需要接收完body内容
                //更新map头部内容
                recv_msg_map_[head->req_id()]->head(*head.get());
                //异步写入body数据
                do_read_msg(recv_msg_map_[head->req_id()]);
			}else{
			//接收map中未找到
				RPCStruct_ptr t_recv(new RPCStruct(io_service_, *head.get()));
				do_read_msg(t_recv);
			}

        }
        else
        {
            std::cout<<"cal handle_read_header ["<<error_msg(error.value())<<"]"<<std::endl;
            do_close();
        }

    }

    void do_read_msg(RPCStruct_ptr read_msg_){
		if(clietn_status != CT_CONN_OK){
			//return;
		}

        boost::asio::async_read(*socket_,
            boost::asio::buffer(read_msg_->recv_msg_buff(), read_msg_->recv_msg_len()),//读取数据包数据部分
            boost::bind(&client_loop::handle_read_msg, this,
            read_msg_,
            boost::asio::placeholders::error));

    }

    void handle_read_msg(RPCStruct_ptr read_msg_, const boost::system::error_code& error){
		if(!is_open())return;
        if (!error)
        {
			//第一部分，处理接收回调函数
            std::cout<<"cal handle_read_msg recv ｜ "<<read_msg_->head().req_id()<<" "<<read_msg_->body()<<std::endl;
            //更新读取状态；
            read_msg_->set_status(CT_READ_OK);

            //第二部分，完成一次读操作（处理完一个数据包）  进行下一次读操作
            do_read_head();

        }
        else
        {
            read_msg_->set_status(CT_ERROR);
            std::cout<<"cal handle_read_msg "<<" ["<<error_msg(error.value())<<"]"<<std::endl;
            do_close();
        }
    }

	//do_write函数只管发，只发一次，发送失败时则不处理，由接收进行超时设置
    void do_write(RPCStruct_ptr res)
    {
 		if(clietn_status != CT_CONN_OK){
			return;
		}
        // if(write_msg_queue_.size())
        {
            //这里如果，消息队列不为空，则只添加消息不启动写队列，因为有一个正在写
            //消息队列为空，则添加消息后，启动一个写操作
            //RPCStruct_ptr res = write_msg_queue_.pop();
            if(res){
                boost::asio::async_write(*socket_,
                      boost::asio::buffer(res->data(), res->data_len()),
                      boost::bind(&client_loop::handle_write,
                        this,
                        boost::asio::placeholders::error));

            }
        }
    }

    void handle_write(const boost::system::error_code& error)
    {
		if(!is_open())return;
        if (!error)
        {
            // do_write();
        }
        else
        {
            std::cout<<"cal handle_write "<<" ["<<error_msg(error.value())<<"]"<<std::endl;
            do_close();
        }
    }
    //完成发送消息队列和接收消息map新建
    RPCStruct_ptr newMsgBuf(const char *msg, int msgL){
        RPCStruct_ptr res(new RPCStruct(io_service_));
        RPCStructHead head;
        head.req_id(msg_index_++);
        head.body_len(msgL);
        res->head(head);
        res->body(msg);
        // write_msg_queue_.push(res);


        RPCStruct_ptr recv(new RPCStruct(io_service_));
        recv->do_init_timer();
		//必须设置头部 请求id
		//因为可能在异步读取时，在没写完成时，被先取到头reqid等待
		recv->head().req_id(head.req_id());
        recv_msg_map_[head.req_id()]=recv;
        return res;
    }

    void do_close()
    {
		clietn_status=CT_CONN_CLOSE;
        //发生异常后关闭　client，当再次使用时，会请请求链接
        socket_->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        socket_->cancel();
		socket_->close();
    }


    //定时器

    //每３秒清理接收map，设置为删除状态，再次３秒过后进行删除
    void erase_msg_timer(){
        //std::cout<<"cal  erase_msg_timer"<<std::endl;

        for (int i=0; i<del_reqid_list.size(); ++i){
            int reqid = del_reqid_list.pop();
            if(recv_msg_map_.find(reqid) == recv_msg_map_.end())
                continue;

            if(recv_msg_map_[reqid]->erase_flag== CT_ERASE_EXEC){
                recv_msg_map_.erase(reqid);
                continue;
            }

            if(recv_msg_map_[reqid]->erase_flag == CT_ERASE_STATE){
                recv_msg_map_[reqid]->erase_flag = CT_ERASE_EXEC;
            }
            del_reqid_list.push(reqid);

        }
        //初始化定时器
        //e_timer.expires_at(e_timer.expires_at() + boost::posix_time::seconds(3));
        e_timer.expires_from_now(boost::posix_time::seconds(e_timer_i));
        e_timer.async_wait(boost::bind(&client_loop::erase_msg_timer, this));
    }

    //10分钟　输出一次，一天8640条记录
    void log_timer(){
        // std::cout<<"cal "<<ip_<<":"<<port_<<" | send queue size: "<<write_msg_queue_.size()<<endl;
        std::cout<<"cal "<<ip_<<":"<<port_<<" | recv map size: "<<recv_msg_map_.size()<<endl;
        //初始化定时器
        l_timer.expires_from_now(boost::posix_time::seconds(l_timer_i));
        l_timer.async_wait(boost::bind(&client_loop::log_timer, this));
    }
	
	void set_etimeri(int v){e_timer_i=v;}
	void set_ltimeri(int v){l_timer_i=v;}

};



#endif // CLIENT_ASYNC_LOOP_H
