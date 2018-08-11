１、编译
	环境centos 7
	执行build.sh生成　build目录。
	可执行文件，
		s		服务器
		c		单线程有队列客户端
		cnq		单线程无队列客户端
		p		多线程，池客户端
	

2
	
server_async_loop.h
	多线程异步接收服务端
server.cpp
	server_async_loop 服务启动
	
	
client_async_loop.h
	循环线程异步客户端类
		实现方式，使用的是消息队列，同一时刻只有一个读写
		
		对于socket链接出错可以比较方便处理 ，消息还在可以重新链接发送
		
client.cpp
	client_async_loop 测试
		
client_async_loop_poll.h
	对client_async_loop可行池化
client_poll.h
	client_async_loop_poll 测试
		
		
client_async_loop_no_queue.h
	循环线程异步客户端类
		实现方式，未使用的是消息队列，而是使用异步写队列，同一时刻只有一个读多个写

		对于socket链接出错不是很好处理，有一个中断，后继所有的会异步或必须丢失


3、依赖库
	boost
	libconfig

	yum install boost*
	
＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
4、设置最大客户端数
	client_loop_pool
    		//当前链接数，和ip port hash 集合
    		const int client_max_num=10;
		释放接收数据时间间隔 6秒
		

rpc client  存在问题：
	1、socket error 104, “服务器过早终止”错误	server terminated prematurely。经常是一端异常中断发送RST，另一端仍然有读写操作，会产生SIGPIPE信号，该信号的默认行为是终止进程。在捕获或是忽略信息时，第二次写操作都将返回EPIPE错误。https://blog.csdn.net/terence1212/article/details/51881659
		没有对中断信号进行处理
	
	2、client没有对异常或正常退出时遗留数据进行处理。
	3、client只有增加链接操作，没有删除链接操作。
	4、增加链接方式只有动态增长方式。
	5、没有添加配置方式，使用参数可调整。
