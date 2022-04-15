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
	// libuv�Ĺ����������û������ض��¼�����Ȥ�����ģ���ͨ��ͨ������I/O�豸����ʱ�������̵ȵ�handle��ʵ�֡�
	// hanleʱ��͸�������ݽṹ�����ж�Ӧ������uv_TYPE_t�е�TYPEָ����handle��ʹ��Ŀ�ġ�
	// handle�����˳־��Զ������첽�Ĳ����У���Ӧ��handle���������֮��ص�request��
	// requestʱ�����ԵĶ���ͨ��ֻά��һ���ص�������ʱ�䣩��ͨ����Ӧ��handle�ϵ�һ��I/O������
	// request�����ڳ�ʼ�����ͻص�����֮�䣬���������ġ�
	// ���磺uv_udp_t������һ��udp��socket��Ȼ��������ÿһ����socket��д����ɺ󣬶�����ص���������һ��uv_udp_send_t

	// һ����ת��handle
	uv_idle_t idlter;

	// ͨ��uv_TYPE_int��������handle
	uv_idle_init(uv_default_loop(), &idlter);
	uv_idle_start(&idlter, wait_for_a_while);

	printf("idleing... \n");
	uv_run(uv_default_loop(), UV_RUN_DEFAULT);
	uv_loop_close(uv_default_loop());

	return 0;
}