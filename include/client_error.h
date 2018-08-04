#ifndef CLIENT_ERROR_H
#define CLIENT_ERROR_H
 

 // 1 #define EPERM 1    // Operation not permitted 操作不允许
  // 2 #define ENOENT 2  // No such file or directory 文件/路径不存在
  // 3 #define ESRCH 3    // No such process 进程不存在
  // 4 #define EINTR 4    //  Interrupted system call 中断的系统调用
  // 5 #define EIO 5 // I/O error I/O错误
  // 6 #define ENXIO 6 // No such device or address 设备/地址不存在
  // 7 #define E2BIG 7 // Arg list too long 参数列表过长
  // 8 #define ENOEXEC 8 // Exec format error 执行格式错误
  // 9 #define EBADF 9 // Bad file number 错误文件编号
 // 10 #define ECHILD 10 // No child processes 子进程不存在
 // 11 #define EAGAIN 11 // Try again 重试
 // 12 #define ENOMEM 12 // Out of memory 内存不足
 // 13 #define EACCES 13 // Permission denied 无权限
 // 14 #define EFAULT 14 // Bad address 地址错误
 // 15 #define ENOTBLK 15 // Block device required 需要块设备
 // 16 #define EBUSY 16 // Device or resource busy 设备或资源忙
 // 17 #define EEXIST 17 // File exists 文件已存在
 // 18 #define EXDEV 18 // Cross-device link 跨设备链路
 // 19 #define ENODEV 19 // No such device 设备不存在
 // 20 #define ENOTDIR 20 // Not a directory 路径不存在
 // 21 #define EISDIR 21 // Is a directory 是路径
 // 22 #define EINVAL 22 // Invalid argument 无效参数
 // 23 #define ENFILE 23 // File table overflow 文件表溢出
 // 24 #define EMFILE 24 // Too many open files 打开的文件过多
 // 25 #define ENOTTY 25 // Not a typewriter 非打字机
 // 26 #define ETXTBSY 26 // Text file busy 文本文件忙
 // 27 #define EFBIG 27 // File too large 文件太大
 // 28 #define ENOSPC 28 // No space left on device 设备无空间
 // 29 #define ESPIPE 29 // Illegal seek 非法查询
 // 30 #define EROFS 30 // Read-only file system 只读文件系统
 // 31 #define EMLINK 31 // Too many links 链接太多
 // 32 #define EPIPE 32 // Broken pipe 管道破裂
 // 33 #define EDOM 33 // Math argument out of domain of func 参数超出函数域
 // 34 #define ERANGE 34 // Math result not representable 结果无法表示
 // 35 #define EDEADLK 35 // Resource deadlock would occur 资源将发生死锁
 // 36 #define ENAMETOOLONG 36 // File name too long 文件名太长
 // 37 #define ENOLCK 37 // No record locks available 没有可用的记录锁
 // 38 #define ENOSYS 38 // Function not implemented 函数未实现
 // 39 #define ENOTEMPTY 39 // Directory not empty 目录非空
 // 40 #define ELOOP 40 // Too many symbolic links encountered 遇到太多符号链接
 // 41 #define EWOULDBLOCK EAGAIN // Operation would block 操作会阻塞
 // 42 #define ENOMSG 42 // No message of desired type 没有符合需求类型的消息
 // 43 #define EIDRM 43 // Identifier removed 标识符已删除
 // 44 #define ECHRNG 44 // Channel number out of range 通道编号超出范围
 // 45 #define EL2NSYNC 45 // Level 2 not synchronized level2不同步
 // 46 #define EL3HLT 46 // Level 3 halted 3级停止
 // 47 #define EL3RST 47 // Level 3 reset 3级重置
 // 48 #define ELNRNG 48 // Link number out of range 链接编号超出范围
 // 49 #define EUNATCH 49 // Protocol driver not attached 协议驱动程序没有连接
 // 50 #define ENOCSI 50 // No CSI structure available 没有可用的CSI结构
 // 51 #define EL2HLT 51 // Level 2 halted 2级停止
 // 52 #define EBADE 52 // Invalid exchange 无效交换
 // 53 #define EBADR 53 // Invalid request descriptor 无效请求描述
 // 54 #define EXFULL 54 // Exchange full 交换完全
 // 55 #define ENOANO 55 // No anode 无阳极
 // 56 #define EBADRQC 56 // Invalid request code 无效请求码
 // 57 #define EBADSLT 57 // Invalid slot 无效插槽
 // 58 #define EDEADLOCK EDEADLK
 // 59 #define EBFONT 59 // Bad font file format 错误的字体文件格式
 // 60 #define ENOSTR 60 // Device not a stream 设备不是流
 // 61 #define ENODATA 61 // No data available 无数据
 // 62 #define ETIME 62 // Timer expired 计时器到期
 // 63 #define ENOSR 63 // Out of streams resources 流资源不足
 // 64 #define ENONET 64 // Machine is not on the network 机器不在网络上
 // 65 #define ENOPKG 65 // Package not installed 包未安装
 // 66 #define EREMOTE 66 // Object is remote 对象是远程
 // 67 #define ENOLINK 67 // Link has been severed 链接正在服务中
 // 68 #define EADV 68 // Advertise error 广告错误
 // 69 #define ESRMNT 69 // Srmount error ？
 // 70 #define ECOMM 70 // Communication error on send 发送过程中通讯错误
 // 71 #define EPROTO 71 // Protocol error 协议错误
 // 72 #define EMULTIHOP 72 // Multihop attempted 多跳尝试
 // 73 #define EDOTDOT 73 // RFS specific error RFS特定错误
 // 74 #define EBADMSG 74 // Not a data message 不是数据类型消息
 // 75 #define EOVERFLOW 75 // Value too large for defined data type 对指定的数据类型来说值太大
 // 76 #define ENOTUNIQ 76 // Name not unique on network 网络上名字不唯一
 // 77 #define EBADFD 77 // File descriptor in bad state 文件描述符状态错误
 // 78 #define EREMCHG 78 // Remote address changed 远程地址改变
 // 79 #define ELIBACC 79 // Can not access a needed shared library 无法访问需要的共享库
 // 80 #define ELIBBAD 80 // Accessing a corrupted shared library 访问损坏的共享库
 // 81 #define ELIBSCN 81 // .lib section in a.out corrupted 库部分在a.out损坏
 // 82 #define ELIBMAX 82 // Attempting to link in too many shared libraries 试图链接太多的共享库
 // 83 #define ELIBEXEC 83 // Cannot exec a shared library directly 不能直接运行共享库
 // 84 #define EILSEQ 84 // Illegal byte sequence 非法字节序
 // 85 #define ERESTART 85 // Interrupted system call should be restarted 应重新启动被中断的系统调用
 // 86 #define ESTRPIPE 86 // Streams pipe error 流管错误
 // 87 #define EUSERS 87 // Too many users 用户太多
 // 88 #define ENOTSOCK 88 // Socket operation on non-socket 在非套接字上进行套接字操作
 // 89 #define EDESTADDRREQ 89 // Destination address required 需要目的地址
 // 90 #define EMSGSIZE 90 // Message too long 消息太长
 // 91 #define EPROTOTYPE 91 // Protocol wrong type for socket 错误协议类型
 // 92 #define ENOPROTOOPT 92 // Protocol not available 协议不可用
 // 93 #define EPROTONOSUPPORT 93 // Protocol not supported 不支持协议
 // 94 #define ESOCKTNOSUPPORT 94 // Socket type not supported 不支持套接字类型
 // 95 #define EOPNOTSUPP 95 // Operation not supported on transport endpoint 操作上不支持传输端点
 // 96 #define EPFNOSUPPORT 96 // Protocol family not supported 不支持协议族
 // 97 #define EAFNOSUPPORT 97 // Address family not supported by protocol 协议不支持地址群
 // 98 #define EADDRINUSE 98 // Address already in use 地址已被使用
 // 99 #define EADDRNOTAVAIL 99 // Cannot assign requested address 无法分配请求的地址
