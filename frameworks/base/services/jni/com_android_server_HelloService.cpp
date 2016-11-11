#define LOG_TAG "HelloService"
#include "jni.h"
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"
#include <utils/misc.h>
#include <utils/Log.h>
#include <hardware/hardware.h>
#include <hardware/hello.h>
#include <stdio.h>
/*
�� hello_init �����У�ͨ��AndroidӲ��������ṩ�� hw_get_module ����������ģ��IDΪ HELLO_HARDWARE_MODULE_ID ��Ӳ�������ģ�飬���У�
HELLO_HARDWARE_MODULE_ID ����<hardware/hello.h>�ж���ġ�AndroidӲ����������� HELLO_HARDWARE_MODULE_ID ��ֵ��Androidϵͳ
��/system/lib/hwĿ¼���ҵ���Ӧ��ģ�飬Ȼ��������������ҷ��� hw_module_t �ӿڸ�������ʹ�á��� jniRegisterNativeMethods �����У��ڶ�
��������ֵ�����ӦHelloService���ڵİ���·������com.android.server.HelloService��
*/
namespace android
{
	/*��Ӳ��������ж����Ӳ�����ʽṹ�壬�ο�<hardware/hello.h>*/
        struct hello_device_t* hello_device = NULL;
	/*ͨ��Ӳ������㶨���Ӳ�����ʽӿ�����Ӳ���Ĵ���val��ֵ*/
        static void hello_setVal(JNIEnv* env, jobject clazz, jint value) {
		int val = value;
		LOGI("Hello JNI: set value %d to device.", val);
		if(!hello_device) {
			LOGI("Hello JNI: device is not open.");
			return;
		}
		
		hello_device->set_val(hello_device, val);
	}
        /*ͨ��Ӳ������㶨���Ӳ�����ʽӿڶ�ȡӲ���Ĵ���val��ֵ*/
	static jint hello_getVal(JNIEnv* env, jobject clazz) {
		int val = 0;
		if(!hello_device) {
			LOGI("Hello JNI: device is not open.");
			return val;
		}
		hello_device->get_val(hello_device, &val);
		
		LOGI("Hello JNI: get value %d from device.", val);
	
		return val;
	}
        /*ͨ��Ӳ������㶨���Ӳ��ģ��򿪽ӿڴ�Ӳ���豸*/
	static inline int hello_device_open(const hw_module_t* module, struct hello_device_t** device) {
		return module->methods->open(module, HELLO_HARDWARE_MODULE_ID, (struct hw_device_t**)device);
	}
        /*ͨ��Ӳ��ģ��ID������ָ����Ӳ�������ģ�鲢��Ӳ��*/
	static jboolean hello_init(JNIEnv* env, jclass clazz) {
		hello_module_t* module;
		
		LOGI("Hello JNI: initializing......");
		if(hw_get_module(HELLO_HARDWARE_MODULE_ID, (const struct hw_module_t**)&module) == 0) {
			LOGI("Hello JNI: hello Stub found.");
			if(hello_device_open(&(module->common), &hello_device) == 0) {
				LOGI("Hello JNI: hello device is open.");
				return 0;
			}
			LOGE("Hello JNI: failed to open hello device.");
			return -1;
		}
		LOGE("Hello JNI: failed to get hello stub module.");
		return -1;		
	}
        /*JNI������*/
	static const JNINativeMethod method_table[] = {
		{"init_native", "()Z", (void*)hello_init},
		{"setVal_native", "(I)V", (void*)hello_setVal},
		{"getVal_native", "()I", (void*)hello_getVal},
	};
        /*ע��JNI����*/
	int register_android_server_HelloService(JNIEnv *env) {
    		return jniRegisterNativeMethods(env, "com/android/server/HelloService", method_table, NELEM(method_table));
	}
};