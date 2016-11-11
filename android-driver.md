## 为Android系统编写Linux内核驱动程序
1. 在kernel/drivers/目录下新建一个目录,目录中存放要添加的驱动模块*.c *.h及Kconfig,Makefile文件  
	Kconfig内容:

	```config HELLO  
		tristate "First Android Driver"  
		default n  
		---help---  
			This is the first android driver. 
	```

	Makefile内容:  
	obj-$(CONFIG_HELLO) += hello.o  

2. 在drivers/Kconfig中的 menu "Device Drivers" 和endmenu之间添加 source "drivers/hello/Kconfig",这样make menuconfig才能读到hello中的Kconfig  
3. 在drivers/Makefile中的 添加 obj-$(CONFIG_HELLO) += hello/ ,这样才能编译到hello模块  
4. make menuconfig 选中模块,编译android中的内核的时候,android编译环境可能已经指定一个默认的配置文件,那么就需要手动去默认配置文件增加 CONFIG_HELLO=y  

## 为Android系统内置C可执行程序测试Linux内核驱动程序
1. 在external添加目录hello  
2. /external/hello目中存放可执行程序源文件及Android.mk  
	Android.mk内容:  
	```  
		LOCAL_PATH := $(call my-dir)  
		include $(CLEAR_VARS)  
		LOCAL_MODULE_TAGS := optional  
		LOCAL_MODULE := hello  
		LOCAL_SRC_FILES := $(call all-subdir-c-files)  
		include $(BUILD_EXECUTABLE)
	```

	BUILD_EXECUTABLE 表示编译为可执行程序  

3. 单独编译模块  
	mmm ./external/hello  
	编译后在路径target/product/xxx/system/bin 下就有hello可执行文件了  
4. 重新打包system.img  
	make snod			快速打包,不检查依赖  
	或者  
	make systemimage  


## 为Android增加硬件抽象层（HAL）模块访问Linux内核驱动程序
1. 在/hardware/libhardware/include/hardware/添加hello.h头文件, 这里按照Android硬件抽象层规范的要求，分别定义模块ID、模块结构体以及硬件接口结构体  
2. 在hardware/libhardware/modules/,目录新建hello目录,添加hello.c源文件和Android.mk  
	如果设备文件是在内核驱动里面通过device_create创建的，而device_create创建的设备文件默认只有root用户可读写，而hello_device_open一般是由上层APP来调用的，这些APP一般不具有root权限，这  时候就导致打开设备文件失败：  
      Hello Stub: failed to open /dev/hello -- Permission denied.  
    解决办法是类似于Linux的udev规则，打开Android源代码工程目录下，进入到 system/core/rootdir 目录，里面有一个名为 **ueventd.rc** 文件，往里面添加一行：  
      **/dev/hello 0666 root root**  
  Android.mk内容:  
	```
		LOCAL_PATH := $(call my-dir)  
		include $(CLEAR_VARS)  
		LOCAL_MODULE_TAGS := optional  
		LOCAL_PRELINK_MODULE := false  
		LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw  
		LOCAL_SHARED_LIBRARIES := liblog  
		LOCAL_SRC_FILES := hello.c  
		LOCAL_MODULE := hello.default  
		include $(BUILD_SHARED_LIBRARY)  
	```
	
  注意，LOCAL_MODULE的定义规则，hello后面跟有default，hello.default能够保证我们的模块总能被硬象抽象层加载到  
  **修改了ueventd.rc文件后,打包system.img不能使用make snod,要用make systemimage,否则对uevented.rc的更改不能生效,驱动的权限未开放,可能导致系统无法启动**  
  可能出现的错误:  
  	[   33.112928] init: untracked pid 402 exited  
  	原因1:  
  		添加驱动后,修改了ueventd.rc未重新编译,或者使用了make snod不检查依赖去编译  
  	原因2:  
		init是第一个系统用户空间进程，它会解析init.target.rc调起来很多service和后台daemon程序，init: untracked pid xxx exit.
		一般是init调其他程序失败，这种问题可能是init程序文件所在的分区读写权限不够，在init.rc或者init.target.rc里面改下mount init程序所在分区的读写权限应该就OK了  
			mount rootfs rootfs / ro remount 改成  
			mount rootfs rootfs / rw remount


