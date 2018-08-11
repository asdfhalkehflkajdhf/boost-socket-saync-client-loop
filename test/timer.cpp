// http://zh.highscore.de/cpp/boost/asio.html
#include <boost/asio.hpp> 
#include <iostream> 
  boost::asio::io_service io_service; 
  boost::asio::deadline_timer timer1(io_service, boost::posix_time::seconds(5)); 
  boost::asio::deadline_timer timer2(io_service, boost::posix_time::seconds(10)); 

  //一个io_service上可以挂载多个定时器是没有问题的
  
void handler1(const boost::system::error_code &ec) 
{ 
	std::cout << "2 s." << std::endl; 
	timer1.expires_from_now(boost::posix_time::seconds(2));
	timer1.async_wait(handler1); 

} 

void handler2(const boost::system::error_code &ec) 
{ 
	std::cout << "10 s." << std::endl; 
  	timer2.expires_from_now(boost::posix_time::seconds(10));
	timer2.async_wait(handler2); 

} 

int main() 
{ 
  timer1.async_wait(handler1); 
  timer2.async_wait(handler2); 
  io_service.run(); 
}