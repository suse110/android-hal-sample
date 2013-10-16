1.driver
kernel/drivers/hello
修改:arch/arm/Kconfig和drivers/kconfig两个文件，在menu "Device Drivers"和endmenu之间添加一行：source "drivers/hello/Kconfig"
修改:drivers/Makefile文件，添加一行：obj-$(CONFIG_HELLO) += hello/
配置编译选项：kernel/$ make menuconfig找到"Device Drivers" => "First Android Drivers"选项，设置为y
编译：kernel/$ make编译成功后，就可以在hello目录下看到hello.o文件了，这时候编译出来的zImage已经包含了hello驱动
验证：
#ls dev
#cd proc
#ls
#cat hello
#cat '5' > hello
#cat hello
#cd sys/class
#ls
#cd hello
#ls
#cat val
#echo '0' > val
#cat val

2.c application
external/hello
编译：	mmm external/hello
		make snod
验证：
#cd system/bin
#./hello

3.hal layer hello.default.so
hardware/libhardware/modules/hello/
hardware/libhardware/include/hardware/hello.h
编译：	mmm hardware/libhardware/modules/hello
		make snod
生成：	system/lib/hd/hello.default.so

4.jni layer 
framework/base/services/jni
修改：framwork/base/services/jni/onload.cpp，增加int register_android_server_HelloService(JNIEnv *env);和register_android_server_HelloService(env);
修改：framwork/base/services/jni/Android.mk，增加com_android_server_HelloService.cpp \
编译：mmm framwork/base/services/jni
生成：system/lib/libandroid_servers.so

5.framework service
framework/base/core/java/android/os/IHelloService.aidl
修改：framework/base/Android.mk，增加core/java/android/os/IHelloService.aidl /
修改：framework/base/services/java/com/android/server/SystemServer.java，增加
try {
	Slog.i(TAG, "Hello Service");
	ServiceManager.addService("hello", new HelloService());
} catch (Throwable e) {
	Slog.e(TAG, "Failure starting Hello Service", e);
}
编译：mmm frameworks/base
	  mmm frameworks/base/services/java
生成：IHelloService.java
