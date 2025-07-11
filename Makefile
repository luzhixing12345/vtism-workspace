.PHONY: init qemu clean disk

QEMU_EXE = qemu-system-x86_64
#QEMU = qemu-system-i386

QEMU_PATH = /home/lzx/qemu/build
QEMU = $(QEMU_PATH)/$(QEMU_EXE)
QEMU = qemu-system-x86_64

KVM = -enable-kvm -cpu host

QEMU_STR_ARG = earlyprintk=serial,ttyS0 console=ttyS0 noapic norandmaps

INITRAMFS = initramfs.img
#INITRAMFS = /home/kamilu/aos/lab/rootfs/rootfs.img.gz

LINUX_VERSION = 6.6
# LINUX_VERSION = 4.4.6

IMAGE = /home/lzx/linux-$(LINUX_VERSION)
#IMAGE = /home/kamilu/aos/lab/4.4.6-ubuntu1604/ubuntu1604

# CXL_ARGS =  -M q35,cxl=on -m 4G,maxmem=8G,slots=8 -smp 4
# CXL_ARGS += -object memory-backend-ram,id=vmem0,share=on,size=256M \
# -device pxb-cxl,bus_nr=12,bus=pcie.0,id=cxl.1 \
# -device cxl-rp,port=0,bus=cxl.1,id=root_port13,chassis=0,slot=2 \
# -device cxl-type3,bus=root_port13,volatile-memdev=vmem0,id=cxl-vmem0 \
# -M cxl-fmw.0.targets.0=cxl.1,cxl-fmw.0.size=4G

CXL_ARGS = -object memory-backend-file,id=cxl-mem1,share=on,mem-path=/tmp/cxltest.raw,size=256M \
-object memory-backend-file,id=cxl-mem2,share=on,mem-path=/tmp/cxltest2.raw,size=256M \
-object memory-backend-file,id=cxl-mem3,share=on,mem-path=/tmp/cxltest3.raw,size=256M \
-object memory-backend-file,id=cxl-mem4,share=on,mem-path=/tmp/cxltest4.raw,size=256M \
-object memory-backend-file,id=cxl-lsa1,share=on,mem-path=/tmp/lsa.raw,size=256M \
-object memory-backend-file,id=cxl-lsa2,share=on,mem-path=/tmp/lsa2.raw,size=256M \
-object memory-backend-file,id=cxl-lsa3,share=on,mem-path=/tmp/lsa3.raw,size=256M \
-object memory-backend-file,id=cxl-lsa4,share=on,mem-path=/tmp/lsa4.raw,size=256M \
-device pxb-cxl,bus_nr=12,bus=pcie.0,id=cxl.1 \
-device pxb-cxl,bus_nr=222,bus=pcie.0,id=cxl.2 \
-device cxl-rp,port=0,bus=cxl.1,id=root_port13,chassis=0,slot=2 \
-device cxl-type3,bus=root_port13,persistent-memdev=cxl-mem1,lsa=cxl-lsa1,id=cxl-pmem0 \
-device cxl-rp,port=1,bus=cxl.1,id=root_port14,chassis=0,slot=3 \
-device cxl-type3,bus=root_port14,persistent-memdev=cxl-mem2,lsa=cxl-lsa2,id=cxl-pmem1 \
-device cxl-rp,port=0,bus=cxl.2,id=root_port15,chassis=0,slot=5 \
-device cxl-type3,bus=root_port15,persistent-memdev=cxl-mem3,lsa=cxl-lsa3,id=cxl-pmem2 \
-device cxl-rp,port=1,bus=cxl.2,id=root_port16,chassis=0,slot=6 \
-device cxl-type3,bus=root_port16,persistent-memdev=cxl-mem4,lsa=cxl-lsa4,id=cxl-pmem3 \
-M cxl-fmw.0.targets.0=cxl.1,cxl-fmw.0.targets.1=cxl.2,cxl-fmw.0.size=4G,cxl-fmw.0.interleave-granularity=8k

