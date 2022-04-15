#include <stdio.h>
#include <stdlib.h>
#include <uv.h>

int main() {
    // 可以使用uv_loop_init初始化loop之前，给其分配相应的内存，以便你植入自定义内存关联方法。
    uv_loop_t *loop = malloc(sizeof(uv_loop_t));
    
    uv_loop_init(loop);
	// 可以使用uv_default_loop获取libuv提供的默认loop。如果只需要一个loop的话，可以使用这个。
    //uv_loop_init(uv_default_loop());

    printf("quitting.\n");
    uv_run(loop, UV_RUN_DEFAULT);
    
    // 要手动关闭loop,程序退出时会关闭loop，系统也会自动回收内存
    uv_loop_close(loop);
    free(loop);
    return 0;
}