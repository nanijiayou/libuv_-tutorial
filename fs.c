#include <stdio.h>
#include <uv.h>


// 简单文件独写可以通过 uv_fs_*函数族和与之相关的uv_fs_t结构体来完成的

// libuv提供的文件操作和socket operations并不相同。套接字操作使用了操作系统本身提供的非阻塞操作
// 而文件操作内部使用了阻塞函数，但是libuv是在线程池中调用这些函数，并在应用程序需要交互时通知在事件循环中注册的监视器。

// 所有的文件操作函数都有这两种形式，同步synchronous 和 异步 asynchronous.
// 同步synchonous形式如果没有指定回调函数则会被自动调用（并阻塞的），函数的返回值是libuv error code。
// 异步asynchonous形式则会在传入回调函数是被调用，并返回0.
int main() {

	return 0;
}