X86_KERNEL_PLACE=arch/x86/boot/bzImage

BZIMAGE = $(IMAGE)/$(X86_KERNEL_PLACE)
#BZIMAGE = bzImage
MEMORY_SIZE = 1G

DISK=disk/ubuntu.raw
DISK2=disk/ubuntu2.raw
ISO_DISK=iso/ubuntu-24.04-live-server-amd64.iso

MINI_DISK=disk/mini.img
MINI_ISO_DISK=iso/mini.iso


NET_ARG = -net nic,model=e1000 -net user,hostfwd=tcp::2222-:22

VIRTIO_ARGS = -chardev socket,id=char0,path=/tmp/vhostqemu -device vhost-user-fs-pci,queue-size=1024,chardev=char0,tag=myfs -object memory-backend-file,id=mem,size=4G,mem-path=/dev/shm,share=on -numa node,memdev=mem -m 4G

default: vm0 

init:
	cd ./initramfs && find . | cpio -ov --format=newc | gzip -9 > ../$(INITRAMFS)

qemu:
	$(QEMU) \
	-kernel $(BZIMAGE) \
	-initrd $(INITRAMFS) \
	-machine accel=kvm:tcg -enable-kvm \
	-nographic -no-reboot -d guest_errors \
	-netdev user,id=net0,hostfwd=tcp:127.0.0.1:8080-:8080 -device e1000,netdev=net0 \
	-append "console=ttyS0 quiet rdinit=/init earlyprintk=serial,ttyS console=ttyS0 noapic norandmaps" -S -s

# -append "root=/dev/vda3 console=ttyS0 noapic" \

aos:
	qemu-system-i386 \
	-kernel /home/kamilu/linux/linux-4.4.6/arch/x86/boot/bzImage \
	-initrd /home/kamilu/aos/lab/rootfs/rootfs.img.gz \
	-append "earlyprintk=serial,ttyS0 console=ttyS0 noapic norandmaps root=/dev/ram rdinit=sbin/init" $(DEBUG) \
	-nographic -m 4G

# -kernel $(BZIMAGE) \
# -append "root=/dev/vda2 console=ttyS0 noapic init=/lib/systemd/systemd" \

disk:
	$(QEMU) \
		-m 4G \
		-kernel $(BZIMAGE) \
		-drive format=raw,file=$(DISK) \
		-append "root=/dev/sda2 console=ttyS0" \
		-nographic -no-reboot -d guest_errors -serial mon:stdio \
  		-netdev user,id=net0,hostfwd=tcp:127.0.0.1:2222-:22 -device e1000,netdev=net0

mini:
	$(QEMU) \
		-m 4G \
		-kernel $(BZIMAGE) \
		-drive format=qcow2,file=$(MINI_DISK),if=virtio \
		-append "root=/dev/vda1 console=ttyS0" -nographic \
		-$(NET_ARG)

minid:
	$(QEMU) \
		-drive format=qcow2,file=$(MINI_DISK) \
		-nographic \
		-$(NET_ARG)


vfs:
	$(QEMU) \
		-m 4G \
		-kernel $(BZIMAGE) \
		-drive format=raw,file=$(DISK),if=virtio \
		-append "root=/dev/vda2 console=ttyS0" \
		-nographic -no-reboot \
		$(VIRTIO_ARGS) $(DEBUG)

clean:
	rm initramfs.img

VTISM_BZIMAGE=/home/lzx/vtism/arch/x86/boot/bzImage
TPP_BZIMAGE=/home/lzx/Nomad/src/linux-5.13-rc6/arch/x86/boot/bzImage
NOMAD_BZIMAGE=/home/lzx/Nomad/src/nomad/arch/x86/boot/bzImage
MEMTIS_BZIMAGE=/home/lzx/memtis/linux/arch/x86/boot/bzImage
# BZIMAGE=$(TPP_BZIMAGE)
# BZIMAGE=$(BASIC_510_BZIMAGE)
# BZIMAGE=$(TPP_BZIMAGE)
# BZIMAGE=$(NOMAD_BZIMAGE)
# BZIMAGE=/home/lzx/code/linux-6.6/arch/x86/boot/bzImage

