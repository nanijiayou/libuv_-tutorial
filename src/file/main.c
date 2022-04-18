#include <stdio.h>
#include <uv.h>
#include <fcntl.h>
#include <assert.h>


// ���ļ���д����ͨ�� uv_fs_*���������֮��ص�uv_fs_t�ṹ������ɵ�

// libuv�ṩ���ļ�������socket operations������ͬ���׽��ֲ���ʹ���˲���ϵͳ�����ṩ�ķ���������
// ���ļ������ڲ�ʹ������������������libuv�����̳߳��е�����Щ����������Ӧ�ó�����Ҫ����ʱ֪ͨ���¼�ѭ����ע��ļ�������

// ���е��ļ���������������������ʽ��ͬ��synchronous �� �첽 asynchronous.
// ͬ��synchonous��ʽ���û��ָ���ص�������ᱻ�Զ����ã��������ģ��������ķ���ֵ��libuv error code��
// �첽asynchonous��ʽ����ڴ���ص������Ǳ����ã�������0.


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
		// �ļ�ϵͳ��д��ʹ�� uv_fs_write()����д�����ʱ�ᴥ���ص�����������ᴥ����һ�εĶ�ȡ��
		uv_fs_read(uv_default_loop(), &read_req, open_req.result, &iov, 1, -1, on_read);
	}
}

void on_read(uv_fs_t *req) {
	if (req->result < 0) {
		fprintf(stderr, "read error: %s\n", uv_strerror(req->result));
	}
	else if (req->result == 0) {
		// �������ļ���βʱ(EOF)��result ����0��
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
	// req->result������uv_fs_open�ص������򿪵��ļ���������
	if (req->result >= 0) {
		// ���ö�ȡ����ʱ�����봫��һ���Ѿ���ʼ���Ļ�����
		// ��on_read�������󣬻���������д�����ݡ�
		iov = uv_buf_init(buffer, sizeof(buffer));
		uv_fs_read(uv_default_loop(), &read_req, req->result, &iov, 1, -1, on_read);
	}
	else {
		fprintf(stderr, "error on open file: %s\n", uv_strerror(req->result));
	}
}
int main(int argc, char **argv) {
	// ��ȡ�ļ�������
	// int uv_fs_open(uv_loop_t* loop, uv_fs_t* req, const char* path, int flags, int mode, uv_fs_cb cb)
	// flags �� mode �ı�׼�� Unix flags��ͬ��libuv�ᴦ���ƽ̨��ת��.
	// �ص�������ʽ��void callbcak(uv_fs_t* req)
	printf("file name %s\n", argv[1]);
	uv_fs_open(uv_default_loop(), &open_req, argv[1], O_RDONLY, 0, on_open);
	uv_run(uv_default_loop(), UV_RUN_DEFAULT);

	uv_fs_req_cleanup(&open_req);
	uv_fs_req_cleanup(&read_req);
	uv_fs_req_cleanup(&write_req);

	// �ر��ļ�������
	// int uv_fs_close(uv_loop_t* loop, uv_fs_t* req, uv_file file, uv_fs_cb cb)

	return 0;
}