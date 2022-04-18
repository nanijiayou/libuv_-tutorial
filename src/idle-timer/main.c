#include <stdio.h>
#include <uv.h>

uv_loop_t* loop;
uv_fs_t stdin_watcher;

// һ����ת��handle
uv_idle_t idler;

char buffer[1024];

void crunch_away(uv_idle_t* handle) {
	fprintf(stderr, "Computing ... \n");
	uv_idle_stop(handle);
}

void on_type(uv_fs_t* req) {
	if (stdin_watcher.result > 0) {
		buffer[stdin_watcher.result] = '\0';
		printf("typed %s \n", buffer);

		uv_buf_t buf = uv_buf_init(buffer, sizeof(buffer));
		// ������һ�λص���
		uv_fs_read(loop, &stdin_watcher, 0, &buf, 1, -1, on_type);
		uv_idle_start(&idler, crunch_away);
	}
	else if (stdin_watcher.result < 0) {
		fprintf(stderr, "error opening file: %s\n", uv_strerror(req->result));
	}
}

int main() {
	// ʹ��Ĭ�ϵ�loop
	loop = uv_default_loop();

	// ͨ��uv_TYPE_int��������handle
	uv_idle_init(loop, &idler);

	// ��ʼ��һ������������stdin����д��buffer
	uv_buf_t buf = uv_buf_init(buffer, sizeof(buffer));
	uv_fs_read(loop, &stdin_watcher, 0, &buf, 1, -1, on_type);
	uv_idle_start(&idler, crunch_away);
	return uv_run(loop, UV_RUN_DEFAULT);
}