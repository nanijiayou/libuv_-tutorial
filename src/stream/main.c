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

// ��̬����洢�ռ䣬����malloc
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

// ����һ���ַ�ռ�洢�ӻ�������ȡ����������
// ��黺�岻���ͷţ�ֱ����uv_write�󶨵Ļص�ִ�к���ִ�С�
void write_data(uv_stream_t* dest, size_t size, uv_buf_t buf, uv_write_cb cb) {
	write_req_t* req = (write_req_t*)malloc(sizeof(write_req_t));
	req->buf = uv_buf_init((char*)malloc(size), size);
	memcpy(req->buf.base, buf.base, size);
	uv_write((uv_write_t*)req, (uv_stream_t*)dest, &req->buf, 1, cb);
}

void read_stdin(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
	printf("read stin \n", nread);
	if (nread < 0) {
		// nreadС��0����ʾ�д��������������ܵ�EOF(�����ļ�β��)
		if (nread == UV_EOF) {
			uv_close((uv_handle_t*)&stdin_pipe, NULL);
			uv_close((uv_handle_t*)&stdout_pipe, NULL);
			uv_close((uv_handle_t*)&file_pipe, NULL);
		}
	}
	else if(nread > 0) {
		printf("read stin \n", nread);
		// nread ����0����ʾ���ǿ������������д����ֽ���Ŀ��
		write_data((uv_stream_t*)&stdout_pipe, nread, *buf, on_stdout_write);
		write_data((uv_stream_t*)&file_pipe, nread, *buf, on_file_write);
	}
	// ��������Ҫ�ֶ�����
	if (buf->base) free(buf->base);
}

// ʵ��һ��liux tee����
int main(int argc, char **argv) {
	loop = uv_default_loop();

	// uv_pipe_t ���Խ������ļ�ת��Ϊstream����̬
	// ����Ҫʹ�þ����ܵ�ʱ�����Խ�uv_pipe_init�ĵ�����������Ϊ1
	uv_pipe_init(loop, &stdin_pipe, 0);

	// ���ܵ����ļ���������������������ѹܵ�stdin_pipe�ͱ�׼�������
	// 0����׼���룬1�� ��׼�����2����׼�������
	uv_pipe_open(&stdin_pipe, 0);

	// ͬ�ϣ�������׼���
	uv_pipe_init(loop, &stdout_pipe, 0);
	uv_pipe_open(&stdout_pipe, 1);

	// ��������ļ����ļ��ɿ�ִ�г����������
	uv_fs_t file_req;
	int fd = uv_fs_open(loop, &file_req, argv[1], O_CREAT | O_RDWR, 0644, NULL);
	uv_pipe_init(loop, &file_pipe, 0);
	uv_pipe_open(&file_pipe, fd);

	// ����uv_read_start����ʼ����stdin
	// ����Ҫ�µĻ��������洢����ʱ������alloc_buffer
	// �ں���read_stdin�п��Զ��建�����е����ݴ������
	uv_read_start((uv_stream_t*)&stdin_pipe, alloc_buffer, read_stdin);

	uv_run(loop, UV_RUN_DEFAULT);
	return 0;
}
