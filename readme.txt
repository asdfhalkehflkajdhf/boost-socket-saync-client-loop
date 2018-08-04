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
		
client.cpp
	client_async_loop 测试
		
client_async_loop_poll.h
	对client_async_loop可行池化
client_poll.cpp
	client_async_loop_poll 测试
		


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

	3、client只有增加链接操作，没有删除链接操作。
	4、增加链接方式只有动态增长方式。
	5、没有添加配置方式，使用参数可调整。



