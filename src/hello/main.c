#include <stdio.h>
#include <stdlib.h>
#include <uv.h>

int main() {
    // ����ʹ��uv_loop_init��ʼ��loop֮ǰ�����������Ӧ���ڴ棬�Ա���ֲ���Զ����ڴ����������
    uv_loop_t *loop = malloc(sizeof(uv_loop_t));
    
    uv_loop_init(loop);
	// ����ʹ��uv_default_loop��ȡlibuv�ṩ��Ĭ��loop�����ֻ��Ҫһ��loop�Ļ�������ʹ�������
    //uv_loop_init(uv_default_loop());

    printf("quitting.\n");
    uv_run(loop, UV_RUN_DEFAULT);
    
    // Ҫ�ֶ��ر�loop,�����˳�ʱ��ر�loop��ϵͳҲ���Զ������ڴ�
    uv_loop_close(loop);
    free(loop);
    return 0;
}