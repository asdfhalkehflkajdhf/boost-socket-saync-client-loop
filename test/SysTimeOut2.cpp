// g++ work.cpp -lboost_system -lboost_filesystem -lboost_thread 

#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>



using namespace boost;
using namespace boost::asio;
using boost::asio::deadline_timer;


char buf[1024];
	io_service ios;
	boost::asio::ip::tcp::socket socket_(ios);
deadline_timer output_deadline_(ios);
 extern void do_timeout();

  bool stopped()
  {
    return !socket_.is_open();
  }

void handle_read(const boost::system::error_code& error)
{
	if (!error)
	{
		std::cout<<"read ok"<<std::endl;
	}else{
		if(error.value() == boost::system::errc::operation_canceled){
			std::cout<<"read timeout "<<error.value()<<std::endl;
		}
	}
}
void do_read(){

		boost::asio::async_read(socket_,
				boost::asio::buffer(buf, 1024), //读取数据报头
				boost::bind(&handle_read,
					boost::asio::placeholders::error));


}

void check_deadline(deadline_timer* deadline, const boost::system::error_code& error)
{
	if (stopped())
	  return;

  	if (error)
	{
		std::cout<<"check_deadline "<<error.message()<<std::endl;
	}

  
	// Check whether the deadline has passed. We compare the deadline against
	// the current time since a new asynchronous operation may have moved the
	// deadline before this actor had a chance to run.
	if (deadline->expires_at() <= deadline_timer::traits_type::now())
	{
		//超时重新设置异步超时回
	  // The deadline has passed. Stop the session. The other actors will
	  // terminate as soon as possible.
	  socket_.cancel();
	  do_read();
		do_timeout();
	  std::cout<<"timeout"<<std::endl;
	}
	else
	{
	  // Put the actor back to sleep.
	  deadline->async_wait(
		  boost::bind(check_deadline,
		  deadline,
		  boost::asio::placeholders::error));
	}
}
 
 void do_timeout(){
	 		output_deadline_.expires_from_now(boost::posix_time::seconds(1));

	    output_deadline_.async_wait(
        boost::bind(&check_deadline,
        &output_deadline_, 
		boost::asio::placeholders::error));
}


void handle_connect(const boost::system::error_code& error)
{
	if (!error)
	{
		std::cout<<"conn ok"<<std::endl;
		do_read();
		do_timeout();
		
		
	}else{
		std::cout<<"conn "<<error.message()<<std::endl;
	}
}




int main(){
	

	//设置超时时间
	
	//设置为pos_infin 。这可确保输出actor保持
	output_deadline_.expires_at(boost::posix_time::pos_infin);
	
	
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