// 100 #define ENETDOWN 100 // Network is down 网络已关闭
// 101 #define ENETUNREACH 101 // Network is unreachable 网络不可达
// 102 #define ENETRESET 102 // Network dropped connection because of reset 网络由于复位断开连接
// 103 #define ECONNABORTED 103 // Software caused connection abort 软件导致连接终止
// 104 #define ECONNRESET 104 // Connection reset by peer 连接被对方复位
// 105 #define ENOBUFS 105 // No buffer space available 没有可用的缓存空间
// 106 #define EISCONN 106 // Transport endpoint is already connected 传输端点已连接
// 107 #define ENOTCONN 107 // Transport endpoint is not connected 传输端点未连接
// 108 #define ESHUTDOWN 108 // Cannot send after transport endpoint shutdown 传输端点关闭后不能在发送
// 109 #define ETOOMANYREFS 109 // Too many references: cannot splice 太多的引用：无法接合
// 110 #define ETIMEDOUT 110 // Connection timed out 连接超时
// 111 #define ECONNREFUSED 111 // Connection refused 连接被拒绝
// 112 #define EHOSTDOWN 112 // Host is down 主机已关闭
// 113 #define EHOSTUNREACH 113 // No route to host 无法路由到主机
// 114 #define EALREADY 114 // Operation already in progress 操作已在进程中
// 115 #define EINPROGRESS 115 // Operation now in progress 进程中正在进行的操作
// 116 #define ESTALE 116 // Stale NFS file handle 
// 117 #define EUCLEAN 117 // Structure needs cleaning 
// 118 #define ENOTNAM 118 // Not a XENIX named type file 
// 119 #define ENAVAIL 119 // No XENIX semaphores available 
// 120 #define EISNAM 120 // Is a named type file 
// 121 #define EREMOTEIO 121 // Remote I/O error 
// 122 #define EDQUOT 122 // Quota exceeded 
// 123 #define ENOMEDIUM 123 // No medium found 
// 124 #define EMEDIUMTYPE 124 // Wrong medium type
// 125:Operation canceled
// 126:Required key not available
// 127:Key has expired
// 128:Key has been revoked
// 129:Key was rejected by service
// 130:Owner died
// 131:State not recoverable
#include <iostream>
#include <string>
#include <map>
std::string error_msg(int eid);
#endif