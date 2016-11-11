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
在 hello_init 函数中，通过Android硬件抽象层提供的 hw_get_module 方法来加载模块ID为 HELLO_HARDWARE_MODULE_ID 的硬件抽象层模块，其中，
HELLO_HARDWARE_MODULE_ID 是在<hardware/hello.h>中定义的。Android硬件抽象层会根据 HELLO_HARDWARE_MODULE_ID 的值在Android系统
的/system/lib/hw目录中找到相应的模块，然后加载起来，并且返回 hw_module_t 接口给调用者使用。在 jniRegisterNativeMethods 函数中，第二
个参数的值必须对应HelloService所在的包的路径，即com.android.server.HelloService。
*/
namespace android
{
	/*在硬件抽象层中定义的硬件访问结构体，参考<hardware/hello.h>*/
        struct hello_device_t* hello_device = NULL;
	/*通过硬件抽象层定义的硬件访问接口设置硬件寄存器val的值*/
        static void hello_setVal(JNIEnv* env, jobject clazz, jint value) {
		int val = value;
		LOGI("Hello JNI: set value %d to device.", val);
		if(!hello_device) {
			LOGI("Hello JNI: device is not open.");
			return;
		}
		
		hello_device->set_val(hello_device, val);
	}
        /*通过硬件抽象层定义的硬件访问接口读取硬件寄存器val的值*/
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
        /*通过硬件抽象层定义的硬件模块打开接口打开硬件设备*/
	static inline int hello_device_open(const hw_module_t* module, struct hello_device_t** device) {
		return module->methods->open(module, HELLO_HARDWARE_MODULE_ID, (struct hw_device_t**)device);
	}
        /*通过硬件模块ID来加载指定的硬件抽象层模块并打开硬件*/
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
        /*JNI方法表*/
	static const JNINativeMethod method_table[] = {
		{"init_native", "()Z", (void*)hello_init},
		{"setVal_native", "(I)V", (void*)hello_setVal},
		{"getVal_native", "()I", (void*)hello_getVal},
	};
        /*注册JNI方法*/
	int register_android_server_HelloService(JNIEnv *env) {
    		return jniRegisterNativeMethods(env, "com/android/server/HelloService", method_table, NELEM(method_table));
	}
};