# BZIMAGE=/home/lzx/code/linux-5.4.49/arch/x86/boot/bzImage

k=vtism
# if k is not set, use default kernel
BZIMAGE=$(shell ./get_kernel.sh $(k))

vtism:
	taskset -c 48-55 $(QEMU) \
	-kernel $(BZIMAGE) \
	-drive format=raw,file=$(DISK) \
	-append "root=/dev/sda2 console=ttyS0 quiet" \
	-nographic -no-reboot -d guest_errors \
	-net nic -net user,hostfwd=tcp::2222-:22 \
	-cpu host \
	-enable-kvm \
	-m 32G -smp 8 -device virtio-balloon-pci,id=balloon0,bus=pci.0,addr=0x7 -S -s
	

vm_share:
	taskset -c 48-55 $(QEMU) -name guest=vm0,debug-threads=off \
    -machine pc \
    -cpu host \
    -m 120G \
    -enable-kvm\
    -overcommit mem-lock=off \
    -smp 8 \
    -object memory-backend-ram,size=120G,host-nodes=0,policy=bind,prealloc=no,id=m0 \
    -numa node,nodeid=0,memdev=m0 \
    -uuid 9bc02bdb-58b3-4bb0-b00e-313bdae0ac81 \
    -device ich9-usb-ehci1,id=usb,bus=pci.0,addr=0x5.0x7 \
    -device virtio-serial-pci,id=virtio-serial0,bus=pci.0,addr=0x6 \
    -drive file=$(DISK),format=raw,if=none,id=drive-ide0-0-0 \
    -device ide-hd,bus=ide.0,unit=0,drive=drive-ide0-0-0,id=ide0-0-0,bootindex=1 \
    -drive if=none,id=drive-ide0-0-1,readonly=on \
    -device ide-cd,bus=ide.0,unit=1,drive=drive-ide0-0-1,id=ide0-0-1 \
    -device virtio-balloon-pci,id=balloon0,bus=pci.0,addr=0x7 \
    -netdev user,id=ndev.0,hostfwd=tcp::5555-:22 \
    -device e1000,netdev=ndev.0 \
    -nographic \
	-kernel $(BZIMAGE) \
    -append "root=/dev/sda2 console=ttyS0 quiet"

SHARED_MEM = -object memory-backend-file,id=shmem1,share=on,mem-path=/dev/shm/my_shm,size=64M \
    -device ivshmem-plain,memdev=shmem1,id=ivshmem1,bus=pci.0,addr=0xb

SHARED_MEM1 = -object memory-backend-file,id=shmem1,share=on,mem-path=/dev/shm/my_shm1,size=64M \
    -device ivshmem-plain,memdev=shmem1,id=ivshmem1,bus=pci.0,addr=0xb

NODE=1

vm0:
	taskset -c 0-15 $(QEMU) -name guest=vm0,debug-threads=off \
    -machine pc \
    -cpu host \
    -m 32G \
    -enable-kvm \
    -overcommit mem-lock=off \
    -smp 16,sockets=1,cores=16,threads=1 \
    -device ich9-usb-ehci1,id=usb,bus=pci.0,addr=0x5.0x7 \
    -device virtio-serial-pci,id=virtio-serial0,bus=pci.0,addr=0x6 \
    -drive file=$(DISK),format=raw,id=drive-ide0-0-0,if=none \
    -device ide-hd,bus=ide.0,unit=0,drive=drive-ide0-0-0,id=ide0-0-0,bootindex=1 \
    -drive if=none,id=drive-ide0-0-1,readonly=on \
    -device ide-cd,bus=ide.0,unit=1,drive=drive-ide0-0-1,id=ide0-0-1 \
    -device virtio-balloon-pci,id=balloon0,bus=pci.0,addr=0x7 \
    -netdev user,id=ndev.0,hostfwd=tcp::5555-:22 \
    -device e1000,netdev=ndev.0 \
    -nographic \
	-kernel $(BZIMAGE) \
    -append "root=/dev/sda2 console=ttyS0 quiet" \
	$(SHARED_MEM)

