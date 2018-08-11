#ifndef CLIENT_ASYNC_LOOP_POLL_H
#define CLIENT_ASYNC_LOOP_POLL_H

#include <vector>
#include <future>
#include "client_msg.hpp"
#include "client_async_loop.h"
#include "client_error.h"

using namespace std;

class client_loop_pool{
    typedef std::shared_ptr<boost::asio::io_service> io_service_ptr;
    typedef std::shared_ptr<boost::thread> thread_ptr;
    using work_ptr = std::unique_ptr<boost::asio::io_service::work>;
    using client_ptr = std::unique_ptr<client_loop>;

    //ios list,每一个socket都的一个独立的io服务
    std::vector<io_service_ptr> io_service_list;
    //启动io服务线程
    std::vector<thread_ptr> thread_list;
    //在无操作时，防止io服务退出
    std::vector<work_ptr> work_list;
    //client　socket  类
    std::vector<client_ptr> client_list;


    //当前链接数，和ip port hash 集合
    const int client_max_num=10;
    std::set<int> link_set_poll;
    //当前client计数
    std::atomic_int cur_client_index=ATOMIC_VAR_INIT(0);
    //上次选择的client
    std::atomic_int select_index=ATOMIC_VAR_INIT(0);


    // 加锁和解锁使用作用
    typedef boost::shared_lock<boost::shared_mutex> read_lock;
    typedef boost::unique_lock<boost::shared_mutex> write_lock;
    boost::shared_mutex read_write_mutex;


public:
    client_loop_pool(){}
    ~client_loop_pool(){

        //停止io_server
        for (client_ptr &client: client_list)
        {
            client->close();
        }

		//停止io_server
        for (work_ptr &work: work_list)
        {
            work.reset();
        }
        //停止io_server
        for (io_service_ptr &ios_p: io_service_list)
        {
            ios_p->stop();
        }
		
        //等待线程退出
        for (thread_ptr &t: thread_list)
        {
            t->join();
			std::cout<<"pool join"<<std::endl;
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
        std::future<RPCStruct_ptr > f2 = std::async(std::launch::async, boost::bind(&client_loop::read, client_list[ch.i].get(), ch.r));
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

private:
    int BKDR_hash(const string str){
        register int hash = 0;
        for(size_t i=0; i<str.size(); ++i)
        {
            hash = hash * 131 + str[i];   // 也可以乘以31、131、1313、13131、131313..
        }
        return hash;
    }

    //新的请求查看是否需要新建socket
    //新建一个socket请求
    void add_link(string ip, int port){

        if(cur_client_index > client_max_num){
            //当前client　数目已经到达最大数，不在添加新的
            return;
        }

        {
            //加写锁
            write_lock rlock(read_write_mutex);

            auto result_1 = link_set_poll.insert(BKDR_hash(ip)+port);
            //insert 成功返回值为1,需要新建link,
            //未成功时，insert 返回值为0 集合中已经存在，不需要再新建
            if(result_1.second){
                io_service_ptr ios_t(new boost::asio::io_service());

                io_service_list.push_back(ios_t);
                client_list.push_back(client_ptr( new client_loop(*ios_t.get(), ip,port)));
                //设置work
                work_list.push_back(work_ptr(new boost::asio::io_service::work(*(ios_t.get()))));
                //启动io_server
                thread_list.push_back(thread_ptr(new boost::thread(boost::bind(&boost::asio::io_service::run, ios_t.get()))));
                //thread_list.push_back(thread_ptr(new boost::thread(boost::bind(&boost::asio::io_service::run, io_service_list.back().get()))));

                ++cur_client_index;

            }
        }
    }

    int get_client_index(){
		int index=-1;
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
