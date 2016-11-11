#ifndef ANDROID_HELLO_INTERFACE_H
#define ANDROID_HELLO_INTERFACE_H
#include <hardware/hardware.h>
#define LOG_TAG "HelloStub"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

__BEGIN_DECLS
/*
���ﰴ��AndroidӲ�������淶��Ҫ�󣬷ֱ���ģ��ID��ģ��ṹ���Լ�Ӳ���ӿڽṹ�塣��Ӳ���ӿڽṹ���У�fd��ʾ�豸�ļ���������
��Ӧ���ǽ�Ҫ������豸�ļ�"/dev/hello"��set_val��get_valΪ��HAL�����ṩ�ĺ����ӿ�
*/
/*����ģ��ID*/
#define HELLO_HARDWARE_MODULE_ID "hello"

/*Ӳ��ģ��ṹ��*/
struct hello_module_t {
	struct hw_module_t common;
};

/*Ӳ���ӿڽṹ��*/
struct hello_device_t {
	struct hw_device_t common;
	int fd;
	int (*set_val)(struct hello_device_t* dev, int val);
	int (*get_val)(struct hello_device_t* dev, int* val);
};

__END_DECLS

#endif