vm1:
	taskset -c 48-63 $(QEMU) -name guest=vm1,debug-threads=off \
    -machine pc \
    -cpu host \
    -m 64G \
    -enable-kvm \
    -overcommit mem-lock=off \
    -smp 16,sockets=1,cores=16,threads=1 \
    -device ich9-usb-ehci1,id=usb,bus=pci.0,addr=0x5.0x7 \
    -device virtio-serial-pci,id=virtio-serial0,bus=pci.0,addr=0x6 \
    -drive file=$(DISK2),format=raw,id=drive-ide0-0-0,if=none \
    -device ide-hd,bus=ide.0,unit=0,drive=drive-ide0-0-0,id=ide0-0-0,bootindex=1 \
    -drive if=none,id=drive-ide0-0-1,readonly=on \
    -device ide-cd,bus=ide.0,unit=1,drive=drive-ide0-0-1,id=ide0-0-1 \
    -device virtio-balloon-pci,id=balloon0,bus=pci.0,addr=0x7 \
    -netdev user,id=ndev.0,hostfwd=tcp::4444-:22 \
    -device e1000,netdev=ndev.0 \
    -nographic \
	-kernel $(BZIMAGE) \
    -append "root=/dev/sda2 console=ttyS0 quiet" \
	$(SHARED_MEM1)


vnuma:
	$(QEMU) -name guest=vm0,debug-threads=off \
    -machine pc \
    -cpu host \
    -enable-kvm \
    -m 32G \
    -overcommit mem-lock=off \
    -smp 8 \
    -object memory-backend-ram,size=8G,host-nodes=0,policy=bind,prealloc=no,id=m0 \
    -object memory-backend-ram,size=8G,host-nodes=1,policy=bind,prealloc=no,id=m1 \
	-object memory-backend-ram,size=8G,host-nodes=2,policy=bind,prealloc=no,id=m2 \
    -object memory-backend-ram,size=8G,host-nodes=3,policy=bind,prealloc=no,id=m3 \
    -numa node,nodeid=0,memdev=m0,cpus=0-3 \
    -numa node,nodeid=1,memdev=m1,cpus=4-7 \
	-numa node,nodeid=2,memdev=m2 \
    -numa node,nodeid=3,memdev=m3 \
	-numa dist,src=0,dst=0,val=10 \
    -numa dist,src=0,dst=1,val=21 \
    -numa dist,src=0,dst=2,val=24 \
    -numa dist,src=0,dst=3,val=24 \
    -numa dist,src=1,dst=0,val=21 \
    -numa dist,src=1,dst=1,val=10 \
    -numa dist,src=1,dst=2,val=14 \
    -numa dist,src=1,dst=3,val=14 \
    -numa dist,src=2,dst=0,val=24 \
    -numa dist,src=2,dst=1,val=14 \
    -numa dist,src=2,dst=2,val=10 \
    -numa dist,src=2,dst=3,val=16 \
    -numa dist,src=3,dst=0,val=24 \
    -numa dist,src=3,dst=1,val=14 \
    -numa dist,src=3,dst=2,val=16 \
    -numa dist,src=3,dst=3,val=10 \
    -uuid 9bc02bdb-58b3-4bb0-b00e-313bdae0ac81 \
    -device ich9-usb-ehci1,id=usb,bus=pci.0,addr=0x5.0x7 \
    -device virtio-serial-pci,id=virtio-serial0,bus=pci.0,addr=0x6 \
    -drive file=$(DISK),format=raw,id=drive-ide0-0-0,if=none \
    -device ide-hd,bus=ide.0,unit=0,drive=drive-ide0-0-0,id=ide0-0-0,bootindex=1 \
    -drive if=none,id=drive-ide0-0-1,readonly=on \
    -device ide-cd,bus=ide.0,unit=1,drive=drive-ide0-0-1,id=ide0-0-1 \
    -device virtio-balloon-pci,id=balloon0,bus=pci.0,addr=0x7 \
    -netdev user,id=ndev.0,hostfwd=tcp::5555-:22 \
    -device e1000,netdev=ndev.0 \
    -nographic \
	-kernel $(BZIMAGE) \
    -append "root=/dev/sda2 console=ttyS0 quiet"

