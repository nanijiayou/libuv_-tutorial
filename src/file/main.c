#include <stdio.h>
#include <uv.h>
#include <fcntl.h>
#include <assert.h>


// 简单文件独写可以通过 uv_fs_*函数族和与之相关的uv_fs_t结构体来完成的

// libuv提供的文件操作和socket operations并不相同。套接字操作使用了操作系统本身提供的非阻塞操作
// 而文件操作内部使用了阻塞函数，但是libuv是在线程池中调用这些函数，并在应用程序需要交互时通知在事件循环中注册的监视器。

// 所有的文件操作函数都有这两种形式，同步synchronous 和 异步 asynchronous.
// 同步synchonous形式如果没有指定回调函数则会被自动调用（并阻塞的），函数的返回值是libuv error code。
// 异步asynchonous形式则会在传入回调函数是被调用，并返回0.


void on_read(uv_fs_t* req);

uv_fs_t open_req;
uv_fs_t read_req;
uv_fs_t write_req;

static uv_buf_t iov;
static char buffer[1024];

void on_write(uv_fs_t* req) {
	if (req->result < 0) {
		fprintf(stderr, "write error: %s\n", uv_strerror((int)req->result));
	}
	else {
		// 文件系统的写入使用 uv_fs_write()，当写入完成时会触发回调函数，这里会触发下一次的读取。
		uv_fs_read(uv_default_loop(), &read_req, open_req.result, &iov, 1, -1, on_read);
	}
}

void on_read(uv_fs_t *req) {
	if (req->result < 0) {
		fprintf(stderr, "read error: %s\n", uv_strerror(req->result));
	}
	else if (req->result == 0) {
		// 当读到文件结尾时(EOF)，result 返回0。
		uv_fs_t close_req;
		uv_fs_close(uv_default_loop(), &close_req, open_req.result, NULL);
		printf("file read completed \n");
	}
	else if (req->result > 0) {
		iov.len = req->result;
		uv_fs_write(uv_default_loop(), &write_req, 1, &iov, 1, -1, on_write);
	}
}

void on_open(uv_fs_t* req) {
	assert(req == &open_req);
	// req->result保存了uv_fs_open回调函数打开的文件描述符。
	if (req->result >= 0) {
		// 调用读取函数时，必须传入一个已经初始化的缓冲区
		// 在on_read被触发后，缓冲区将被写入数据。
		iov = uv_buf_init(buffer, sizeof(buffer));
		uv_fs_read(uv_default_loop(), &read_req, req->result, &iov, 1, -1, on_read);
	}
	else {
		fprintf(stderr, "error on open file: %s\n", uv_strerror(req->result));
	}
}
int main(int argc, char **argv) {
	// 获取文件描述符
	// int uv_fs_open(uv_loop_t* loop, uv_fs_t* req, const char* path, int flags, int mode, uv_fs_cb cb)
	// flags 与 mode 的标准的 Unix flags相同，libuv会处理跨平台的转换.
	// 回调函数形式：void callbcak(uv_fs_t* req)
	printf("file name %s\n", argv[1]);
	uv_fs_open(uv_default_loop(), &open_req, argv[1], O_RDONLY, 0, on_open);
	uv_run(uv_default_loop(), UV_RUN_DEFAULT);

	uv_fs_req_cleanup(&open_req);
	uv_fs_req_cleanup(&read_req);
	uv_fs_req_cleanup(&write_req);

	// 关闭文件描述符
	// int uv_fs_close(uv_loop_t* loop, uv_fs_t* req, uv_file file, uv_fs_cb cb)

	return 0;
}