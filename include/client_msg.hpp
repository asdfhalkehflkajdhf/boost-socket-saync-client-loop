#ifndef CLIENT_MSG_HPP
#define CLIENT_MSG_HPP

#include <iostream>
#include <assert.h>
#include <atomic>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

//client 结果handle
class clientHandle{
public:
    int i;//socket index
    int r;//req_id index
    clientHandle(int i1,int r1):i(i1),r(r1){}
    clientHandle():i(-1), r(-1){}
};

enum client_status_type{
    CT_WRITE,
    CT_WRITE_OK,
    CT_READ,
    CT_READ_OK,
    CT_TIMEOUT,
    CT_ERASE_STATE,//map 设置为销毁状态 5
    CT_ERASE_EXEC,
    CT_CONN,
	CT_CONN_CLOSE,
    CT_CONN_OK,
    CT_ERROR,//10
    CT_OK,
    CT_UNKNOWN
};

struct RPC_MSG_HEAD{
	int req_id;
    int body_size;
	int meta_size;
};

struct RPCStructHead{
	RPC_MSG_HEAD head;
    //client_status_type status=CT_UNKNOWN;
    RPCStructHead(){
		head.req_id=-1;
		head.body_size=-1;
		head.meta_size=8;
	}
	
	char *data(){return (char *)&head;}
	int data_len(){return sizeof(RPC_MSG_HEAD);}
	
	int req_id(){return head.req_id;}
	void req_id(int v){head.req_id=v;}
	
	int body_len(){return head.body_size;}
	void body_len(int v){head.body_size=v;}
	
	int meta_len(){return head.meta_size;}
	void meta_len(int v){head.meta_size=v;}
};

class RPCStruct{
    RPCStructHead head_info;
    const int head_size;
	const int head_offset;
	const int meta_offset;
	const int body_offset;

    client_status_type status;
    char *_data;
	//预分配大小，当body大于pre_body_size进行重申请
	int pre_body_size;

	//数据通信超时定时器，从创建到接收完成
    boost::asio::deadline_timer timer;
	
    //分配 msg buffer，只在更新头部数据和获取body数据地址时进行检查分配
	//realloc 分配第一个参数，为空时会分配空间失败，也不一定，不同编译器有不同实现
    void new_data(){
		
		try{
			if(head_info.body_len() > pre_body_size){
				//更新body空间
				pre_body_size = head_info.body_len();
				if(NULL == _data){
					//第一次申请空间
					_data = (char *)malloc(body_offset + pre_body_size);
					//_data=new char[head_info.body_size+head_size];
				}else{
					_data = (char *)realloc(_data, body_offset + pre_body_size);
				}
			}else{
				//不需要更新空间
				if(NULL == _data){
					//第一次申请空间
					_data = (char *)malloc(body_offset + pre_body_size);
					//_data=new char[head_info.body_size+head_size];
				}
			}
			
		
			assert(_data!=NULL);
			//写头部数据
			memcpy(_data, head_info.data(), head_size);
		}
		catch (std::exception& e)
		{
			std::cerr << "Exception: " << e.what() << "\n";
			exit(1);
		}

    }

	void set_timeout(const boost::system::error_code& error){
		if(!error){
			//超时重新设置异步超时回
			set_status(CT_TIMEOUT);
		}
	}

public:
    //删除标记,
    std::atomic<client_status_type> erase_flag=ATOMIC_VAR_INIT(CT_UNKNOWN);

    // RPCStruct(int pbs=1024*2)
		// : head_size(head_info.data_len()),
		// head_offset(0), meta_offset(head_offset + head_size), body_offset(meta_offset + head_info.meta_len()),
		// status(CT_UNKNOWN), _data(NULL), pre_body_size(pbs){}
		
    RPCStruct(boost::asio::io_service &io_service_,int pbs=1024*2)
		: head_size(head_info.data_len()),
		head_offset(0), meta_offset(head_offset + head_size), body_offset(meta_offset + head_info.meta_len()),
		status(CT_UNKNOWN), _data(NULL), pre_body_size(pbs),
		timer(io_service_){}
		
    RPCStruct(boost::asio::io_service &io_service_, RPCStructHead rt,int pbs=1024*2)
    	:head_info(rt),
    	head_size(head_info.data_len()), 
		head_offset(0), meta_offset(head_offset + head_size), body_offset(meta_offset + head_info.meta_len()),
    	status(CT_UNKNOWN), _data(NULL), pre_body_size(pbs),    	
		timer(io_service_){}
		
    ~RPCStruct(){
		do_close_timer();
        if(_data){
            //std::cout<<"~RPCStruct reqid="<<head_info.req_id<<" "<<this<<" |"<<body()<<std::endl;
            free(_data);
			//delete _data;
        }
        _data=NULL;
    }
	
	void do_init_timer(int time=80){
		timer.expires_at(boost::posix_time::pos_infin);
		timer.expires_from_now(boost::posix_time::milliseconds(time));
		timer.async_wait(boost::bind(&RPCStruct::set_timeout, this,boost::asio::placeholders::error));
	}
	
	void do_close_timer(){
		timer.cancel();
	}
	
    void head(RPCStructHead rt){
        head_info = rt;
        new_data();
    }

    RPCStructHead &head(){
        return head_info;
    }

	void body(const char *msg){
        //初始body空间
        new_data();
		std::cout<<"msg set body offset["<<body_offset<<"] len["<<head_info.body_len()<<"]"<<std::endl;
        memcpy(_data+body_offset, msg, head_info.body_len());
    }
    char *body(){
        new_data();
        return _data+body_offset;
    }
	
    //注意，在使用这前，必须设置　头结构或是body大小。
    char *data(){
        assert(_data!=NULL);
        return _data;
    }
    int data_len(){
		//最后一个字段的偏移量+字段大小
        return head_info.body_len()+body_offset;
    }

	char *meta(){
        new_data();
        return _data+meta_offset;
	}
	void meta(char *msg ){
        new_data();
        memcpy(_data+meta_offset, msg, head_info.meta_len());
	}
	
	//读取msg部分，除于头以处的所有数据
	char *recv_msg_buff(){
		new_data();
		return _data+meta_offset;
	}
	int recv_msg_len(){
		return head_info.body_len()+head_info.meta_len();
	}
	
    void set_status(client_status_type v){
		//更新状态，说明消息数据已经有结果，则关闭超时定时器
		do_close_timer();
        status = v;
    }
    client_status_type get_status(){
        return status;
    }
	
	void showInfo(){
		RPC_MSG_HEAD *head=(RPC_MSG_HEAD *)_data;
		std::cout<<"\tr["<<head->req_id<<"] b["<<head->body_size<<"] m["<<head->meta_size<<"]"<<std::endl;
		std::cout<<"\tm["<<_data+sizeof(RPC_MSG_HEAD)<<"]"<<std::endl;
		std::cout<<"\tb["<<_data+ sizeof(RPC_MSG_HEAD) +head->meta_size<<"]"<<std::endl;
		std::cout<<"\t hs["<<head_size<<"] ho["<<head_offset<<"] mo["<<meta_offset<<"] bo["<<body_offset<<"]"<<std::endl;
	}

};


#endif // CLIENT_MSG_HPP