3. 编译模块  
	mmm ./hardware/libhardware/modules/hello  
	编译成功后，就可以在out/target/product/xxx/system/lib/hw/目录下看到hello.default.so文件了	
4. 打包system.img镜像  
	make snod  
	重新打包后，system.img就包含我们定义的硬件抽象层模块hello.default了。  
    虽然我们在Android系统为我们自己的硬件增加了一个硬件抽象层模块，但是现在Java应用程序还不能访问到我们的硬件。我们还必须编写JNI方法和在Android的Application Frameworks层增加API接口，才能让上层Application访问我们的硬件


## 为Android硬件抽象层（HAL）模块编写JNI方法提供Java访问硬件服务接口
1. 在frameworks/base/services/jni/ 目录下 增加com_android_server_HelloService.cpp文件  
	注意文件的命令方法，com_android_server前缀表示的是包名，表示硬件服务HelloService是放在frameworks/base/services/java目录下的com/android/server目录的，即存在一个命名为com.android.server.HelloService的类.简单地说，HelloService是一个提供Java接口的硬件访问服务类.
2. 修改同目录下的onload.cpp文件:  
	首先在namespace android增加 int register_android_server_HelloService(JNIEnv *env); 函数声明.
	然后在JNI_onLoad增加  register_android_server_HelloService(env); 函数调用
3. 修改同目录下的Android.mk文件，在LOCAL_SRC_FILES变量中增加一行  com_android_server_HelloService.cpp \  
4. 编译模块  
	mmm ./frameworks/base/services/jni
5. 打包system.img  
	make snod			快速打包,不检查依赖  
	或者  
	make systemimage  
	重新打包的system.img镜像文件就包含我们刚才编写的JNI方法了，也就是我们可以通过Android系统的Application Frameworks层提供的硬件服务HelloService来调用这些JNI方法，进而调用低层的硬件抽象层接口去访问硬件了




## 为Android系统的Application Frameworks层增加硬件访问服务
	在Android系统中，硬件服务一般是运行在一个独立的进程中为各种应用程序提供服务。因此，调用这些硬件服务的应用程序与这些硬件服务之间的通信需要通过代理来进行。为此，我们要先定义好通信接口  
1. 在 frameworks/base/core/java/android/os 目录，新增 IHelloService.aidl 接口定义文件  
IHelloService.aidl内容:  
	```  
	package android.os;  
	interface IHelloService {  
	    void setVal(int val);  
	    int getVal();  
	}  
	```
2. 到frameworks/base目录，打开Android.mk文件，修改LOCAL_SRC_FILES变量的值，增加IHelloService.aidl源文件  
	 core/java/android/os/IHelloService.aidl \  
3. 编译 IHelloService.aidl 接口  
	mmm frameworks/base  
	这样，就会根据IHelloService.aidl生成相应的 IHelloService.Stub 接口  
4. 在 frameworks/base/services/java/com/android/server 目录，新增HelloService.java文件  
	HelloService主要是通过调用JNI方法来提供硬件服务  
5. 修改同目录的SystemServer.java文件，在ServerThread::run函数中增加加载HelloService的代码  
	```  
	try {  
		Slog.i(TAG, "Hello Service");  
		ServiceManager.addService("hello", new HelloService());  
	} catch (Throwable e) {  
		Slog.e(TAG, "Failure starting Hello Service", e);  
	}  
	```
6. 编译模块  
	mmm ./frameworks/base/services/java  
7. 打包system.img  
	make snod			快速打包,不检查依赖  
	或者  
	make systemimage  
	重新打包后的system.img系统镜像文件就在Application Frameworks层中包含了我们自定义的硬件服务HelloService了，并且会在系统启动的时候，自动加载HelloService。这时，应用程序就可以通过Java接口来访问Hello硬件服务了  

## 为Android系统内置Java应用程序测试Application Frameworks层的硬件服务 
1. 将Hello目录复制到 package/experimental/ 目录下  
2. 编译模块  
	mmm ./packages/experimental/Hello  
3. 打包system.img  
	make snod			快速打包,不检查依赖  
	或者  
	make systemimage  


[参考文献](http://blog.csdn.net/luoshengyang/article/details/6568411)