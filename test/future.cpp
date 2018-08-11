//boost future

// #define BOOST_THREAD_PROVIDES_FUTURE
// #include <boost/thread.hpp>
// #include <boost/thread/future.hpp>
// #include <iostream>

// int accumulate()
// {
  // int sum = 0;
  // for (int i = 0; i < 5; ++i)
    // sum += i;
  // return sum;
// }

// int main()
// {
  // boost::future<int> f = boost::async(accumulate);
  // std::cout << f.get() << '\n';
// }


//https://blog.csdn.net/GW569453350game/article/details/50435879
#include <iostream>
#include <string>
#include <future>         // std::promise, std::future, std::async
#include <vector>
#include<boost/bind.hpp>
using namespace std;



class acc{
public:
	int accumulate()
	{
	  int sum = 0;
	  for (int i = 0; i < 5; ++i)
		sum += i;
	  return sum;
	}
	
	string getstr(string para)
	{ 
				cout<<para<<endl;
				return std::string("hello, world");  // 创建线程， 设定 promise, 返回一个 future。区别于 std::launch::deferred（不会创建新的线程去执行该lambda函数，其实最好是使用 deferred 策略，否则可能出现内存问题，see link: https://svn.boost.org/trac/boost/ticket/12220）
	}
};

int main()
{
    // 注意，如果不用一个变量f1保存返回的future,则默认构造的future会马上被销毁，销毁过程中会阻塞等待线程被执行完。
    // std::future<std::string> f1 = std::async(std::launch::async, [](){ 
            // return std::string("hello, world");  // 创建线程， 设定 promise, 返回一个 future。区别于 std::launch::deferred（不会创建新的线程去执行该lambda函数，其实最好是使用 deferred 策略，否则可能出现内存问题，see link: https://svn.boost.org/trac/boost/ticket/12220）
        // }); 
	acc fa;
    std::future<std::string> f1 = std::async(std::launch::async, boost::bind(&acc::getstr, &fa, "ssss")); 
    std::vector< std::future<std::string> > vf;
    vf.push_back(std::move(f1));  // future不能够被复制，但是可以被move
    cout<<vf[0].get()<<endl; // get 方法阻塞，直到执行完成并返回结果，output: hello, world

    std::future<int> f2 = std::async(std::launch::async, [](){ 
            cout<<8<<endl;
            return 8;
        }); 

    f2.wait(); //wait 方法等待执行完成，不返回结果

    std::future<int> future = std::async(std::launch::async, [](){ 
            std::this_thread::sleep_for(std::chrono::seconds(3));
            return 8;  
        }); 

    std::cout << "waiting...\n";
    std::future_status status;
    do {
        status = future.wait_for(std::chrono::seconds(1));
        if (status == std::future_status::deferred) {
            std::cout << "deferred\n";
        } else if (status == std::future_status::timeout) {
            std::cout << "timeout\n";
        } else if (status == std::future_status::ready) {
            std::cout << "ready!\n";
        }
    } while (status != std::future_status::ready); 

    std::cout << "result is " << future.get() << '\n';
}