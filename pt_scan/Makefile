obj-m += pt_scan.o

pt_scan-objs := page_table.o main.o ivshmem-drv.o communicate.o

vtism_dir = ~/code/linux-6.6
basic_dir = /lib/modules/$(shell uname -r)/build

kernel_dir = $(vtism_dir)
# kernel_dir = $(basic_dir)

EXTRA_CFLAGS += -Wno-declaration-after-statement -Wno-unused-result

all:
	make -C $(kernel_dir) M=$(PWD) modules
	

clean:
	rm -f *.o *.ko *.mod.c *.mod.o *.cmd *.symvers *.order

insmod:
	sudo insmod pt_scan.ko

rmmod:
	sudo insmod ../force_rmmod/force_rmmod.ko modname=pt_scan
	sudo rmmod pt_scan
	sudo rmmod force_rmmod

dmesg:
	sudo dmesg
