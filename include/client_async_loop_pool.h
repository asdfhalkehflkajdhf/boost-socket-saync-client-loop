#ifndef CLIENT_ASYNC_LOOP_POLL_H
#define CLIENT_ASYNC_LOOP_POLL_H

#include <vector>
#include <future>
#include "client_msg.hpp"
#include "client_async_loop.h"
#include "client_error.h"

using namespace std;

class client_loop_pool{
	class pool_client_{
		using io_service_ptr = std::shared_ptr<boost::asio::io_service> ;
		using thread_ptr = std::shared_ptr<boost::thread> ;
		using work_ptr = std::unique_ptr<boost::asio::io_service::work>;
		using client_ptr = std::unique_ptr<client_loop>;

		//启动io服务线程
		io_service_ptr io_service_;
		//在无操作时，防止io服务退出
		work_ptr work_;
		//client　socket  类
		client_ptr client_;
		thread_ptr tid_;

		//防止多次销毁
		bool destroy_=false;
		public:
			int hash_key;

			pool_client_(string ip, int port): io_service_(new boost::asio::io_service()), 
				work_(new boost::asio::io_service::work(*(io_service_.get()))),
				client_( new client_loop(*io_service_.get(), ip,port)),
				tid_(new boost::thread(boost::bind(&boost::asio::io_service::run, io_service_.get()))),
				hash_key(client_loop_pool::BKDR_hash(ip)+port)
			{
			}
			
			~pool_client_(){
				destroy();
			}
			
			void destroy(){
				if(destroy_)return;
				destroy_=true;
				//停止io_server
				client_->destroy();
			
				//停止io_server
				work_.reset();

				//停止io_server
				io_service_->stop();
				
				//等待线程退出
				tid_->join();
			}
			
			int write(const char *msg, int msgL, int timeout=80){
				return client_->write(msg, msgL, timeout);
			}
			
			client_loop *read(){
				return client_.get();
			}
			
			bool is_open(){return client_->is_open();}
			
	};
    //ios list,每一个socket都的一个独立的io服务
	using pool_client_ptr = std::shared_ptr<pool_client_> ;
    std::vector<pool_client_ptr> client_list;

    //当前链接数，和ip port hash 集合
    int client_max_num=10;
    std::set<int> link_set_poll;
    //当前client计数
    std::atomic_int cur_client_index=ATOMIC_VAR_INIT(0);
    //上次选择的client
    std::atomic_int select_index=ATOMIC_VAR_INIT(0);


    // 加锁和解锁使用作用
    typedef boost::shared_lock<boost::shared_mutex> read_lock;
    typedef boost::unique_lock<boost::shared_mutex> write_lock;
    boost::shared_mutex read_write_mutex;

	// 设置clinet方式
	client_send_queue csq_=client_send_queue::CSQ_ON;
public:
    client_loop_pool(){}
    ~client_loop_pool(){
		destroy();
    }
	
	void destroy(){
		//停止io_server
		while(client_list.size()){
			client_list[0]->destroy();
			client_list.erase(client_list.begin());
		}
	}
	
	void destroy(string ip, int port){
		int hash_key = BKDR_hash(ip)+port;
		//加写锁
		write_lock rlock(read_write_mutex);
		for(int i=0;i<client_list.size();++i){
			if(client_list[i]->hash_key == hash_key){
				//客户端释放需要一定时间，需要起一个线程单独处理，减少对主线程调试的影响
				boost::thread t([](pool_client_ptr client) { client->destroy(); }, client_list[i]);
				
				//client_list[i]->destroy();
				client_list.erase(client_list.begin()+i);
				link_set_poll.erase(hash_key);
				return;
			}
		}
	}
	
    //发送前检查,-1 时，没有可用链接
    clientHandle write(string ip, int port, char*sendData, int sendSize){
        clientHandle res;
        //0、检查是否为新链接
        add_link(ip, port);

        //1、获取socketid
        res.i = get_client_index();

        if(res.i>=0){
            res.r=client_list[res.i]->write(sendData, sendSize);
        }

        std::cout<<"calp write getClientIndex i="<<res.i<<" r="<<res.r<<std::endl;

        return res;
    }
    char *read(clientHandle ch){
        if(ch.i==-1){
            return NULL;
        }
        std::future<RPCStruct_ptr > f2 = std::async(std::launch::async, boost::bind(&client_loop::read, client_list[ch.i]->read(), ch.r));
        RPCStruct_ptr res = f2.get();
        if(res == nullptr || res->get_status() != CT_READ_OK){
            return nullptr;
        }
        return res->body();
    }
    static client_loop_pool& get_instance(){
        static client_loop_pool instance;
        return instance;
    }

	void set_csq(bool v){
		if(v)
			csq_=client_send_queue::CSQ_ON;
		else
			csq_=client_send_queue::CSQ_OFF;
	}
	static int BKDR_hash(const string str){
        register int hash = 0;
        for(size_t i=0; i<str.size(); ++i)
        {
            hash = hash * 131 + str[i];   // 也可以乘以31、131、1313、13131、131313..
        }
        return hash;
    }

	void set_c_max_num(int v){
		if(client_list.size()<v)
			client_max_num=v;
	}
private:

    //新的请求查看是否需要新建socket
    //新建一个socket请求
    void add_link(string ip, int port){

        if(cur_client_index > client_max_num){
            //当前client　数目已经到达最大数，不在添加新的
            return;
        }

		auto result_1 = link_set_poll.insert(BKDR_hash(ip)+port);
		//insert 成功返回值为1,需要新建link,
		//未成功时，insert 返回值为0 集合中已经存在，不需要再新建
		if(result_1.second){
			//加写锁
			write_lock rlock(read_write_mutex);
			client_list.push_back(pool_client_ptr( new pool_client_(ip,port)));
			++cur_client_index;
		}
    }

    int get_client_index(){
		int index=-1;
		
		read_lock rlock(read_write_mutex);
        for(int i=0; i<cur_client_index; ++i){
            index = (select_index++)%cur_client_index;
            std::cout<<"calp select index "<<index<<std::endl;
            if(client_list[index]->is_open()){
                std::cout<<"select index is open"<<std::endl;
                return index;
            }
            std::cout<<"calp select index is close"<<std::endl;

        }
        return index;
    }

};

#endif // CLIENT_ASYNC_LOOP_POLL_H
