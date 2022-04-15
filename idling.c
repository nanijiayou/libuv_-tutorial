#include <stdio.h>
#include <uv.h>

int64_t counter = 0;

void wait_for_a_while(uv_idle_t* handle) {
	counter++;
	if (counter >= 5e6) {
		printf("counter %d", counter);
		uv_idle_stop(handle);
	}
}

int main() {
	// libuv的工作建立在用户表达对特定事件的兴趣（订阅），通常通过创建I/O设备，定时器，进程等的handle来实现。
	// hanle时不透明的数据结构，其中对应得类型uv_TYPE_t中的TYPE指定了handle的使用目的。
	// handle代表了持久性对象。在异步的操作中，相应的handle上有许多与之相关的request。
	// request时短暂性的对象（通常只维持一个回调函数的时间），通常对应着handle上的一个I/O操作。
	// request用来在初始函数和回调函数之间，传递上下文。
	// 例如：uv_udp_t代表了一个udp的socket，然而，对于每一个向socket的写入完成后，都会向回调函数传递一个uv_udp_send_t

	// 一个空转的handle
	uv_idle_t idlter;

	// 通过uv_TYPE_int函数设置handle
	uv_idle_init(uv_default_loop(), &idlter);
	uv_idle_start(&idlter, wait_for_a_while);

	printf("idleing... \n");
	uv_run(uv_default_loop(), UV_RUN_DEFAULT);
	uv_loop_close(uv_default_loop());

	return 0;
}