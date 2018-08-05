１、编译
	环境centos 7
	执行build.sh生成　build目录。
	可执行文件，
		s		服务器
		c		单线程有队列客户端（使用参数y/n来指定是否使用发送消息队列）
		p		多线程，池客户端
	

2
	
server_async_loop.h
	多线程异步接收服务端
server.cpp
	server_async_loop 服务启动
	
	
client_async_loop.h
	循环线程异步客户端类
		实现方式，使用的是消息队列，同一时刻只有一个读写
		未使用的是消息队列，而是使用异步写队列，同一时刻只有一个读多个写

		对于socket链接出错可以比较方便处理 ，消息还在可以重新链接发送
		
		1、设置删除时间间隔，单位秒
			client_loop::set_etimeri
		2、设置日志输出时间间隔，单位分
			client_loop::set_ltimeri
		3、请求发发送消息.	//返回请求id 用于区分发送的是那个消息，用于接收(reqid)
			int write(const char *msg, int msgL, int timeout=80)
		4、请求接收消息
			RPCStruct_ptr read(int reqid)
		
client.cpp
	client_async_loop 测试
		
client_async_loop_pool.h
	对client_async_loop可行池化
		1、设置最大客户端数
			client_loop_pool::set_c_max_num
		2、设置消息队列方式
			client_loop_pool::set_csq
		3、删除链接操作：注意，没有进行过测试
			void destroy(string ip, int port)
client_poll.cpp
	client_async_loop_poll 测试
		


3、依赖库
	boost
	libconfig

	yum install boost*
	
＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
4、


		

rpc client  存在问题：

	4、增加链接方式只有动态增长方式。
	5、没有添加配置方式，使用参数可调整。



