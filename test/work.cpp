// g++ work.cpp -lboost_system -lboost_filesystem -lboost_thread 

#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

int main(){
	
	using namespace boost;
	using namespace boost::asio;
	io_service ios;
	int thread_num = 1;
	thread *t[thread_num] = {0};
	
	//启动work,　ios中没有任务不会退出，注释掉的话会立刻退出　使用work.reset()来结束其生命周期
	io_service::work work(ios);
	
	
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
