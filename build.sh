#bin 

rm build -rf
mkdir -p build
cd build
cmake ../
make
cd -




# g++  -Wall -g -o c  client.cpp client_error.cpp -lboost_system -lboost_filesystem -lboost_thread 
# 2>info.txt

# g++  -Wall -g -o p  client_pool.cpp client_error.cpp -lboost_system -lboost_filesystem -lboost_thread -lpthread

# g++  -Wall -g -o s  server.cpp  client_error.cpp -lboost_system -lboost_filesystem -lboost_thread -lpthread

# g++  -Wall -g -o cnq  client_no_queue.cpp  client_error.cpp -lboost_system -lboost_filesystem -lboost_thread 


# 二 配置操作系统使其产生core文件

# 首先通过ulimit命令查看一下系统是否配置支持了dump core的功能。通过ulimit -c或ulimit -a，可以查看core file大小的配置情况，如果为0，则表示系统关闭了dump core。可以通过ulimit -c unlimited来打开。若发生了段错误，但没有core dump，是由于系统禁止core文件的生成。

# 解决方法:
# $ulimit -c unlimited　　（只对当前shell进程有效）
# 或在~/.bashrc　的最后加入： ulimit -c unlimited （一劳永逸

#gdb [exec file] [core file]

# 如: gdb ./test test.core

# gdb>bt

# 这样，一般就可以看到出错的代码是哪一句了，还可以打印出相应变量的数值，进行进一步分析。
# 1、程序运行参数。 
# set args 可指定运行时参数。（如：set args 10 20 30 40 50） 
# show args 命令可以查看设置好的运行参数。 

# gdb>print 变量名

# 之后，就全看各位自己的编程功力与经验了，gdb已经做了很多了。

# 2. 对于结构复杂的程序，如涉及模板类及复杂的调用，gdb得出了出错位置，似乎这还不够，这时候要使用更为专业的工具——valgrind。

# valgrind是一款专门用作内存调试，内存泄露检测的开源工具软件，valgrind这个名字取自北欧神话英灵殿的入口，不过，不能不承认，它确实是Linux下做内存调用分析的神器。一般Linux系统上应该没有自带valgrind，需要自行进行下载安装。

# 下载地址：http://valgrind.org/downloads/current.html

# 进入下载文件夹，分别执行(需要root权限，且必须按默认路径安装，否则有加载错误)：

# ./configure

# make

# make install

# 安装成功后，使用类似如下命令启动程序：

# valgrind --tool=memcheck --leak-check=full --track-origins=yes --leak-resolution=high --show-reachable=yes --log-file=memchecklog ./controller_test