ARCH=arm
CROSS_COMPILE=arm-none-linux-gnueabi-
obj-m := ioport.o
# CC	= $(CROSS_COMPILE)gcc
KDIR	:=/root/qt_x210v3/kernel
PWD	:=$(shell pwd)
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	rm -rf *.o* *~  *.ko *.mod.c
	rm -rf *.tmp_versions/
	rm -rf *.symvers

#ARCH=arm  
#CROSS_COMPILE=arm-linux-  
#obj-m := led.o  
#KDIR :=/home/cfan/linux/linux-3.0.1   
#PWD :=$(shell pwd)  
#all:  
#    $(MAKE) -C $(KDIR) M=$(PWD) modules  
#clean:  
#    $(MAKE) -C $(KDIR) M=$(PWD) clean  