debug:
	taskset -c 0-7 $(QEMU) -name guest=vm0 \
    -machine pc \
    -m 16G \
    -smp 8 \
    -uuid 9bc02bdb-58b3-4bb0-b00e-313bdae0ac81 \
    -drive file=$(DISK),format=raw \
    -netdev user,id=ndev.0,hostfwd=tcp::5555-:22 \
    -device e1000,netdev=ndev.0 \
    -nographic \
	-kernel $(BZIMAGE) \
    -append "root=/dev/sda2 console=ttyS0 quiet nokaslr" -S -s


debug_numa:
	taskset -c 0-15 $(QEMU) -name guest=vm0 \
    -machine pc \
    -m 8G \
    -overcommit mem-lock=off \
    -smp 16 \
    -object memory-backend-ram,size=2G,host-nodes=0,policy=bind,prealloc=no,id=m0 \
    -object memory-backend-ram,size=2G,host-nodes=1,policy=bind,prealloc=no,id=m1 \
	-object memory-backend-ram,size=2G,host-nodes=2,policy=bind,prealloc=no,id=m2 \
    -object memory-backend-ram,size=2G,host-nodes=3,policy=bind,prealloc=no,id=m3 \
    -numa node,nodeid=0,memdev=m0,cpus=0-7 \
    -numa node,nodeid=1,memdev=m1,cpus=8-15 \
	-numa node,nodeid=2,memdev=m2 \
    -numa node,nodeid=3,memdev=m3 \
	-numa dist,src=0,dst=0,val=10 \
    -numa dist,src=0,dst=1,val=21 \
    -numa dist,src=0,dst=2,val=24 \
    -numa dist,src=0,dst=3,val=24 \
    -numa dist,src=1,dst=0,val=21 \
    -numa dist,src=1,dst=1,val=10 \
    -numa dist,src=1,dst=2,val=14 \
    -numa dist,src=1,dst=3,val=14 \
    -numa dist,src=2,dst=0,val=24 \
    -numa dist,src=2,dst=1,val=14 \
    -numa dist,src=2,dst=2,val=10 \
    -numa dist,src=2,dst=3,val=16 \
    -numa dist,src=3,dst=0,val=24 \
    -numa dist,src=3,dst=1,val=14 \
    -numa dist,src=3,dst=2,val=16 \
    -numa dist,src=3,dst=3,val=10 \
    -uuid 9bc02bdb-58b3-4bb0-b00e-313bdae0ac81 \
    -device ich9-usb-ehci1,id=usb,bus=pci.0,addr=0x5.0x7 \
    -device virtio-serial-pci,id=virtio-serial0,bus=pci.0,addr=0x6 \
    -drive file=$(DISK),format=raw,id=drive-ide0-0-0,if=none \
    -device ide-hd,bus=ide.0,unit=0,drive=drive-ide0-0-0,id=ide0-0-0,bootindex=1 \
    -drive if=none,id=drive-ide0-0-1,readonly=on \
    -device ide-cd,bus=ide.0,unit=1,drive=drive-ide0-0-1,id=ide0-0-1 \
    -device virtio-balloon-pci,id=balloon0,bus=pci.0,addr=0x7 \
    -netdev user,id=ndev.0,hostfwd=tcp::5555-:22 \
    -device e1000,netdev=ndev.0 \
    -nographic \
	-kernel $(BZIMAGE) \
    -append "root=/dev/sda2 console=ttyS0 quiet nokaslr" -S -s
