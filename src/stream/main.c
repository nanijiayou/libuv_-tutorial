#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <uv.h>

typedef struct {
	uv_write_t req;
	uv_buf_t buf;
}write_req_t;

uv_loop_t* loop;
uv_pipe_t stdin_pipe;
uv_pipe_t stdout_pipe;
uv_pipe_t file_pipe;

// 动态分配存储空间，调用malloc
void alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
	*buf = uv_buf_init((char*)malloc(suggested_size), suggested_size);
}

void free_write_req(uv_write_t* req) {
	write_req_t* wr = (write_req_t*)req;
	free(wr->buf.base);
	free(wr);
}

void on_stdout_write(uv_write_t* req, int status) {
	free_write_req(req);
}

void on_file_write(uv_write_t* req, int status) {
	free_write_req(req);
}

// 开辟一块地址空间存储从缓冲区读取出来的数据
// 这块缓冲不回释放，直到与uv_write绑定的回调执行函数执行。
void write_data(uv_stream_t* dest, size_t size, uv_buf_t buf, uv_write_cb cb) {
	write_req_t* req = (write_req_t*)malloc(sizeof(write_req_t));
	req->buf = uv_buf_init((char*)malloc(size), size);
	memcpy(req->buf.base, buf.base, size);
	uv_write((uv_write_t*)req, (uv_stream_t*)dest, &req->buf, 1, cb);
}

void read_stdin(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
	printf("read stin \n", nread);
	if (nread < 0) {
		// nread小于0，表示有错误发生，包括可能的EOF(读到文件尾部)
		if (nread == UV_EOF) {
			uv_close((uv_handle_t*)&stdin_pipe, NULL);
			uv_close((uv_handle_t*)&stdout_pipe, NULL);
			uv_close((uv_handle_t*)&file_pipe, NULL);
		}
	}
	else if(nread > 0) {
		printf("read stin \n", nread);
		// nread 大于0，表示我们可以向输出流中写入的字节数目。
		write_data((uv_stream_t*)&stdout_pipe, nread, *buf, on_stdout_write);
		write_data((uv_stream_t*)&file_pipe, nread, *buf, on_file_write);
	}
	// 缓冲区需要手动回收
	if (buf->base) free(buf->base);
}

// 实现一个liux tee命令
int main(int argc, char **argv) {
	loop = uv_default_loop();

	// uv_pipe_t 可以将本地文件转换为stream的形态
	// 当需要使用具名管道时，可以将uv_pipe_init的第三个参数设为1
	uv_pipe_init(loop, &stdin_pipe, 0);

	// 将管道和文件描述符关联起来，这里把管道stdin_pipe和标准输入关联
	// 0：标准输入，1： 标准输出，2：标准错误输出
	uv_pipe_open(&stdin_pipe, 0);

	// 同上，关联标准输出
	uv_pipe_init(loop, &stdout_pipe, 0);
	uv_pipe_open(&stdout_pipe, 1);

	// 关联输出文件，文件由可执行程序参数给出
	uv_fs_t file_req;
	int fd = uv_fs_open(loop, &file_req, argv[1], O_CREAT | O_RDWR, 0644, NULL);
	uv_pipe_init(loop, &file_pipe, 0);
	uv_pipe_open(&file_pipe, fd);

	// 调用uv_read_start，开始监听stdin
	// 当需要新的缓冲区来存储数据时，调用alloc_buffer
	// 在函数read_stdin中可以定义缓冲区中的数据处理操作
	uv_read_start((uv_stream_t*)&stdin_pipe, alloc_buffer, read_stdin);

	uv_run(loop, UV_RUN_DEFAULT);
	return 0;
}
