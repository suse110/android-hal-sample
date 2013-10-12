1.driver
kernel/drivers/hello
修改arch/arm/Kconfig和drivers/kconfig两个文件，在menu "Device Drivers"和endmenu之间添加一行：source "drivers/hello/Kconfig"
修改drivers/Makefile文件，添加一行：obj-$(CONFIG_HELLO) += hello/
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
