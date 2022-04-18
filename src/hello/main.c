#include <stdio.h>
#include <stdlib.h>
#include <uv.h>

// 系统编程中最经常处理的一般时输入输出，而不是处理一堆数据。而问题在于传统的输入/输出函数（read, fprintf）都是阻塞式的。
// 也就是如果这些操作（写入文件，从网络上下载）没有完成，函数是不会返回的，即使cpu空闲，你的程序这段时间什么也做不了。

// 对于这个问题，一个标准的解决方案是使用多线程。每个阻塞操作都被分配到各个线程中（或者使用线程池）。
// 当某个线程一旦阻塞，处理器就可以调度处理其他需要cpu资源的线程。

// libuv使用另一种解决方案，那就是异步，非阻塞。而且大多数现代操作系统也提供了基于事件通知的子系统。
// 例如: 一个正常的socket上的read调用会发生阻塞，直到发送方把信息发送过来。但是，实际上程序可以请求操作系统监视socket事件的到来，并将这个事件通知放到事件队列中。这样，程序就可以很简单地检查事件是否到来
// 说libuv是异步的，是因为程序可以在一头表达对某一事件的兴趣，并在另一头获取到数据。
// 本质上是一种发布订阅模式（先订阅，后续收到订阅的消息再做相应的处理）。

// 在事件驱动编程中，程序会关注每一个事件，并且对每一个事件的发生做出反应。
// libuv会负责将来自操作系统的事件收集起来，或者监视其他来源的事件。
// 这样，用户就可以注册回调函数，回调函数会在事件发生的时候被调用。
// event-loop会一直保持运行状态, 用伪代码描述如下:

// ----------------------------------------------------------------
// while there are still events to process:
//   e = get the next event
//   if there is a callback associated with e:
//      call the callback
// ----------------------------------------------------------------

int main() {
    // 可以使用uv_loop_init初始化loop之前，给其分配相应的内存，以便你植入自定义内存关联方法。
    // uv_loop_t *loop = malloc(sizeof(uv_loop_t));

    // 这里使用默认的loop
    uv_loop_t *loop = uv_default_loop();
    
    // uv_loop_init(loop);
	// 可以使用uv_default_loop获取libuv提供的默认loop。如果只需要一个loop的话，可以使用这个。
    // uv_loop_init(uv_default_loop());
    printf("quitting.\n");
    uv_run(loop, UV_RUN_DEFAULT);
    
    // 要手动关闭loop,程序退出时会关闭loop，系统也会自动回收内存
    uv_loop_close(loop);
    //free(loop);
	//return uv_run(loop, UV_RUN_DEFAULT);
    return 0;
}