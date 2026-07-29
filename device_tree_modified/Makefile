obj-m += gpio_demo.o
KDIR := /home/sdp/armv7-multiplatform/KERNEL
PWD := $(shell pwd)
CROSS_COMPILE := /home/sdp/armv7-multiplatform/dl/gcc-8.5.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi-

ccflags-y += -g -gdwarf-4 -O0

all:
	make -C $(KDIR) M=$(PWD) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) modules

clean:
	make -C $(KDIR) M=$(PWD) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) clean
